#pragma once
#pragma execution_character_set("utf-8")

#include <QObject>
#include <QMediaPlayer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "MusicInfoData.h"
#include "CacheDownloader.h"

// MusicPlayer类负责管理缓存和本地播放
class MusicPlayer : public QObject
{
    Q_OBJECT

public:
    MusicPlayer(QObject* parent = nullptr);
    ~MusicPlayer();

    void openUrl(const QString& url); // 打开在线音乐文件
    void play(); // 播放音乐
    void pause(); // 暂停播放
    void stop(); // 停止播放
    void setVolume(int volume); // 设置音量
    void setPosition(qint64 position); // 设置播放进度
    void setCurrentMusic(const MusicInfoData& music); // 设置当前播放音乐

signals:
    void downloadFinished(const MusicInfoData& music); // 下载完成信号

    //用于改变前端
    void playbackStateChanged(QMediaPlayer::State state); // 播放状态改变信号
    void positionChanged(qint64 position); // 播放进度变化信号
    void playbackEnded(); // 播放结束信号
    void playMusicChanged(const MusicInfoData& music); // 歌曲改变信号

private slots:
    void onDownloadFinished(const QString& localPath, const QString& id); // 处理下载完成
    void onPositionChanged(qint64 position); // 处理播放进度变化
    void onStateChanged(QMediaPlayer::State state); // 处理播放状态变化
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status); // 处理媒体状态变化

private:
    CacheDownloader* cacheDownloader; // 缓存下载器
    QMediaPlayer* player; // 媒体播放器
    QString currentUrl; // 当前URL
    MusicInfoData currentMusic; // 当前播放的音乐信息
};
