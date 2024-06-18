#include "MusicPlayer.h"
#include <QFileInfo>
#include <QDebug>
#include <QUrl>
#include <QDir>
#include <QStandardPaths>
#include "MusicInfoManager.h"

// ���캯��
MusicPlayer::MusicPlayer(QObject* parent)
    : QObject(parent)
{
    player = new QMediaPlayer(this);
    cacheDownloader = new CacheDownloader(this);

    connect(cacheDownloader, &CacheDownloader::downloadFinished, this, &MusicPlayer::onDownloadFinished);
    connect(player, &QMediaPlayer::positionChanged, this, &MusicPlayer::onPositionChanged);
    connect(player, &QMediaPlayer::stateChanged, this, &MusicPlayer::onStateChanged);
    connect(player, &QMediaPlayer::mediaStatusChanged, this, &MusicPlayer::onMediaStatusChanged);

    qDebug() << "MusicPlayer: ���캯��ִ�����";
}

// ��������
MusicPlayer::~MusicPlayer()
{
    delete player;
    delete cacheDownloader;
    qDebug() << "MusicPlayer: ��������ִ�����";
}

// ��������
void MusicPlayer::play()
{
    qDebug() << "��ǰ��������" << " " << currentUrl;
    player->play();
    qDebug() << "MusicPlayer: ��������";
}

// ��ͣ����
void MusicPlayer::pause()
{
    player->pause();
    qDebug() << "MusicPlayer: ��ͣ����";
}

// ֹͣ����
void MusicPlayer::stop()
{
    player->stop();
    qDebug() << "MusicPlayer: ֹͣ����";
}

// �������ļ��������Ǳ��ػ�����URL��
void MusicPlayer::openUrl(const QString& url)
{
    qDebug() << "MusicPlayer: �������ļ�:" << url;
    currentUrl = url;

    QUrl qurl(url);
    if (qurl.isLocalFile() || QFileInfo::exists(url)) {
        // �������ļ�
        qDebug() << "MusicPlayer: �򿪱����ļ�:" << url;
        player->setMedia(QUrl::fromLocalFile(url));
        play();
        emit playMusicChanged(currentMusic);
    }
    else {
        // ���������ļ�
        qDebug() << "MusicPlayer: �����������ļ�:" << url;
        // ��黺�����Ƿ����и��ļ�
        QByteArray cachedData = cacheDownloader->loadFromCache(url, currentMusic.id);
        if (!cachedData.isEmpty()) {
            // ʹ�û�������
            QString cacheFilePath = cacheDownloader->generateCacheFilePath(url, currentMusic.id);
            player->setMedia(QUrl::fromLocalFile(cacheFilePath));
            play();
            emit playMusicChanged(currentMusic);
            qDebug() << "MusicPlayer: ʹ�û����ļ� - " << cacheFilePath;
        }
        else {
            qDebug() << "MusicPlayer: �ļ�δ���棬��ʼ����: " << url;
            // �����ļ�
            cacheDownloader->download(url, currentMusic.id);
        }
    }
}

// �����������
void MusicPlayer::onDownloadFinished(const QString& localPath, const QString& id)
{
    if (id == currentMusic.id) {
        qDebug() << "MusicPlayer: ������� - " << localPath;
        player->setMedia(QUrl::fromLocalFile(localPath));
        play();
        emit downloadFinished(currentMusic);
    }
}

// ��������
void MusicPlayer::setVolume(int volume)
{
    player->setVolume(volume);
    qDebug() << "MusicPlayer: ��������Ϊ" << volume;
}

// ���ò��Ž���
void MusicPlayer::setPosition(qint64 position)
{
    player->setPosition(position);
    qDebug() << "MusicPlayer: ���ò��Ž���Ϊ" << position << "����";
}

// ���õ�ǰ��������
void MusicPlayer::setCurrentMusic(const MusicInfoData& music)
{
    qDebug() << "MusicPlayer: ���õ�ǰ�������� - " << music.id;
    currentMusic = music;
    openUrl(music.filePath); // �������ļ���URL
    emit playMusicChanged(music);
}

// �����Ž��ȱ仯
void MusicPlayer::onPositionChanged(qint64 position)
{
    emit positionChanged(position);
    qDebug() << "MusicPlayer: ���Ž��ȱ仯 - " << position << "����";
}

// ������״̬�仯
void MusicPlayer::onStateChanged(QMediaPlayer::State state)
{
    emit playbackStateChanged(state);
    qDebug() << "MusicPlayer: ����״̬�仯 - " << state;
}

// ����ý��״̬�仯
void MusicPlayer::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia) {
        emit playbackEnded();
        qDebug() << "MusicPlayer: ���Ž���";
    }
    qDebug() << "MusicPlayer: ý��״̬�仯 - " << status;
}
