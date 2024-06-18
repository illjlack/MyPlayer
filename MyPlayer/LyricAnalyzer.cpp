#include "LyricAnalyzer.h"
#include <QRegularExpression>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QTextStream>

LyricAnalyzer::LyricAnalyzer(QObject* parent)
    : QObject(parent)
{
    cacheDownloader = new CacheDownloader(this);
    connect(cacheDownloader, &CacheDownloader::downloadFinished, this, &LyricAnalyzer::onLyricsDownloaded);
}

LyricAnalyzer::~LyricAnalyzer()
{
    // CacheDownloader 由父对象自动删除，无需显式删除
}

// 加载并解析歌词的方法
void LyricAnalyzer::loadLyrics(const MusicInfoData& music)
{
    m_lyricsMap.clear();  // 清空当前的歌词映射
    m_timeKeys.clear();  // 清空时间标签列表
    qDebug() << "loadLyrics:加载歌词" << music.id;
    if (music.isLocal) {
        // 加载本地歌词文件
        QFile file(music.lyric);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            in.setCodec("UTF-8"); // 设置为UTF-8编码
            QString lyrics = in.readAll();
            file.close();
            parseLyrics(lyrics);
        }
    }
    else {
        // 使用 CacheDownloader 下载和缓存歌词文件
        cacheDownloader->download(music.lyric, music.id);
    }
}

// 处理歌词下载完成的槽函数
void LyricAnalyzer::onLyricsDownloaded(const QString& localPath, const QString& id)
{
    QFile file(localPath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        in.setCodec("UTF-8"); // 设置为UTF-8编码
        QString lyrics = in.readAll();
        file.close();
        parseLyrics(lyrics);
    }
}

// 解析歌词内容的方法
void LyricAnalyzer::parseLyrics(const QString& lyrics)
{
    // 去除转义字符和多余的空白字符
    QString cleanedLyrics = lyrics;
    cleanedLyrics.replace("\\r", "\r");
    cleanedLyrics.replace("\\n", "\n");
    cleanedLyrics.replace("\\\\", "\\");

    // 使用正则表达式分割字符串，处理不同的换行符
    QStringList lines = cleanedLyrics.split(QRegularExpression("[\r\n]+"));

    QRegularExpression re("\\[(\\d+):(\\d+)\\.(\\d+)\\](.*)");
    // 逐行读取歌词文本
    for (const QString& line : lines) {
        QRegularExpressionMatch match = re.match(line);
        // 如果匹配成功，解析时间标签和对应的歌词文本
        if (match.hasMatch()) {
            qint64 minutes = match.captured(1).toInt();
            qint64 seconds = match.captured(2).toInt();
            qint64 milliseconds = match.captured(3).toInt();
            QString text = match.captured(4).trimmed();

            // 计算时间标签的毫秒值并存储在映射中
            qint64 time = (minutes * 60 + seconds) * 1000 + milliseconds;
            m_lyricsMap[time] = text;
            m_timeKeys.append(time);
        }
    }
    emit lyricsChanged(0); // 初始更新歌词显示
}

// 更新显示歌词的方法
void LyricAnalyzer::updateLyrics(qint64 position)
{
    QString displayLyrics = "<html><body style='text-align:center;'>";

    // 二分查找当前播放位置对应的歌词索引
    auto it = std::lower_bound(m_timeKeys.begin(), m_timeKeys.end(), position);
    int currentIndex = std::distance(m_timeKeys.begin(), it);
    if (currentIndex == m_timeKeys.size() || (currentIndex > 0 && m_timeKeys[currentIndex] > position)) {
        currentIndex--;
    }

    int range = 2; // 显示当前歌词以及其前后两行
    int start = std::max(0, currentIndex - range);
    int end = std::min(m_timeKeys.size() - 1, currentIndex + range);

    for (int i = start; i <= end; ++i) {
        if (i == currentIndex) {
            // 当前歌词，使用红色显示
            displayLyrics += "<font color='red'>" + m_lyricsMap[m_timeKeys[i]] + "</font><br>";
        }
        else {
            // 其他歌词，使用白色显示
            displayLyrics += "<font color='white'>" + m_lyricsMap[m_timeKeys[i]] + "</font><br>";
        }
    }

    displayLyrics += "</body></html>";

    emit lyricsChanged(displayLyrics);
}
