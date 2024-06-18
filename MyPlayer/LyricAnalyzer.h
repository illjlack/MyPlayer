#pragma once
#pragma execution_character_set("utf-8")

#include <QObject>
#include <QMap>
#include <QList>
#include "CacheDownloader.h"
#include "MusicInfoData.h"

class LyricAnalyzer : public QObject
{
    Q_OBJECT

public:
    LyricAnalyzer(QObject* parent = nullptr);
    ~LyricAnalyzer();

public slots:
    void loadLyrics(const MusicInfoData& music); // 加载歌词，解析时间标签并存储在QMap中
    void updateLyrics(qint64 position); // 更新歌词

signals:
    void lyricsChanged(const QString& lyricsHtml); // 歌词变化信号

private slots:
    void onLyricsDownloaded(const QString& localPath, const QString& id); // 处理歌词下载完成

private:
    void parseLyrics(const QString& lyrics); // 解析歌词内容

    QMap<qint64, QString> m_lyricsMap;  // 存储时间标签和对应歌词的映射
    QList<qint64> m_timeKeys;  // 存储所有时间标签的列表
    CacheDownloader* cacheDownloader; // 缓存下载器
};
