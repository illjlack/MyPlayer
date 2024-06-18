#pragma once
#pragma execution_character_set("utf-8")

#include <QObject>
#include "MusicLoader.h"
#include "MusicInfoManager.h"

// 音乐列表管理类，负责管理本地音乐、远程音乐和喜爱列表
class MusicListManager : public QObject
{
    Q_OBJECT

public:
    MusicLoader* musicLoader;  // 音乐加载器

    explicit MusicListManager(QObject* parent = nullptr);

    // 从数据库加载所有音乐列表
    void loadAllMusic();

    // 加载本地音乐列表
    void loadLocalMusic(const QString& filePath);

    // 加载远程音乐列表
    void loadRemoteMusic(const QString& url);

    // 加载喜爱列表
    void loadFavoriteMusic();

    // 添加到喜爱列表
    void addToFavorites(const MusicInfoData& music);

    // 从喜爱列表移除
    void removeFromFavorites(const MusicInfoData& music);

signals:
    // 信号，当音乐列表更新时发出
    void localMusicLoaded(const QList<MusicInfoData>& musicList);
    void remoteMusicLoaded(const QList<MusicInfoData>& musicList);
    void favoriteMusicLoaded(const QList<MusicInfoData>& musicList);

public slots:
    void onLoadLocalMusic();

private slots:
    // 槽函数，处理音乐数据加载完成
    void onMusicDataLoaded(const QList<MusicInfoData>& musicList);

private:
    
    MusicInfoManager& musicInfoManager; // 音乐信息管理器
    QList<MusicInfoData> localMusicList;  // 本地音乐列表
    QList<MusicInfoData> remoteMusicList; // 远程音乐列表
    QList<MusicInfoData> favoriteMusicList; // 喜爱音乐列表
};
