#pragma once
#pragma execution_character_set("utf-8")

#include <QObject>
#include <QList>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include "MusicPlayer.h"
#include "LyricAnalyzer.h"
#include "CommentEnum.h"
#include "MusicInfoData.h"

class MusicPlayerList : public QObject
{
    Q_OBJECT

public:
    MusicPlayerList(QObject* parent = nullptr);
    ~MusicPlayerList();

signals:
    void playlistChanged(const QList<MusicInfoData>& playlist); // 播放列表状态改变信号
    void playModeChanged(PlayMode mode); // 播放模式改变信号
    void currentMusicChanged(const MusicInfoData& music); // 当前播放音乐改变信号
    void positionChanged(qint64 position); // 播放进度变化信号
    void lyricsChanged(const QString& lyricsHtml);//歌词渲染改变
    void playStateChange(bool playing);//是否播放，给前端

public slots:
    void openUrl(const MusicInfoData& music); // 打开在线音乐文件
    void setPlaylist(const QList<MusicInfoData>& playlist); // 设置播放列表
    void addMusic(const MusicInfoData& music); // 添加音乐
    void addAndPlayMusic(const MusicInfoData& music);// 添加并播放音乐
    
    void play(); // 播放音乐
    void pause(); // 暂停播放
    void stop(); // 停止播放
    void playNext(); // 播放下一首
    void playPrevious(); // 播放上一首
    void setVolume(int volume); // 设置音量
    void setPosition(qint64 position); // 设置播放进度
    void loadLyrics(const MusicInfoData& music); // 加载歌词文件
    void changePlayMode(PlayMode playMode); // 改变播放模式
    void playStateChanged(bool playing);

    void setPlaylistPosition(const MusicInfoData& music);//设置当前播放音乐

private slots:
    void onPositionChanged(qint64 position); // 处理播放位置变化
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status); // 处理媒体状态变化
    void onPlaylistPositionChanged(int position); // 处理播放列表位置变化

private:
    MusicPlayer* musicPlayer; // 音乐播放实例
    LyricAnalyzer* lyricAnalyzer; // 歌词管理实例
    QMediaPlaylist* playlist; // 播放列表
    QList<MusicInfoData> playlistData; // 播放列表数据集合
    int currentPlaylistIndex; // 当前播放列表索引
};
