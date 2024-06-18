#pragma once
#pragma execution_character_set("utf-8")

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <QMediaPlayer>
#include <QEventLoop>
#include "MusicInfoManager.h"
#include "MusicInfoData.h"
#include "CacheDownloader.h"

// MusicLoader类，负责从URL加载和解析音乐数据，并下载相关文件
class MusicLoader : public QObject
{
    Q_OBJECT

public:
    explicit MusicLoader(QObject* parent = nullptr);

    // 从指定URL加载音乐数据
    void loadFromUrl(const QString& url);

    // 从本地选择音乐文件并加载
    void loadFromLocalFile(const QString& filePath);

    // 从本地JSON文件加载音乐数据
    void loadFromLocalJson(const QString& filePath);

signals:
    // 信号，在音乐数据加载完成时发出
    void musicDataLoaded(const QList<MusicInfoData>& musicList);
    void musicAddLocal();
    void musicAddRemote();
    void result(bool ok);

private slots:
    // 槽函数，处理JSON回复完成
    void onJsonReplyFinished(QNetworkReply* reply);

private:
    // 解析音乐数据
    void parseMusicData(const QJsonArray& musicArray, const QString& baseUrl, bool isLocal);

    // 获取音乐文件的实际时长
    void fetchMusicDuration(const QString& filePath, MusicInfoData& music);

    QNetworkAccessManager* networkManager;  // 网络管理器，用于发送HTTP请求
    QList<MusicInfoData> musicList;         // 音乐列表，存储解析后的音乐数据
    MusicInfoManager& musicInfoManager;     // 数据库管理器实例
    CacheDownloader* cacheDownloader;
};
