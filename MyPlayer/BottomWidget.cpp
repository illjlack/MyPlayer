#include "BottomWidget.h"

BottomWidget::BottomWidget(QWidget* parent) : QFrame(parent), m_playing(false), m_playMode(PlayMode::Loop) // 初始化播放模式为循环播放
{
    qDebug() << "BottomWidget: 初始化";

    // 设置底部控制栏的固定高度
    setFixedHeight(100);

    // 设置控制栏的样式
    setStyleSheet("QFrame{background:rgb(246,246,248);border-color:rgb(225,225,225);border-width:1px;border-style:solid;}");
    setMouseTracking(true); // 启用鼠标跟踪

    // 初始化小部件
    initWidget();

    connect(this, &BottomWidget::MusicChanged, m_PlaySongInfoWidget, &PlaySongInfoWidget::updateSongInfo);

    // 连接槽函数
    connect(volumeSlider, &QSlider::valueChanged, this, &BottomWidget::updateVolume);
    connect(playBtn, &QPushButton::clicked, this, &BottomWidget::updatePlayState);
    connect(playModeBtn, &QPushButton::clicked, this, &BottomWidget::changePlayMode); // 连接播放模式按钮的点击信号到槽函数

    connect(nextBtn, &QPushButton::clicked, this, &BottomWidget::playNextClicked);
    connect(lastBtn, &QPushButton::clicked, this, &BottomWidget::playPreviousClicked);

    //暂时没实现小窗口歌词，直接绑定到歌曲详细页
    //connect(lyricBtn, &QPushButton::clicked, this, &BottomWidget::openLyricsClicked);
    connect(lyricBtn, &QPushButton::clicked, this->m_PlaySongInfoWidget, &PlaySongInfoWidget::isToggleSongDetails);

    connect(listBtn, &QPushButton::clicked, this, &BottomWidget::showPlaylistClicked);

    // 连接进度条点击信号到槽函数
    connect(playTimeSlider, &ClickableSlider::sliderClicked, this, &BottomWidget::onSliderClicked);

    // 连接进度条值改变信号到槽函数
    connect(playTimeSlider, &QSlider::valueChanged, this, &BottomWidget::updateStartTimeLabel);

    // 连接音量按钮点击信号到槽函数
    connect(volumeBtn, &QPushButton::clicked, this, &BottomWidget::toggleVolume);
}

void BottomWidget::initWidget()
{
    qDebug() << "BottomWidget: 初始化小部件";

    // 创建布局管理器
    h_mainLayout = new QHBoxLayout(this);

    // 创建播放歌曲信息组件
    m_PlaySongInfoWidget = new PlaySongInfoWidget(this);
    h_mainLayout->addWidget(m_PlaySongInfoWidget);

    // 播放控制组
    QHBoxLayout* h_PlayControlsLayout = new QHBoxLayout(this);

    lastBtn = new QPushButton(this);
    lastBtn->setFixedSize(50, 50);
    lastBtn->setCursor(Qt::PointingHandCursor);
    lastBtn->setToolTip("上一个");
    lastBtn->setStyleSheet("QPushButton{border-image:url(:/images/BottomPlay/previous.png);}");

    playBtn = new QPushButton(this);
    playBtn->setFixedSize(60, 60);
    playBtn->setCursor(Qt::PointingHandCursor);
    playBtn->setToolTip("播放");
    playBtn->setStyleSheet("QPushButton{border-image:url(:/images/BottomPlay/play.png);}");

    nextBtn = new QPushButton(this);
    nextBtn->setFixedSize(50, 50);
    nextBtn->setCursor(Qt::PointingHandCursor);
    nextBtn->setToolTip("下一个");
    nextBtn->setStyleSheet("QPushButton{border-image:url(:/images/BottomPlay/next.png);}");

    h_PlayControlsLayout->addWidget(lastBtn);
    h_PlayControlsLayout->addWidget(playBtn);
    h_PlayControlsLayout->addWidget(nextBtn);

    // 进度控制组
    QHBoxLayout* h_ProgressControlsLayout = new QHBoxLayout(this);

    startTimeLabel = new QLabel("00:00", this);

    playTimeSlider = new ClickableSlider(Qt::Horizontal, this);
    playTimeSlider->setFixedHeight(14);
    playTimeSlider->setRange(0, 0);
    playTimeSlider->setSingleStep(1);
    // 样式表在 ClickableSlider 类中已设置，因此此处无需重复设置

    endTimeLabel = new QLabel("00:00", this);

    h_ProgressControlsLayout->addWidget(startTimeLabel);
    h_ProgressControlsLayout->addWidget(playTimeSlider);
    h_ProgressControlsLayout->addWidget(endTimeLabel);

    // 播放控制和进度
    QVBoxLayout* v_PlayControlsLayout = new QVBoxLayout(this);
    v_PlayControlsLayout->addLayout(h_PlayControlsLayout);
    v_PlayControlsLayout->addLayout(h_ProgressControlsLayout);
    h_mainLayout->addLayout(v_PlayControlsLayout);

    // 音量控制组
    QHBoxLayout* h_VolumeControlsLayout = new QHBoxLayout(this);
    h_mainLayout->addLayout(h_VolumeControlsLayout);

    volumeBtn = new QPushButton(this);
    volumeBtn->setFixedSize(50, 50);
    volumeBtn->setCursor(Qt::PointingHandCursor);
    volumeBtn->setToolTip("大音量");
    volumeBtn->setStyleSheet("QPushButton{border-image:url(:/images/BottomPlay/high_volume.png);}");

    volumeSlider = new ClickableSlider(Qt::Horizontal, this);
    volumeSlider->setFixedSize(100, 12);
    volumeSlider->setToolTip("音量控制");
    volumeSlider->setMinimum(0);
    volumeSlider->setMaximum(100);
    volumeSlider->setValue(100);
    // 样式表在 ClickableSlider 类中已设置，因此此处无需重复设置

    h_VolumeControlsLayout->addWidget(volumeBtn);
    h_VolumeControlsLayout->addWidget(volumeSlider);

    // 其他功能组
    QHBoxLayout* h_AdditionalFeaturesLayout = new QHBoxLayout(this);
    h_mainLayout->addLayout(h_AdditionalFeaturesLayout);

    playModeBtn = new QPushButton(this);
    playModeBtn->setFixedSize(50, 50);
    playModeBtn->setCursor(Qt::PointingHandCursor);
    playModeBtn->setToolTip("循环播放");
    playModeBtn->setStyleSheet("QPushButton{border-image:url(:/images/BottomPlay/loop.png);}");

    lyricBtn = new QPushButton(this);
    lyricBtn->setFixedSize(50, 50);
    lyricBtn->setCursor(Qt::PointingHandCursor);
    lyricBtn->setToolTip("打开歌词\n(暂时没实现小窗口歌词，直接绑定到歌曲详细页)");
    lyricBtn->setStyleSheet("QPushButton{border-image:url(:/images/BottomPlay/lyric.png);}");

    listBtn = new QPushButton(this);
    listBtn->setFixedSize(50, 50);
    listBtn->setCursor(Qt::PointingHandCursor);
    listBtn->setToolTip("当前播放列表");
    listBtn->setStyleSheet("QPushButton{border-image:url(:/images/BottomPlay/currentList.png);}");

    h_AdditionalFeaturesLayout->addWidget(playModeBtn);
    h_AdditionalFeaturesLayout->addWidget(lyricBtn);
    h_AdditionalFeaturesLayout->addWidget(listBtn);

    // 设置主布局参数
    h_mainLayout->setMargin(10);
    h_mainLayout->setSpacing(10);
}

// 主动暂停/播放
void BottomWidget::updatePlayState()
{
    qDebug() << "BottomWidget: 更新播放状态";

    if (m_playing)
    {
        playBtn->setStyleSheet("QPushButton{border-image:url(:/images/BottomPlay/play.png);}");
        playBtn->setToolTip("播放");
        m_playing = false;
    }
    else
    {
        playBtn->setStyleSheet("QPushButton{border-image:url(:/images/BottomPlay/pause.png);}");
        playBtn->setToolTip("暂停");
        m_playing = true;
    }
    emit playStateChanged(m_playing); // 发射播放状态改变信号
}

// 被动暂停/播放
void BottomWidget::onPlayStateChanged(bool playing) {
    qDebug() << "BottomWidget: 被动暂停/播放" << "=====" << playing;

    if (playing)
    {
        playBtn->setStyleSheet("QPushButton{border-image:url(:/images/BottomPlay/pause.png);}");
        playBtn->setToolTip("暂停");
    }
    else
    {
        playBtn->setStyleSheet("QPushButton{border-image:url(:/images/BottomPlay/play.png);}");
        playBtn->setToolTip("播放");
    }
}

// 更新音量的槽函数实现
void BottomWidget::updateVolume(int volume)
{
    qDebug() << "BottomWidget: 更新音量" << volume;

    if (volume == 0)
    {
        volumeBtn->setStyleSheet("QPushButton{border-image:url(:/images/BottomPlay/mute.png);}");
        volumeBtn->setToolTip("静音");
    }
    else if (volume < 50)
    {
        volumeBtn->setStyleSheet("QPushButton{border-image:url(:/images/BottomPlay/low_volume.png);}");
        volumeBtn->setToolTip("小音量");
    }
    else
    {
        volumeBtn->setStyleSheet("QPushButton{border-image:url(:/images/BottomPlay/high_volume.png);}");
        volumeBtn->setToolTip("大音量");
    }
    emit volumeChanged(volume); // 发射音量改变信号
}

// 改变播放模式的槽函数实现
void BottomWidget::changePlayMode()
{
    qDebug() << "BottomWidget: 改变播放模式";

    m_playMode = getNextPlayMode(m_playMode);
    switch (m_playMode)
    {
    case PlayMode::Sequential:
        playModeBtn->setStyleSheet("QPushButton{border-image:url(:/images/BottomPlay/sequence.png);}");
        playModeBtn->setToolTip("顺序播放");
        break;
    case PlayMode::Random:
        playModeBtn->setStyleSheet("QPushButton{border-image:url(:/images/BottomPlay/random.png);}");
        playModeBtn->setToolTip("随机播放");
        break;
    case PlayMode::SingleLoop:
        playModeBtn->setStyleSheet("QPushButton{border-image:url(:/images/BottomPlay/single_loop.png);}");
        playModeBtn->setToolTip("单曲循环");
        break;
    case PlayMode::Loop:
        playModeBtn->setStyleSheet("QPushButton{border-image:url(:/images/BottomPlay/loop.png);}");
        playModeBtn->setToolTip("循环播放");
        break;
    }
    emit playModeChanged(m_playMode); // 发射播放模式改变信号
}

// 滑块点击的槽函数实现
void BottomWidget::onSliderClicked(int value)
{
    qDebug() << "BottomWidget: 滑块点击" << value << "毫秒";
    emit progressChanged(value); // 发射进度条改变信号
}

// 处理歌曲改变的槽函数实现
void BottomWidget::onMusicChanged(const MusicInfoData& music)
{
    qDebug() << "BottomWidget: 歌曲改变" << music.musicName;

    // 更新进度条最大值
    playTimeSlider->setRange(0, music.duration * 1000); // 单位为毫秒
    playTimeSlider->setValue(0);

    // 更新开始和结束时间标签
    startTimeLabel->setText("00:00");
    int minutes = music.duration / 60;
    int seconds = music.duration % 60;
    QString endTimeText = QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
    endTimeLabel->setText(endTimeText);

    emit MusicChanged(music);
}

// 更新开始时间标签的槽函数实现
void BottomWidget::updateStartTimeLabel(int value)
{
    qDebug() << "BottomWidget: 更新开始时间标签" << value;

    int minutes = value / 60000; // 分钟
    int seconds = (value / 1000) % 60; // 秒
    QString startTimeText = QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
    startTimeLabel->setText(startTimeText);
}

// 切换音量的槽函数实现
void BottomWidget::toggleVolume()
{
    qDebug() << "BottomWidget: 切换音量";

    int volume = volumeSlider->value();
    if (volume != 0) {
        volumeSlider->setValue(0);
    }
    else {
        volumeSlider->setValue(50);
    }
    updateVolume(volumeSlider->value());
}

// 被动更新进度的槽函数实现
void BottomWidget::onProgressChanged(int value)
{
    qDebug() << "BottomWidget: 被动更新进度" << value;

    playTimeSlider->setValue(value);
    updateStartTimeLabel(value);
}
