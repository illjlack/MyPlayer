#pragma once
#pragma execution_character_set("utf-8")

#include <QObject>
#include <QMediaPlayer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "MusicInfoData.h"
#include "CacheDownloader.h"

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

signals:
    void downloadFinished(const MusicInfoData& music); // ��������ź�

    //���ڸı�ǰ��
    void playbackStateChanged(QMediaPlayer::State state); // ����״̬�ı��ź�
    void positionChanged(qint64 position); // ���Ž��ȱ仯�ź�
    void playbackEnded(); // ���Ž����ź�
    void playMusicChanged(const MusicInfoData& music); // �����ı��ź�

private slots:
    void onDownloadFinished(const QString& localPath, const QString& id); // �����������
    void onPositionChanged(qint64 position); // �����Ž��ȱ仯
    void onStateChanged(QMediaPlayer::State state); // ������״̬�仯
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status); // ����ý��״̬�仯

private:
    CacheDownloader* cacheDownloader; // ����������
    QMediaPlayer* player; // ý�岥����
    QString currentUrl; // ��ǰURL
    MusicInfoData currentMusic; // ��ǰ���ŵ�������Ϣ
};
