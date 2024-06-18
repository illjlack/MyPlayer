#pragma once
#pragma execution_character_set("utf-8")

#include <QString>
#include <QMetaType>

// 音乐信息结构体，用于存储音乐数据
struct MusicInfoData {
    QString id;         // 音乐唯一ID (hashKey)
    bool isLocal;       // 是否为本地音乐
    QString musicName;  // 音乐名称
    QString albumName;  // 专辑名称
    int duration;       // 持续时间（秒）
    QString path;       // 基础路径或URL
    QString filePath;   // 文件路径或URL
    QString lyric;      // 歌词文件路径或URL
    QString img;        // 图片文件路径或URL
    QString fileType;   // 文件类型（例如 mp3, wav, flac 等）
    bool isLove;        // 是否喜爱

    MusicInfoData()
        : isLocal(false),
        isLove(false),
        duration(0) {}

    void generateId() {
        id = musicName + "_" + albumName + "_" + (isLocal ? '1' : '0') + "_"+QString::number(duration) ;
    }

    // 重载 == 操作符，QList需要
    bool operator==(const MusicInfoData& other) const {
        return id == other.id;
    }
};

// 将自定义数据类型注册为元类型
Q_DECLARE_METATYPE(MusicInfoData)
