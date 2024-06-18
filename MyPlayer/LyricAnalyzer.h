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
    void loadLyrics(const MusicInfoData& music); // ���ظ�ʣ�����ʱ���ǩ���洢��QMap��
    void updateLyrics(qint64 position); // ���¸��

signals:
    void lyricsChanged(const QString& lyricsHtml); // ��ʱ仯�ź�

private slots:
    void onLyricsDownloaded(const QString& localPath, const QString& id); // �������������

private:
    void parseLyrics(const QString& lyrics); // �����������

    QMap<qint64, QString> m_lyricsMap;  // �洢ʱ���ǩ�Ͷ�Ӧ��ʵ�ӳ��
    QList<qint64> m_timeKeys;  // �洢����ʱ���ǩ���б�
    CacheDownloader* cacheDownloader; // ����������
};
