#include "MusicPlayer_temp.h"
#include <QNetworkRequest>
#include <QFileInfo>
#include <QDebug>
#include <QUrl>
#include <QUuid>
#include <QSet>
#include <QFile>

// 构造函数
MusicPlayer::MusicPlayer(QObject* parent)
    : QObject(parent),
    bufferPosition(0),
    fileSize(0),
    totalDuration(0),
    cache(CACHE_CAPACITY), // 最多可以缓存100个项, QCache在达到容量上限时会自动删除最少使用的项
    currentBuffer(nullptr)
{
    player = new QMediaPlayer(this);
    networkManager = new QNetworkAccessManager(this);

    //connect(player, &QMediaPlayer::positionChanged, this, &MusicPlayer::positionChanged);
    //connect(player, &QMediaPlayer::durationChanged, this, &MusicPlayer::durationChanged);
    connect(player, &QMediaPlayer::bufferStatusChanged, this, &MusicPlayer::onBufferStatusChanged);
    connect(player, &QMediaPlayer::mediaStatusChanged, this, &MusicPlayer::onMediaStatusChanged);
    connect(networkManager, &QNetworkAccessManager::finished, this, &MusicPlayer::onNetworkReply);
}

// 析构函数
MusicPlayer::~MusicPlayer()
{
    delete player;
    delete networkManager;
    if (currentBuffer) {
        currentBuffer->close();
        delete currentBuffer;
    }
}

// 播放音乐
void MusicPlayer::play()
{
    player->play();
}

// 暂停播放
void MusicPlayer::pause()
{
    player->pause();
}

// 停止播放
void MusicPlayer::stop()
{
    player->stop();
    if (currentBuffer) {
        currentBuffer->close();
        delete currentBuffer;
        currentBuffer = nullptr;
    }
    bufferingSet.clear();
}

// 发送HEAD请求获取文件大小
void MusicPlayer::sendHeadRequest(const QString& url)
{
    qDebug() << "发送HEAD请求获取文件大小=====" << url;
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    QNetworkReply* reply = networkManager->head(request);
    reply->setProperty("requestId", currentRequestId);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {this->onHeadRequestFinished(reply); });
}

// 处理HEAD请求的响应
void MusicPlayer::onHeadRequestFinished(QNetworkReply* reply)
{
    QString requestId = reply->property("requestId").toString();
    if (requestId != currentRequestId) {
        qWarning() << "HEAD request with stale ID received, ignoring.";
        reply->deleteLater();
        return;
    }

    if (reply->error() == QNetworkReply::NoError) {
        QVariant contentLength = reply->header(QNetworkRequest::ContentLengthHeader);
        if (contentLength.isValid()) {
            fileSize = contentLength.toLongLong();
            qDebug() << "File size:" << fileSize;

            // 继续处理文件下载或缓存
            prepareBuffer();
            downloadRange(0);
        }
    }
    else {
        qWarning() << "HEAD request error:" << reply->errorString();
    }
    reply->deleteLater();
}

// 打开在线音乐文件
void MusicPlayer::openUrl(const QString& url)
{
    qDebug() << "打开在线音乐文件:" << url;
    currentUrl = url;
    bufferPosition = 0;
    currentRequestId = generateRequestId();

    // 检查本地文件是否存在
    QFileInfo fileInfo(url);
    if (fileInfo.exists() && fileInfo.isFile()) {
        QFile file(url);
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray fileData = file.readAll();
            file.close();
            fileSize = file.size(); // 获取文件大小
            totalDuration = player->duration(); // 获取音频总时长
            saveToCache(url, 0, fileData); // 将本地文件保存到缓存
            useCachedData(std::move(fileData)); // 使用缓存数据
            return;
        }
    }

    // 发送HEAD请求获取文件大小
    sendHeadRequest(url);

    // 检查缓存中是否有该文件的相关位置的数据
    QByteArray cachedData = loadFromCache(url, bufferPosition);

    if (!cachedData.isEmpty()) {
        // 使用缓存数据
        useCachedData(std::move(cachedData));
    }
    else {
        // 如果缓存中没有，开始下载

        qDebug() << "下载 0";
        prepareBuffer();
        downloadRange(0);
    }
}

// 设置音量
void MusicPlayer::setVolume(int volume)
{
    player->setVolume(volume);
}

// 设置播放进度
void MusicPlayer::setPosition(qint64 position)
{
    if (fileSize > 0 && totalDuration > 0) {
        // 计算位置在文件中的实际字节偏移
        bufferPosition = (position * fileSize) / totalDuration;

        // 确保bufferPosition是BUFFER_SIZE的倍数
        bufferPosition = (bufferPosition / BUFFER_SIZE) * BUFFER_SIZE;

        // 设置播放器的播放位置
        player->setPosition(position);

        // 生成新的请求ID
        currentRequestId = generateRequestId();

        // 检查是否已经在缓冲区集合中
        if (!bufferingSet.contains(generateBufferSetKey(bufferPosition))) {
            // 如果缓冲区中没有数据，则下载所需的范围数据
            downloadRange(bufferPosition);
        }

        // 更新当前缓冲区内容
        QByteArray cachedData = loadFromCache(currentUrl, bufferPosition);
        if (!cachedData.isEmpty()) {
            // 使用缓存数据
            useCachedData(std::move(cachedData));
        }
    }
}

// 下载指定范围的数据
void MusicPlayer::downloadRange(qint64 start)
{
    qDebug() << "下载缓存=======" << start;
    QString bufferSetKey = generateBufferSetKey(start);
    if (bufferingSet.contains(bufferSetKey)) {
        return;
    }

    QNetworkRequest request;
    request.setUrl(QUrl(currentUrl));
    QString rangeHeaderValue = QString("bytes=%1-%2").arg(start).arg(start + BUFFER_SIZE - 1);
    request.setRawHeader("Range", rangeHeaderValue.toUtf8());
    QNetworkReply* reply = networkManager->get(request);
    reply->setProperty("requestId", currentRequestId);
    reply->setProperty("bufferStart", start);
    bufferingSet.insert(bufferSetKey);
}

// 处理网络回复
void MusicPlayer::onNetworkReply(QNetworkReply* reply)
{
    QString requestId = reply->property("requestId").toString();
    qDebug() << "下载完成=======" << requestId << ' ' << currentRequestId;
    if (requestId != currentRequestId) {
        qWarning() << "Network reply with stale ID received, ignoring.";
        reply->deleteLater();
        return;
    }

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        qint64 start = reply->property("bufferStart").toLongLong();

        bufferingSet.remove(generateBufferSetKey(start));
        saveToCache(currentUrl, start, data);
        appendToBuffer(std::move(data));
    }
    else {
        qWarning() << "Network error:" << reply->errorString();
    }
    reply->deleteLater();
}

// 处理缓冲状态改变
void MusicPlayer::onBufferStatusChanged(int percent)
{
    static const int BUFFER_THRESHOLD = 50;
    if (percent < BUFFER_THRESHOLD) {
        bufferPosition = (player->position() * fileSize / totalDuration / BUFFER_SIZE) * BUFFER_SIZE + BUFFER_SIZE;
        if (!bufferingSet.contains(generateBufferSetKey(bufferPosition))) {
            downloadRange(bufferPosition);
        }
    }
}

// 处理媒体状态改变
void MusicPlayer::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia) {
        stop();
    }
}

// 将数据保存到缓存
void MusicPlayer::saveToCache(const QString& url, qint64 position, const QByteArray& data)
{
    QString cacheKey = generateCacheKey(url, position);
    if (!cache.contains(cacheKey)) {
        cache.insert(cacheKey, new QByteArray(data));
    }
}

// 从缓存中加载数据
QByteArray MusicPlayer::loadFromCache(const QString& url, qint64 position)
{
    QString cacheKey = generateCacheKey(url, position);
    if (cache.contains(cacheKey)) {
        return *cache.object(cacheKey);
    }
    return QByteArray();
}

// 生成缓存键
inline QString MusicPlayer::generateCacheKey(const QString& url, qint64 position) const
{
    QString fileName = QFileInfo(url).fileName();
    qint64 blockIndex = position / BUFFER_SIZE;
    return QString("%1_%2").arg(fileName).arg(blockIndex);
}

// 生成唯一请求标识符
inline QString MusicPlayer::generateRequestId() const
{
    return QUuid::createUuid().toString();
}

// 将数据追加到缓冲区
void MusicPlayer::appendToBuffer(QByteArray&& data)
{
    if (currentBuffer) {
        qint64 pos = currentBuffer->pos();
        currentBuffer->write(data);
        currentBuffer->seek(pos);
        currentBuffer->open(QIODevice::ReadOnly);
    }
    qDebug() << "缓冲完成";
    play();
}

// 使用缓存数据
void MusicPlayer::useCachedData(QByteArray&& data)
{
    if (currentBuffer) {
        currentBuffer->close();
        delete currentBuffer;
    }
    currentBuffer = new QBuffer();
    currentBuffer->setData(std::move(data));
    currentBuffer->open(QIODevice::ReadOnly);
    player->setMedia(QMediaContent(), currentBuffer);
    play();
}

// 准备缓冲区
void MusicPlayer::prepareBuffer()
{
    if (currentBuffer) {
        currentBuffer->close();
        delete currentBuffer;
    }
    currentBuffer = new QBuffer();
    currentBuffer->open(QIODevice::ReadWrite);
    player->setMedia(QMediaContent(), currentBuffer);
}

// 解析Content-Range头部以获取起始位置
qint64 MusicPlayer::parseStartFromRangeHeader(const QByteArray& contentRange) const
{
    QList<QByteArray> parts = contentRange.split(' ');
    if (parts.size() == 2) {
        QList<QByteArray> rangeParts = parts[1].split('-');
        if (rangeParts.size() == 2) {
            return rangeParts[0].toLongLong();
        }
    }
    return -1;
}

// 生成缓冲区集合键
inline QString MusicPlayer::generateBufferSetKey(qint64 position) const
{
    return QString("%1_%2").arg(currentUrl).arg(position / BUFFER_SIZE);
}

// 设置当前播放音乐
void MusicPlayer::setCurrentMusic(const MusicInfoData& music)
{
    qDebug() << "设置当前播放音乐====" << music.id;
    currentMusic = music;
    openUrl(music.mp3); // 打开音乐文件的URL
}
