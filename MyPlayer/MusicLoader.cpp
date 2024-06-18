#include "MusicLoader.h"
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QMediaPlayer>
#include <QEventLoop>

// ���캯������ʼ������������ͻ���Ŀ¼
MusicLoader::MusicLoader(QObject* parent)
    : QObject(parent)
    , networkManager(new QNetworkAccessManager(this))
    , musicInfoManager(MusicInfoManager::instance()) // ʹ�õ���ģʽ��MusicInfoManagerʵ��
    , cacheDownloader(new CacheDownloader(this))
{
    // ���������������finished�źŵ���Ӧ�Ĳۺ���
    connect(networkManager, &QNetworkAccessManager::finished, this, &MusicLoader::onJsonReplyFinished);
}

void MusicLoader::loadFromUrl(const QString& url) {
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    qDebug() << "MusicLoader: ��������URL: " << request.url();

    // ��������� URL
    QUrl qurl(url);
    QString path = qurl.path();
    QStringList pathSegments = path.split('/', QString::SkipEmptyParts);

    // ���·���Ƿ����㹻�Ĳ��
    if (pathSegments.size() >= 2) {
        // ȥ���������·��
        pathSegments.removeLast();
        pathSegments.removeLast();
        QString basePath = "/" + pathSegments.join('/');
        QUrl baseUrl = qurl;
        baseUrl.setPath(basePath);
        qDebug() << "MusicLoader: Base URL: " << baseUrl.toString();

        // ������������ baseUrl ����
        QNetworkReply* reply = networkManager->get(request);
        reply->setProperty("baseUrl", baseUrl.toString());
    }
    else {
        // �������󲢽� baseUrl ����Ϊԭʼ URL ��Ϊ���˷���
        QNetworkReply* reply = networkManager->get(request);
        reply->setProperty("baseUrl", url);
    }
}

void MusicLoader::loadFromLocalFile(const QString& filePath) {
    qDebug() << "MusicLoader: ��������== " << filePath;
    QFile file(filePath);
    if (!file.exists()) {
        qDebug() << "MusicLoader: �ļ������� - " << filePath;
        return;
    }

    // ���� localmusic Ŀ¼
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QString localMusicDir = appDataPath + "/localmusic";
    QDir dir;
    if (!dir.exists(localMusicDir)) {
        dir.mkpath(localMusicDir);
    }

    // �����ļ��� localmusic Ŀ¼
    QString fileName = QFileInfo(filePath).fileName();
    QString newFilePath = localMusicDir + "/" + fileName;

    if (!QFile::copy(filePath, newFilePath)) {
        qDebug() << "MusicLoader: �ļ�����ʧ�� - " << filePath;
        return;
    }
    qDebug() << "MusicLoader: �������ֵ�== " << newFilePath;
    // ���� MusicInfoData
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
        qDebug() << "MusicLoader: �޷����ļ� - " << filePath;
        return;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    QJsonArray jsonArray = doc.object().value("list").toArray();

    // ��ȡ�ļ��Ļ�·�����˺��������
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
    qDebug() << "MusicLoader: �յ�JSON�ظ�";

    if (reply->error() != QNetworkReply::NoError) {
        // ������󣬴�ӡ������Ϣ
        qDebug() << "MusicLoader: ���ش��� - " << reply->errorString();
        reply->deleteLater();
        return;
    }

    QString baseUrl = reply->property("baseUrl").toString();

    // ��ȡ�ظ����ݲ�����JSON
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

        // �ж��ļ����Ͳ������ļ�·��
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

        // ����Ǳ������֣�����ļ�·���Ƿ����
        if (isLocal && !QFile::exists(music.filePath)) {
            qDebug() << "MusicLoader: �ļ������� - " << music.filePath;
            emit result(false);
            continue;
        }

        // ��鲢��ȡʵ��ʱ��
        if (music.duration == 0) {
            fetchMusicDuration(music.filePath, music);
        }
        else {
            musicList.append(music);
            qDebug() << "MusicLoader: ������� - " << music.musicName;
        }

        // (͵͵����)���û�����ȥ�����ļ�
        if (!isLocal) {
            cacheDownloader->download(music.filePath, music.id);
            cacheDownloader->download(music.lyric, music.id);
            cacheDownloader->download(music.img, music.id);
        }


        // ��������Ϣ��ӵ����ݿ�
        if (!musicInfoManager.addOrUpdateMusic(music)) {
            qDebug() << "MusicLoader: ��ӻ�������ֵ����ݿ�ʧ�� - " << music.musicName;
        }
        else {
            if (isLocal) {
                emit musicAddLocal();
                emit result(true);
            }
            else emit musicAddRemote();
        }


    }

    // �����������ݼ�������ź�
    emit musicDataLoaded(musicList);
}

void MusicLoader::fetchMusicDuration(const QString& filePath, MusicInfoData& music) {
    QMediaPlayer* player = new QMediaPlayer;
    QEventLoop* loop = new QEventLoop;

    connect(player, &QMediaPlayer::durationChanged, loop, [&](qint64 duration) {
        music.duration = duration / 1000; // ��������
        musicList.append(music);
        // ��������Ϣ��ӵ����ݿ�
        qDebug() << "MusicLoader: ����ʱ��ɹ� - " << music.musicName << "=" << duration / 1000;
        if (!musicInfoManager.addOrUpdateMusic(music)) {
            qDebug() << "MusicLoader: ��ӻ�������ֵ����ݿ�ʧ�� - " << music.musicName;
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
            qDebug() << "MusicLoader: ��Ч��ý���ļ� - " << filePath;
            loop->quit();
        }
        });

    player->setMedia(QUrl::fromLocalFile(filePath));
    player->play();
    loop->exec();

    delete player;
    delete loop;
}
