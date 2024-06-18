#include "BottomWidget.h"

BottomWidget::BottomWidget(QWidget* parent) : QFrame(parent), m_playing(false), m_playMode(PlayMode::Loop) // ��ʼ������ģʽΪѭ������
{
    qDebug() << "BottomWidget: ��ʼ��";

    // ���õײ��������Ĺ̶��߶�
    setFixedHeight(100);

    // ���ÿ���������ʽ
    setStyleSheet("QFrame{background:rgb(246,246,248);border-color:rgb(225,225,225);border-width:1px;border-style:solid;}");
    setMouseTracking(true); // ����������

    // ��ʼ��С����
    initWidget();

    connect(this, &BottomWidget::MusicChanged, m_PlaySongInfoWidget, &PlaySongInfoWidget::updateSongInfo);

    // ���Ӳۺ���
    connect(volumeSlider, &QSlider::valueChanged, this, &BottomWidget::updateVolume);
    connect(playBtn, &QPushButton::clicked, this, &BottomWidget::updatePlayState);
    connect(playModeBtn, &QPushButton::clicked, this, &BottomWidget::changePlayMode); // ���Ӳ���ģʽ��ť�ĵ���źŵ��ۺ���

    connect(nextBtn, &QPushButton::clicked, this, &BottomWidget::playNextClicked);
    connect(lastBtn, &QPushButton::clicked, this, &BottomWidget::playPreviousClicked);

    //��ʱûʵ��С���ڸ�ʣ�ֱ�Ӱ󶨵�������ϸҳ
    //connect(lyricBtn, &QPushButton::clicked, this, &BottomWidget::openLyricsClicked);
    connect(lyricBtn, &QPushButton::clicked, this->m_PlaySongInfoWidget, &PlaySongInfoWidget::isToggleSongDetails);

    connect(listBtn, &QPushButton::clicked, this, &BottomWidget::showPlaylistClicked);

    // ���ӽ���������źŵ��ۺ���
    connect(playTimeSlider, &ClickableSlider::sliderClicked, this, &BottomWidget::onSliderClicked);

    // ���ӽ�����ֵ�ı��źŵ��ۺ���
    connect(playTimeSlider, &QSlider::valueChanged, this, &BottomWidget::updateStartTimeLabel);

    // ����������ť����źŵ��ۺ���
    connect(volumeBtn, &QPushButton::clicked, this, &BottomWidget::toggleVolume);
}

void BottomWidget::initWidget()
{
    qDebug() << "BottomWidget: ��ʼ��С����";

    // �������ֹ�����
    h_mainLayout = new QHBoxLayout(this);

    // �������Ÿ�����Ϣ���
    m_PlaySongInfoWidget = new PlaySongInfoWidget(this);
    h_mainLayout->addWidget(m_PlaySongInfoWidget);

    // ���ſ�����
    QHBoxLayout* h_PlayControlsLayout = new QHBoxLayout(this);

    lastBtn = new QPushButton(this);
    lastBtn->setFixedSize(50, 50);
    lastBtn->setCursor(Qt::PointingHandCursor);
    lastBtn->setToolTip("��һ��");
    lastBtn->setStyleSheet("QPushButton{border-image:url(:/images/BottomPlay/previous.png);}");

    playBtn = new QPushButton(this);
    playBtn->setFixedSize(60, 60);
    playBtn->setCursor(Qt::PointingHandCursor);
    playBtn->setToolTip("����");
    playBtn->setStyleSheet("QPushButton{border-image:url(:/images/BottomPlay/play.png);}");

    nextBtn = new QPushButton(this);
    nextBtn->setFixedSize(50, 50);
    nextBtn->setCursor(Qt::PointingHandCursor);
    nextBtn->setToolTip("��һ��");
    nextBtn->setStyleSheet("QPushButton{border-image:url(:/images/BottomPlay/next.png);}");

    h_PlayControlsLayout->addWidget(lastBtn);
    h_PlayControlsLayout->addWidget(playBtn);
    h_PlayControlsLayout->addWidget(nextBtn);

    // ���ȿ�����
    QHBoxLayout* h_ProgressControlsLayout = new QHBoxLayout(this);

    startTimeLabel = new QLabel("00:00", this);

    playTimeSlider = new ClickableSlider(Qt::Horizontal, this);
    playTimeSlider->setFixedHeight(14);
    playTimeSlider->setRange(0, 0);
    playTimeSlider->setSingleStep(1);
    // ��ʽ���� ClickableSlider ���������ã���˴˴������ظ�����

    endTimeLabel = new QLabel("00:00", this);

    h_ProgressControlsLayout->addWidget(startTimeLabel);
    h_ProgressControlsLayout->addWidget(playTimeSlider);
    h_ProgressControlsLayout->addWidget(endTimeLabel);

    // ���ſ��ƺͽ���
    QVBoxLayout* v_PlayControlsLayout = new QVBoxLayout(this);
    v_PlayControlsLayout->addLayout(h_PlayControlsLayout);
    v_PlayControlsLayout->addLayout(h_ProgressControlsLayout);
    h_mainLayout->addLayout(v_PlayControlsLayout);

    // ����������
    QHBoxLayout* h_VolumeControlsLayout = new QHBoxLayout(this);
    h_mainLayout->addLayout(h_VolumeControlsLayout);

    volumeBtn = new QPushButton(this);
    volumeBtn->setFixedSize(50, 50);
    volumeBtn->setCursor(Qt::PointingHandCursor);
    volumeBtn->setToolTip("������");
    volumeBtn->setStyleSheet("QPushButton{border-image:url(:/images/BottomPlay/high_volume.png);}");

    volumeSlider = new ClickableSlider(Qt::Horizontal, this);
    volumeSlider->setFixedSize(100, 12);
    volumeSlider->setToolTip("��������");
    volumeSlider->setMinimum(0);
    volumeSlider->setMaximum(100);
    volumeSlider->setValue(100);
    // ��ʽ���� ClickableSlider ���������ã���˴˴������ظ�����

    h_VolumeControlsLayout->addWidget(volumeBtn);
    h_VolumeControlsLayout->addWidget(volumeSlider);

    // ����������
    QHBoxLayout* h_AdditionalFeaturesLayout = new QHBoxLayout(this);
    h_mainLayout->addLayout(h_AdditionalFeaturesLayout);

    playModeBtn = new QPushButton(this);
    playModeBtn->setFixedSize(50, 50);
    playModeBtn->setCursor(Qt::PointingHandCursor);
    playModeBtn->setToolTip("ѭ������");
    playModeBtn->setStyleSheet("QPushButton{border-image:url(:/images/BottomPlay/loop.png);}");

    lyricBtn = new QPushButton(this);
    lyricBtn->setFixedSize(50, 50);
    lyricBtn->setCursor(Qt::PointingHandCursor);
    lyricBtn->setToolTip("�򿪸��\n(��ʱûʵ��С���ڸ�ʣ�ֱ�Ӱ󶨵�������ϸҳ)");
    lyricBtn->setStyleSheet("QPushButton{border-image:url(:/images/BottomPlay/lyric.png);}");

    listBtn = new QPushButton(this);
    listBtn->setFixedSize(50, 50);
    listBtn->setCursor(Qt::PointingHandCursor);
    listBtn->setToolTip("��ǰ�����б�");
    listBtn->setStyleSheet("QPushButton{border-image:url(:/images/BottomPlay/currentList.png);}");

    h_AdditionalFeaturesLayout->addWidget(playModeBtn);
    h_AdditionalFeaturesLayout->addWidget(lyricBtn);
    h_AdditionalFeaturesLayout->addWidget(listBtn);

    // ���������ֲ���
    h_mainLayout->setMargin(10);
    h_mainLayout->setSpacing(10);
}

// ������ͣ/����
void BottomWidget::updatePlayState()
{
    qDebug() << "BottomWidget: ���²���״̬";

    if (m_playing)
    {
        playBtn->setStyleSheet("QPushButton{border-image:url(:/images/BottomPlay/play.png);}");
        playBtn->setToolTip("����");
        m_playing = false;
    }
    else
    {
        playBtn->setStyleSheet("QPushButton{border-image:url(:/images/BottomPlay/pause.png);}");
        playBtn->setToolTip("��ͣ");
        m_playing = true;
    }
    emit playStateChanged(m_playing); // ���䲥��״̬�ı��ź�
}

// ������ͣ/����
void BottomWidget::onPlayStateChanged(bool playing) {
    qDebug() << "BottomWidget: ������ͣ/����" << "=====" << playing;

    if (playing)
    {
        playBtn->setStyleSheet("QPushButton{border-image:url(:/images/BottomPlay/pause.png);}");
        playBtn->setToolTip("��ͣ");
    }
    else
    {
        playBtn->setStyleSheet("QPushButton{border-image:url(:/images/BottomPlay/play.png);}");
        playBtn->setToolTip("����");
    }
}

// ���������Ĳۺ���ʵ��
void BottomWidget::updateVolume(int volume)
{
    qDebug() << "BottomWidget: ��������" << volume;

    if (volume == 0)
    {
        volumeBtn->setStyleSheet("QPushButton{border-image:url(:/images/BottomPlay/mute.png);}");
        volumeBtn->setToolTip("����");
    }
    else if (volume < 50)
    {
        volumeBtn->setStyleSheet("QPushButton{border-image:url(:/images/BottomPlay/low_volume.png);}");
        volumeBtn->setToolTip("С����");
    }
    else
    {
        volumeBtn->setStyleSheet("QPushButton{border-image:url(:/images/BottomPlay/high_volume.png);}");
        volumeBtn->setToolTip("������");
    }
    emit volumeChanged(volume); // ���������ı��ź�
}

// �ı䲥��ģʽ�Ĳۺ���ʵ��
void BottomWidget::changePlayMode()
{
    qDebug() << "BottomWidget: �ı䲥��ģʽ";

    m_playMode = getNextPlayMode(m_playMode);
    switch (m_playMode)
    {
    case PlayMode::Sequential:
        playModeBtn->setStyleSheet("QPushButton{border-image:url(:/images/BottomPlay/sequence.png);}");
        playModeBtn->setToolTip("˳�򲥷�");
        break;
    case PlayMode::Random:
        playModeBtn->setStyleSheet("QPushButton{border-image:url(:/images/BottomPlay/random.png);}");
        playModeBtn->setToolTip("�������");
        break;
    case PlayMode::SingleLoop:
        playModeBtn->setStyleSheet("QPushButton{border-image:url(:/images/BottomPlay/single_loop.png);}");
        playModeBtn->setToolTip("����ѭ��");
        break;
    case PlayMode::Loop:
        playModeBtn->setStyleSheet("QPushButton{border-image:url(:/images/BottomPlay/loop.png);}");
        playModeBtn->setToolTip("ѭ������");
        break;
    }
    emit playModeChanged(m_playMode); // ���䲥��ģʽ�ı��ź�
}

// �������Ĳۺ���ʵ��
void BottomWidget::onSliderClicked(int value)
{
    qDebug() << "BottomWidget: ������" << value << "����";
    emit progressChanged(value); // ����������ı��ź�
}

// ��������ı�Ĳۺ���ʵ��
void BottomWidget::onMusicChanged(const MusicInfoData& music)
{
    qDebug() << "BottomWidget: �����ı�" << music.musicName;

    // ���½��������ֵ
    playTimeSlider->setRange(0, music.duration * 1000); // ��λΪ����
    playTimeSlider->setValue(0);

    // ���¿�ʼ�ͽ���ʱ���ǩ
    startTimeLabel->setText("00:00");
    int minutes = music.duration / 60;
    int seconds = music.duration % 60;
    QString endTimeText = QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
    endTimeLabel->setText(endTimeText);

    emit MusicChanged(music);
}

// ���¿�ʼʱ���ǩ�Ĳۺ���ʵ��
void BottomWidget::updateStartTimeLabel(int value)
{
    qDebug() << "BottomWidget: ���¿�ʼʱ���ǩ" << value;

    int minutes = value / 60000; // ����
    int seconds = (value / 1000) % 60; // ��
    QString startTimeText = QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
    startTimeLabel->setText(startTimeText);
}

// �л������Ĳۺ���ʵ��
void BottomWidget::toggleVolume()
{
    qDebug() << "BottomWidget: �л�����";

    int volume = volumeSlider->value();
    if (volume != 0) {
        volumeSlider->setValue(0);
    }
    else {
        volumeSlider->setValue(50);
    }
    updateVolume(volumeSlider->value());
}

// �������½��ȵĲۺ���ʵ��
void BottomWidget::onProgressChanged(int value)
{
    qDebug() << "BottomWidget: �������½���" << value;

    playTimeSlider->setValue(value);
    updateStartTimeLabel(value);
}
