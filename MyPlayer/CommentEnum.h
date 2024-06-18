#pragma once
#pragma execution_character_set("utf-8")

#include <QObject>
#include <QMediaPlaylist>

// 自定义角色枚举
enum CustomRoles {
    MusicInfoDataRole = Qt::UserRole + 1
};

// 播放模式枚举
enum class PlayMode {
    SingleLoop = 1,
    Sequential = 2,
    Random = 3,
    Loop = 4 
};

// 获取下一个播放模式的函数声明
PlayMode getNextPlayMode(PlayMode currentMode);

// 转换为QMediaPlaylist播放模式的函数声明
QMediaPlaylist::PlaybackMode convertToQMediaPlaylistPlayMode(PlayMode mode);
