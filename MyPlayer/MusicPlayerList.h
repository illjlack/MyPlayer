#pragma once
#pragma execution_character_set("utf-8")

#include <QObject>
#include <QList>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include "MusicPlayer.h"
#include "LyricAnalyzer.h"
#include "CommentEnum.h"
#include "MusicInfoData.h"

class MusicPlayerList : public QObject
{
    Q_OBJECT

public:
    MusicPlayerList(QObject* parent = nullptr);
    ~MusicPlayerList();

signals:
    void playlistChanged(const QList<MusicInfoData>& playlist); // �����б�״̬�ı��ź�
    void playModeChanged(PlayMode mode); // ����ģʽ�ı��ź�
    void currentMusicChanged(const MusicInfoData& music); // ��ǰ�������ָı��ź�
    void positionChanged(qint64 position); // ���Ž��ȱ仯�ź�
    void lyricsChanged(const QString& lyricsHtml);//�����Ⱦ�ı�
    void playStateChange(bool playing);//�Ƿ񲥷ţ���ǰ��

public slots:
    void openUrl(const MusicInfoData& music); // �����������ļ�
    void setPlaylist(const QList<MusicInfoData>& playlist); // ���ò����б�
    void addMusic(const MusicInfoData& music); // �������
    void addAndPlayMusic(const MusicInfoData& music);// ��Ӳ���������
    
    void play(); // ��������
    void pause(); // ��ͣ����
    void stop(); // ֹͣ����
    void playNext(); // ������һ��
    void playPrevious(); // ������һ��
    void setVolume(int volume); // ��������
    void setPosition(qint64 position); // ���ò��Ž���
    void loadLyrics(const MusicInfoData& music); // ���ظ���ļ�
    void changePlayMode(PlayMode playMode); // �ı䲥��ģʽ
    void playStateChanged(bool playing);

    void setPlaylistPosition(const MusicInfoData& music);//���õ�ǰ��������

private slots:
    void onPositionChanged(qint64 position); // ������λ�ñ仯
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status); // ����ý��״̬�仯
    void onPlaylistPositionChanged(int position); // �������б�λ�ñ仯

private:
    MusicPlayer* musicPlayer; // ���ֲ���ʵ��
    LyricAnalyzer* lyricAnalyzer; // ��ʹ���ʵ��
    QMediaPlaylist* playlist; // �����б�
    QList<MusicInfoData> playlistData; // �����б����ݼ���
    int currentPlaylistIndex; // ��ǰ�����б�����
};
