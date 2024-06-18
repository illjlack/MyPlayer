#pragma once
#pragma execution_character_set("utf-8")

#include <QObject>
#include <QMediaPlayer>
#include <QBuffer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QCache>
#include "MusicInfoData.h"

#define BUFFER_SIZE 1024 * 1024 // 1 MB
#define CACHE_CAPACITY 100      // ��������

// MusicPlayer�ฺ�������ͱ��ز���
class MusicPlayer : public QObject
{
    Q_OBJECT

public:
    MusicPlayer(QObject* parent = nullptr);
    ~MusicPlayer();

    void openUrl(const QString& url); // �����������ļ�
    void play(); // ��������
    void pause(); // ��ͣ����
    void stop(); // ֹͣ����
    void setVolume(int volume); // ��������
    void setPosition(qint64 position); // ���ò��Ž���
    void setCurrentMusic(const MusicInfoData& music); // ���õ�ǰ��������

public slots:
    //void positionChanged(qint64 position); // ����λ�ñ仯�ź�
    //void durationChanged(qint64 duration); // ����ʱ���仯�ź�

private slots:
    void onNetworkReply(QNetworkReply* reply); // ��������ظ�
    void onBufferStatusChanged(int percent); // ������״̬�ı�
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status); // ����ý��״̬�ı�
    void onHeadRequestFinished(QNetworkReply* reply); // ����HEAD�������Ӧ

private:
    void sendHeadRequest(const QString& url); // ����HEAD����
    void downloadRange(qint64 start); // ����ָ����Χ������     
    void saveToCache(const QString& url, qint64 position, const QByteArray& data); // �����ݱ��浽����
    QByteArray loadFromCache(const QString& url, qint64 position); // �ӻ����м�������
    inline QString generateCacheKey(const QString& url, qint64 position) const; // ���ɻ����
    inline QString generateRequestId() const; // ����Ψһ�����ʶ��
    void appendToBuffer(QByteArray&& data); // ������׷�ӵ�������
    void useCachedData(QByteArray&& data); // ʹ�û�������
    void prepareBuffer(); // ׼��������
    qint64 parseStartFromRangeHeader(const QByteArray& contentRange) const; // ����Content-Rangeͷ���Ի�ȡ��ʼλ��
    inline QString generateBufferSetKey(qint64 position) const; // ���ɻ��������ϼ�

    QMediaPlayer* player; // ý�岥����
    QNetworkAccessManager* networkManager; // ������ʹ�����
    QCache<QString, QByteArray> cache; // ����
    QBuffer* currentBuffer; // ��ǰ������
    QString currentUrl; // ��ǰURL
    QString currentRequestId; // ��ǰ����ID
    qint64 bufferPosition; // ����λ��
    qint64 fileSize; // �ļ����ܳ���
    qint64 totalDuration; // ��Ƶ����ʱ��
    QSet<QString> bufferingSet; // ����������
    MusicInfoData currentMusic; // ��ǰ���ŵ�������Ϣ
};
