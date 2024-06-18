#pragma once
#pragma execution_character_set("utf-8")

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include "MusicInfoData.h"

class MusicInfoManager : public QObject {
    Q_OBJECT

public:
    static MusicInfoManager& instance();
    ~MusicInfoManager();

    // 添加或更新音乐信息
    bool addOrUpdateMusic(const MusicInfoData& music);

    // 获取所有音乐信息
    QList<MusicInfoData> getAllMusic();

    // 获取所有喜爱的音乐信息
    QList<MusicInfoData> getLovedMusic();

    // 根据ID获取音乐信息
    MusicInfoData getMusicById(const QString& id);

    // 设置音乐的喜爱状态
    bool setLoveStatus(const QString& id, bool isLove);

    // 检查音乐是否存在
    bool isMusicExists(const QString& id);

private:
    explicit MusicInfoManager(QObject* parent = nullptr);
    QSqlDatabase db;

    void initDatabase();

    // 禁止拷贝和赋值
    MusicInfoManager(const MusicInfoManager&) = delete;
    MusicInfoManager& operator=(const MusicInfoManager&) = delete;

    void showError(const QString& message);
};
