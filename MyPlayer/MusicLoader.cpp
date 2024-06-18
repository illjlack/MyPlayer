#include "MusicLoader.h"
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QMediaPlayer>
#include <QEventLoop>

// 构造函数，初始化网络管理器和缓存目录
MusicLoader::MusicLoader(QObject* parent)
    : QObject(parent)
    , networkManager(new QNetworkAccessManager(this))
    , musicInfoManager(MusicInfoManager::instance()) // 使用单例模式的MusicInfoManager实例
    , cacheDownloader(new CacheDownloader(this))
{
    // 连接网络管理器的finished信号到相应的槽函数
    connect(networkManager, &QNetworkAccessManager::finished, this, &MusicLoader::onJsonReplyFinished);
}

void MusicLoader::loadFromUrl(const QString& url) {
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    qDebug() << "MusicLoader: 正在请求URL: " << request.url();

    // 解析传入的 URL
    QUrl qurl(url);
    QString path = qurl.path();
    QStringList pathSegments = path.split('/', QString::SkipEmptyParts);

    // 检查路径是否有足够的层次
    if (pathSegments.size() >= 2) {
        // 去掉最后两层路径
        pathSegments.removeLast();
        pathSegments.removeLast();
        QString basePath = "/" + pathSegments.join('/');
        QUrl baseUrl = qurl;
        baseUrl.setPath(basePath);
        qDebug() << "MusicLoader: Base URL: " << baseUrl.toString();

        // 发送请求并设置 baseUrl 属性
        QNetworkReply* reply = networkManager->get(request);
        reply->setProperty("baseUrl", baseUrl.toString());
    }
    else {
        // 发送请求并将 baseUrl 设置为原始 URL 作为回退方案
        QNetworkReply* reply = networkManager->get(request);
        reply->setProperty("baseUrl", url);
    }
}

void MusicLoader::loadFromLocalFile(const QString& filePath) {
    qDebug() << "MusicLoader: 加载音乐== " << filePath;
    QFile file(filePath);
    if (!file.exists()) {
        qDebug() << "MusicLoader: 文件不存在 - " << filePath;
        return;
    }

    // 创建 localmusic 目录
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QString localMusicDir = appDataPath + "/localmusic";
    QDir dir;
    if (!dir.exists(localMusicDir)) {
        dir.mkpath(localMusicDir);
    }

    // 复制文件到 localmusic 目录
    QString fileName = QFileInfo(filePath).fileName();
    QString newFilePath = localMusicDir + "/" + fileName;

    if (!QFile::copy(filePath, newFilePath)) {
        qDebug() << "MusicLoader: 文件复制失败 - " << filePath;
        return;
    }
    qDebug() << "MusicLoader: 保存音乐到== " << newFilePath;
    // 更新 MusicInfoData
    MusicInfoData music;
    music.isLocal = true;
    music.filePath = newFilePath;
    music.fileType = QFileInfo(newFilePath).suffix();
    music.musicName = QFileInfo(newFilePath).baseName();
    music.generateId();

    fetchMusicDuration(newFilePath, music);
}

void MusicLoader::loadFromLocalJson(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "MusicLoader: 无法打开文件 - " << filePath;
        return;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    QJsonArray jsonArray = doc.object().value("list").toArray();

    // 获取文件的基路径，退后两个层次
    QUrl fileUrl = QUrl::fromLocalFile(filePath);
    QString path = fileUrl.path();
    QStringList pathSegments = path.split('/', QString::SkipEmptyParts);

    QString basePath;
    if (pathSegments.size() >= 2) {
        pathSegments.removeLast();
        pathSegments.removeLast();
        basePath =  pathSegments.join('/');
    }
    else {
        basePath = "/";
    }

    parseMusicData(jsonArray, basePath, true);
}

void MusicLoader::onJsonReplyFinished(QNetworkReply* reply) {
    qDebug() << "MusicLoader: 收到JSON回复";

    if (reply->error() != QNetworkReply::NoError) {
        // 处理错误，打印错误信息
        qDebug() << "MusicLoader: 下载错误 - " << reply->errorString();
        reply->deleteLater();
        return;
    }

    QString baseUrl = reply->property("baseUrl").toString();

    // 读取回复数据并解析JSON
    QByteArray responseData = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    QJsonObject rootObj = jsonDoc.object();
    QJsonArray musicArray = rootObj["list"].toArray();

    parseMusicData(musicArray, baseUrl, false);

    reply->deleteLater();
}

void MusicLoader::parseMusicData(const QJsonArray& musicArray, const QString& baseUrl, bool isLocal) {
    for (const QJsonValue& value : musicArray) {
        QJsonObject obj = value.toObject();
        MusicInfoData music;
        music.isLocal = isLocal;
        music.musicName = obj["musicName"].toString();
        music.albumName = obj["albumName"].toString();
        music.duration = obj["duration"].toInt();
        music.path = baseUrl + obj["path"].toString();

        // 判断文件类型并设置文件路径
        if (obj.contains("mp3")) {
            music.filePath = music.path + "/" + obj["mp3"].toString();
            music.fileType = "mp3";
        }
        else if (obj.contains("wav")) {
            music.filePath = music.path + "/" + obj["wav"].toString();
            music.fileType = "wav";
        }
        else if (obj.contains("flac")) {
            music.filePath = music.path + "/" + obj["flac"].toString();
            music.fileType = "flac";
        }

        music.lyric = music.path + "/" + obj["lyric"].toString();
        music.img = music.path + "/" + obj["img"].toString();

        music.generateId();

        // 如果是本地音乐，检查文件路径是否存在
        if (isLocal && !QFile::exists(music.filePath)) {
            qDebug() << "MusicLoader: 文件不存在 - " << music.filePath;
            emit result(false);
            continue;
        }

        // 检查并获取实际时长
        if (music.duration == 0) {
            fetchMusicDuration(music.filePath, music);
        }
        else {
            musicList.append(music);
            qDebug() << "MusicLoader: 处理完成 - " << music.musicName;
        }

        // (偷偷摸摸)调用缓存器去缓存文件
        if (!isLocal) {
            cacheDownloader->download(music.filePath, music.id);
            cacheDownloader->download(music.lyric, music.id);
            cacheDownloader->download(music.img, music.id);
        }


        // 将音乐信息添加到数据库
        if (!musicInfoManager.addOrUpdateMusic(music)) {
            qDebug() << "MusicLoader: 添加或更新音乐到数据库失败 - " << music.musicName;
        }
        else {
            if (isLocal) {
                emit musicAddLocal();
                emit result(true);
            }
            else emit musicAddRemote();
        }


    }

    // 发出音乐数据加载完成信号
    emit musicDataLoaded(musicList);
}

void MusicLoader::fetchMusicDuration(const QString& filePath, MusicInfoData& music) {
    QMediaPlayer* player = new QMediaPlayer;
    QEventLoop* loop = new QEventLoop;

    connect(player, &QMediaPlayer::durationChanged, loop, [&](qint64 duration) {
        music.duration = duration / 1000; // 设置秒数
        musicList.append(music);
        // 将音乐信息添加到数据库
        qDebug() << "MusicLoader: 解析时间成功 - " << music.musicName << "=" << duration / 1000;
        if (!musicInfoManager.addOrUpdateMusic(music)) {
            qDebug() << "MusicLoader: 添加或更新音乐到数据库失败 - " << music.musicName;
            emit result(false);
        }
        else {
            emit result(true);
            emit musicAddLocal();
        }

        loop->quit();
        });

    connect(player, &QMediaPlayer::mediaStatusChanged, loop, [&](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::InvalidMedia) {
            qDebug() << "MusicLoader: 无效的媒体文件 - " << filePath;
            loop->quit();
        }
        });

    player->setMedia(QUrl::fromLocalFile(filePath));
    player->play();
    loop->exec();

    delete player;
    delete loop;
}
