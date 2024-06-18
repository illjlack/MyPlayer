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
    // CacheDownloader �ɸ������Զ�ɾ����������ʽɾ��
}

// ���ز�������ʵķ���
void LyricAnalyzer::loadLyrics(const MusicInfoData& music)
{
    m_lyricsMap.clear();  // ��յ�ǰ�ĸ��ӳ��
    m_timeKeys.clear();  // ���ʱ���ǩ�б�
    qDebug() << "loadLyrics:���ظ��" << music.id;
    if (music.isLocal) {
        // ���ر��ظ���ļ�
        QFile file(music.lyric);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            in.setCodec("UTF-8"); // ����ΪUTF-8����
            QString lyrics = in.readAll();
            file.close();
            parseLyrics(lyrics);
        }
    }
    else {
        // ʹ�� CacheDownloader ���غͻ������ļ�
        cacheDownloader->download(music.lyric, music.id);
    }
}

// ������������ɵĲۺ���
void LyricAnalyzer::onLyricsDownloaded(const QString& localPath, const QString& id)
{
    QFile file(localPath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        in.setCodec("UTF-8"); // ����ΪUTF-8����
        QString lyrics = in.readAll();
        file.close();
        parseLyrics(lyrics);
    }
}

// ����������ݵķ���
void LyricAnalyzer::parseLyrics(const QString& lyrics)
{
    // ȥ��ת���ַ��Ͷ���Ŀհ��ַ�
    QString cleanedLyrics = lyrics;
    cleanedLyrics.replace("\\r", "\r");
    cleanedLyrics.replace("\\n", "\n");
    cleanedLyrics.replace("\\\\", "\\");

    // ʹ��������ʽ�ָ��ַ���������ͬ�Ļ��з�
    QStringList lines = cleanedLyrics.split(QRegularExpression("[\r\n]+"));

    QRegularExpression re("\\[(\\d+):(\\d+)\\.(\\d+)\\](.*)");
    // ���ж�ȡ����ı�
    for (const QString& line : lines) {
        QRegularExpressionMatch match = re.match(line);
        // ���ƥ��ɹ�������ʱ���ǩ�Ͷ�Ӧ�ĸ���ı�
        if (match.hasMatch()) {
            qint64 minutes = match.captured(1).toInt();
            qint64 seconds = match.captured(2).toInt();
            qint64 milliseconds = match.captured(3).toInt();
            QString text = match.captured(4).trimmed();

            // ����ʱ���ǩ�ĺ���ֵ���洢��ӳ����
            qint64 time = (minutes * 60 + seconds) * 1000 + milliseconds;
            m_lyricsMap[time] = text;
            m_timeKeys.append(time);
        }
    }
    emit lyricsChanged(0); // ��ʼ���¸����ʾ
}

// ������ʾ��ʵķ���
void LyricAnalyzer::updateLyrics(qint64 position)
{
    QString displayLyrics = "<html><body style='text-align:center;'>";

    // ���ֲ��ҵ�ǰ����λ�ö�Ӧ�ĸ������
    auto it = std::lower_bound(m_timeKeys.begin(), m_timeKeys.end(), position);
    int currentIndex = std::distance(m_timeKeys.begin(), it);
    if (currentIndex == m_timeKeys.size() || (currentIndex > 0 && m_timeKeys[currentIndex] > position)) {
        currentIndex--;
    }

    int range = 2; // ��ʾ��ǰ����Լ���ǰ������
    int start = std::max(0, currentIndex - range);
    int end = std::min(m_timeKeys.size() - 1, currentIndex + range);

    for (int i = start; i <= end; ++i) {
        if (i == currentIndex) {
            // ��ǰ��ʣ�ʹ�ú�ɫ��ʾ
            displayLyrics += "<font color='red'>" + m_lyricsMap[m_timeKeys[i]] + "</font><br>";
        }
        else {
            // ������ʣ�ʹ�ð�ɫ��ʾ
            displayLyrics += "<font color='white'>" + m_lyricsMap[m_timeKeys[i]] + "</font><br>";
        }
    }

    displayLyrics += "</body></html>";

    emit lyricsChanged(displayLyrics);
}
