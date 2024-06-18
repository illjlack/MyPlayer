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

// MusicLoader�࣬�����URL���غͽ����������ݣ�����������ļ�
class MusicLoader : public QObject
{
    Q_OBJECT

public:
    explicit MusicLoader(QObject* parent = nullptr);

    // ��ָ��URL������������
    void loadFromUrl(const QString& url);

    // �ӱ���ѡ�������ļ�������
    void loadFromLocalFile(const QString& filePath);

    // �ӱ���JSON�ļ�������������
    void loadFromLocalJson(const QString& filePath);

signals:
    // �źţ����������ݼ������ʱ����
    void musicDataLoaded(const QList<MusicInfoData>& musicList);
    void musicAddLocal();
    void musicAddRemote();
    void result(bool ok);

private slots:
    // �ۺ���������JSON�ظ����
    void onJsonReplyFinished(QNetworkReply* reply);

private:
    // ������������
    void parseMusicData(const QJsonArray& musicArray, const QString& baseUrl, bool isLocal);

    // ��ȡ�����ļ���ʵ��ʱ��
    void fetchMusicDuration(const QString& filePath, MusicInfoData& music);

    QNetworkAccessManager* networkManager;  // ��������������ڷ���HTTP����
    QList<MusicInfoData> musicList;         // �����б��洢���������������
    MusicInfoManager& musicInfoManager;     // ���ݿ������ʵ��
    CacheDownloader* cacheDownloader;
};
