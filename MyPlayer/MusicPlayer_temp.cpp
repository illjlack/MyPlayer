#include "MusicPlayer_temp.h"
#include <QNetworkRequest>
#include <QFileInfo>
#include <QDebug>
#include <QUrl>
#include <QUuid>
#include <QSet>
#include <QFile>

// ���캯��
MusicPlayer::MusicPlayer(QObject* parent)
    : QObject(parent),
    bufferPosition(0),
    fileSize(0),
    totalDuration(0),
    cache(CACHE_CAPACITY), // �����Ի���100����, QCache�ڴﵽ��������ʱ���Զ�ɾ������ʹ�õ���
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

// ��������
MusicPlayer::~MusicPlayer()
{
    delete player;
    delete networkManager;
    if (currentBuffer) {
        currentBuffer->close();
        delete currentBuffer;
    }
}

// ��������
void MusicPlayer::play()
{
    player->play();
}

// ��ͣ����
void MusicPlayer::pause()
{
    player->pause();
}

// ֹͣ����
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

// ����HEAD�����ȡ�ļ���С
void MusicPlayer::sendHeadRequest(const QString& url)
{
    qDebug() << "����HEAD�����ȡ�ļ���С=====" << url;
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    QNetworkReply* reply = networkManager->head(request);
    reply->setProperty("requestId", currentRequestId);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {this->onHeadRequestFinished(reply); });
}

// ����HEAD�������Ӧ
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

            // ���������ļ����ػ򻺴�
            prepareBuffer();
            downloadRange(0);
        }
    }
    else {
        qWarning() << "HEAD request error:" << reply->errorString();
    }
    reply->deleteLater();
}

// �����������ļ�
void MusicPlayer::openUrl(const QString& url)
{
    qDebug() << "�����������ļ�:" << url;
    currentUrl = url;
    bufferPosition = 0;
    currentRequestId = generateRequestId();

    // ��鱾���ļ��Ƿ����
    QFileInfo fileInfo(url);
    if (fileInfo.exists() && fileInfo.isFile()) {
        QFile file(url);
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray fileData = file.readAll();
            file.close();
            fileSize = file.size(); // ��ȡ�ļ���С
            totalDuration = player->duration(); // ��ȡ��Ƶ��ʱ��
            saveToCache(url, 0, fileData); // �������ļ����浽����
            useCachedData(std::move(fileData)); // ʹ�û�������
            return;
        }
    }

    // ����HEAD�����ȡ�ļ���С
    sendHeadRequest(url);

    // ��黺�����Ƿ��и��ļ������λ�õ�����
    QByteArray cachedData = loadFromCache(url, bufferPosition);

    if (!cachedData.isEmpty()) {
        // ʹ�û�������
        useCachedData(std::move(cachedData));
    }
    else {
        // ���������û�У���ʼ����

        qDebug() << "���� 0";
        prepareBuffer();
        downloadRange(0);
    }
}

// ��������
void MusicPlayer::setVolume(int volume)
{
    player->setVolume(volume);
}

// ���ò��Ž���
void MusicPlayer::setPosition(qint64 position)
{
    if (fileSize > 0 && totalDuration > 0) {
        // ����λ�����ļ��е�ʵ���ֽ�ƫ��
        bufferPosition = (position * fileSize) / totalDuration;

        // ȷ��bufferPosition��BUFFER_SIZE�ı���
        bufferPosition = (bufferPosition / BUFFER_SIZE) * BUFFER_SIZE;

        // ���ò������Ĳ���λ��
        player->setPosition(position);

        // �����µ�����ID
        currentRequestId = generateRequestId();

        // ����Ƿ��Ѿ��ڻ�����������
        if (!bufferingSet.contains(generateBufferSetKey(bufferPosition))) {
            // �����������û�����ݣ�����������ķ�Χ����
            downloadRange(bufferPosition);
        }

        // ���µ�ǰ����������
        QByteArray cachedData = loadFromCache(currentUrl, bufferPosition);
        if (!cachedData.isEmpty()) {
            // ʹ�û�������
            useCachedData(std::move(cachedData));
        }
    }
}

// ����ָ����Χ������
void MusicPlayer::downloadRange(qint64 start)
{
    qDebug() << "���ػ���=======" << start;
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

// ��������ظ�
void MusicPlayer::onNetworkReply(QNetworkReply* reply)
{
    QString requestId = reply->property("requestId").toString();
    qDebug() << "�������=======" << requestId << ' ' << currentRequestId;
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

// ������״̬�ı�
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

// ����ý��״̬�ı�
void MusicPlayer::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia) {
        stop();
    }
}

// �����ݱ��浽����
void MusicPlayer::saveToCache(const QString& url, qint64 position, const QByteArray& data)
{
    QString cacheKey = generateCacheKey(url, position);
    if (!cache.contains(cacheKey)) {
        cache.insert(cacheKey, new QByteArray(data));
    }
}

// �ӻ����м�������
QByteArray MusicPlayer::loadFromCache(const QString& url, qint64 position)
{
    QString cacheKey = generateCacheKey(url, position);
    if (cache.contains(cacheKey)) {
        return *cache.object(cacheKey);
    }
    return QByteArray();
}

// ���ɻ����
inline QString MusicPlayer::generateCacheKey(const QString& url, qint64 position) const
{
    QString fileName = QFileInfo(url).fileName();
    qint64 blockIndex = position / BUFFER_SIZE;
    return QString("%1_%2").arg(fileName).arg(blockIndex);
}

// ����Ψһ�����ʶ��
inline QString MusicPlayer::generateRequestId() const
{
    return QUuid::createUuid().toString();
}

// ������׷�ӵ�������
void MusicPlayer::appendToBuffer(QByteArray&& data)
{
    if (currentBuffer) {
        qint64 pos = currentBuffer->pos();
        currentBuffer->write(data);
        currentBuffer->seek(pos);
        currentBuffer->open(QIODevice::ReadOnly);
    }
    qDebug() << "�������";
    play();
}

// ʹ�û�������
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

// ׼��������
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

// ����Content-Rangeͷ���Ի�ȡ��ʼλ��
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

// ���ɻ��������ϼ�
inline QString MusicPlayer::generateBufferSetKey(qint64 position) const
{
    return QString("%1_%2").arg(currentUrl).arg(position / BUFFER_SIZE);
}

// ���õ�ǰ��������
void MusicPlayer::setCurrentMusic(const MusicInfoData& music)
{
    qDebug() << "���õ�ǰ��������====" << music.id;
    currentMusic = music;
    openUrl(music.mp3); // �������ļ���URL
}
