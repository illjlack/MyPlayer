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

// 底部栏
class BottomWidget : public QFrame
{
    Q_OBJECT
public:
    explicit BottomWidget(QWidget* parent = nullptr); // 构造函数

signals:
    void playStateChanged(bool playing); // 播放状态改变信号
    void volumeChanged(int volume); // 音量改变信号
    void playModeChanged(PlayMode mode); // 播放模式改变信号
    void playNextClicked(); // 下一首点击信号
    void playPreviousClicked(); // 上一首点击信号
    void openLyricsClicked(); // 打开歌词信号
    void showPlaylistClicked(); // 显示播放列表信号
    void progressChanged(int value); // 进度条改变信号
    void MusicChanged(const MusicInfoData& music); // 歌曲改变

public slots:
    void onMusicChanged(const MusicInfoData& music); // 处理歌曲改变的槽函数
    void onPlayStateChanged(bool playing); // 被动暂停/播放的槽函数
    void onProgressChanged(int value); // 被动更新进度的槽函数

private:
    void initWidget(); // 初始化部件
    QHBoxLayout* h_mainLayout, * m_hControlLayout; // 水平布局管理器
    bool m_playing; // 播放状态

    PlayMode m_playMode; // 当前播放模式

private slots:
    void updatePlayState(); // 更新播放状态的槽函数
    void updateVolume(int volume); // 更新音量的槽函数
    void changePlayMode(); // 改变播放模式的槽函数
    void onSliderClicked(int value); // 滑块点击的槽函数
    void updateStartTimeLabel(int value); // 更新开始时间标签的槽函数
    void toggleVolume(); // 切换音量的槽函数

public:
    QFrame* m_controlFrame; // 控制帧
    QPushButton* lastBtn, * playBtn, * nextBtn, * volumeBtn, * playModeBtn, * lyricBtn, * listBtn; // 按钮部件
    QLabel* startTimeLabel, * endTimeLabel, * bottomRightLabel; // 标签部件
    ClickableSlider* playTimeSlider, * volumeSlider; // 滑动条部件
    PlaySongInfoWidget* m_PlaySongInfoWidget; // 播放歌曲信息组件
};
