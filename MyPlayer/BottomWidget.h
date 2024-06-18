#pragma once
#pragma execution_character_set("utf-8")

#include <QFrame>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QHBoxLayout>
#include <QEvent>
#include <QMouseEvent>

#include "ClickableSlider.h"
#include "PlaySongInfoWidget.h"
#include "CommentEnum.h"
#include "MusicInfoData.h"

// �ײ���
class BottomWidget : public QFrame
{
    Q_OBJECT
public:
    explicit BottomWidget(QWidget* parent = nullptr); // ���캯��

signals:
    void playStateChanged(bool playing); // ����״̬�ı��ź�
    void volumeChanged(int volume); // �����ı��ź�
    void playModeChanged(PlayMode mode); // ����ģʽ�ı��ź�
    void playNextClicked(); // ��һ�׵���ź�
    void playPreviousClicked(); // ��һ�׵���ź�
    void openLyricsClicked(); // �򿪸���ź�
    void showPlaylistClicked(); // ��ʾ�����б��ź�
    void progressChanged(int value); // �������ı��ź�
    void MusicChanged(const MusicInfoData& music); // �����ı�

public slots:
    void onMusicChanged(const MusicInfoData& music); // ��������ı�Ĳۺ���
    void onPlayStateChanged(bool playing); // ������ͣ/���ŵĲۺ���
    void onProgressChanged(int value); // �������½��ȵĲۺ���

private:
    void initWidget(); // ��ʼ������
    QHBoxLayout* h_mainLayout, * m_hControlLayout; // ˮƽ���ֹ�����
    bool m_playing; // ����״̬

    PlayMode m_playMode; // ��ǰ����ģʽ

private slots:
    void updatePlayState(); // ���²���״̬�Ĳۺ���
    void updateVolume(int volume); // ���������Ĳۺ���
    void changePlayMode(); // �ı䲥��ģʽ�Ĳۺ���
    void onSliderClicked(int value); // �������Ĳۺ���
    void updateStartTimeLabel(int value); // ���¿�ʼʱ���ǩ�Ĳۺ���
    void toggleVolume(); // �л������Ĳۺ���

public:
    QFrame* m_controlFrame; // ����֡
    QPushButton* lastBtn, * playBtn, * nextBtn, * volumeBtn, * playModeBtn, * lyricBtn, * listBtn; // ��ť����
    QLabel* startTimeLabel, * endTimeLabel, * bottomRightLabel; // ��ǩ����
    ClickableSlider* playTimeSlider, * volumeSlider; // ����������
    PlaySongInfoWidget* m_PlaySongInfoWidget; // ���Ÿ�����Ϣ���
};
