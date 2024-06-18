#include "MusicPlayer.h"
#include <QFileInfo>
#include <QDebug>
#include <QUrl>
#include <QDir>
#include <QStandardPaths>
#include "MusicInfoManager.h"

// 构造函数
MusicPlayer::MusicPlayer(QObject* parent)
    : QObject(parent)
{
    player = new QMediaPlayer(this);
    cacheDownloader = new CacheDownloader(this);

    connect(cacheDownloader, &CacheDownloader::downloadFinished, this, &MusicPlayer::onDownloadFinished);
    connect(player, &QMediaPlayer::positionChanged, this, &MusicPlayer::onPositionChanged);
    connect(player, &QMediaPlayer::stateChanged, this, &MusicPlayer::onStateChanged);
    connect(player, &QMediaPlayer::mediaStatusChanged, this, &MusicPlayer::onMediaStatusChanged);

    qDebug() << "MusicPlayer: 构造函数执行完毕";
}

// 析构函数
MusicPlayer::~MusicPlayer()
{
    delete player;
    delete cacheDownloader;
    qDebug() << "MusicPlayer: 析构函数执行完毕";
}

// 播放音乐
void MusicPlayer::play()
{
    qDebug() << "当前播放音乐" << " " << currentUrl;
    player->play();
    qDebug() << "MusicPlayer: 播放音乐";
}

// 暂停播放
void MusicPlayer::pause()
{
    player->pause();
    qDebug() << "MusicPlayer: 暂停播放";
}

// 停止播放
void MusicPlayer::stop()
{
    player->stop();
    qDebug() << "MusicPlayer: 停止播放";
}

// 打开音乐文件（可以是本地或在线URL）
void MusicPlayer::openUrl(const QString& url)
{
    qDebug() << "MusicPlayer: 打开音乐文件:" << url;
    currentUrl = url;

    QUrl qurl(url);
    if (qurl.isLocalFile() || QFileInfo::exists(url)) {
        // 处理本地文件
        qDebug() << "MusicPlayer: 打开本地文件:" << url;
        player->setMedia(QUrl::fromLocalFile(url));
        play();
        emit playMusicChanged(currentMusic);
    }
    else {
        // 处理在线文件
        qDebug() << "MusicPlayer: 打开在线音乐文件:" << url;
        // 检查缓存中是否已有该文件
        QByteArray cachedData = cacheDownloader->loadFromCache(url, currentMusic.id);
        if (!cachedData.isEmpty()) {
            // 使用缓存数据
            QString cacheFilePath = cacheDownloader->generateCacheFilePath(url, currentMusic.id);
            player->setMedia(QUrl::fromLocalFile(cacheFilePath));
            play();
            emit playMusicChanged(currentMusic);
            qDebug() << "MusicPlayer: 使用缓存文件 - " << cacheFilePath;
        }
        else {
            qDebug() << "MusicPlayer: 文件未缓存，开始下载: " << url;
            // 下载文件
            cacheDownloader->download(url, currentMusic.id);
        }
    }
}

// 处理下载完成
void MusicPlayer::onDownloadFinished(const QString& localPath, const QString& id)
{
    if (id == currentMusic.id) {
        qDebug() << "MusicPlayer: 下载完成 - " << localPath;
        player->setMedia(QUrl::fromLocalFile(localPath));
        play();
        emit downloadFinished(currentMusic);
    }
}

// 设置音量
void MusicPlayer::setVolume(int volume)
{
    player->setVolume(volume);
    qDebug() << "MusicPlayer: 设置音量为" << volume;
}

// 设置播放进度
void MusicPlayer::setPosition(qint64 position)
{
    player->setPosition(position);
    qDebug() << "MusicPlayer: 设置播放进度为" << position << "毫秒";
}

// 设置当前播放音乐
void MusicPlayer::setCurrentMusic(const MusicInfoData& music)
{
    qDebug() << "MusicPlayer: 设置当前播放音乐 - " << music.id;
    currentMusic = music;
    openUrl(music.filePath); // 打开音乐文件的URL
    emit playMusicChanged(music);
}

// 处理播放进度变化
void MusicPlayer::onPositionChanged(qint64 position)
{
    emit positionChanged(position);
    qDebug() << "MusicPlayer: 播放进度变化 - " << position << "毫秒";
}

// 处理播放状态变化
void MusicPlayer::onStateChanged(QMediaPlayer::State state)
{
    emit playbackStateChanged(state);
    qDebug() << "MusicPlayer: 播放状态变化 - " << state;
}

// 处理媒体状态变化
void MusicPlayer::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia) {
        emit playbackEnded();
        qDebug() << "MusicPlayer: 播放结束";
    }
    qDebug() << "MusicPlayer: 媒体状态变化 - " << status;
}
