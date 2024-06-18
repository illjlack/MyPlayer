#include "PlaySongInfoWidget.h"
#include <QMouseEvent>
#include <QEvent>
#include <QtCore/QStandardPaths>
#include <QtCore/QFileInfo>
#include "MusicInfoManager.h"

// ���캯��
PlaySongInfoWidget::PlaySongInfoWidget(QWidget* parent)
    : QFrame(parent), isLoved(false), isExpanded(false), cacheDownloader(new CacheDownloader(this))
{
    setFixedHeight(80);  // ���ù̶��߶�
    setFixedWidth(350);  // ���ù̶����

    // ������ʽ������ɫ�ͱ߿�
    setStyleSheet("QFrame{background:rgb(246,246,248);border-color:rgb(225,225,225);border-width: 0px 1px 0px 1px;border-style: solid;}");

    initWidget();  // ��ʼ������

    // ���� loveBtn �ĵ���źŵ��ۺ���
    connect(loveBtn, &QPushButton::clicked, this, &PlaySongInfoWidget::onLoveBtnClicked);
    // ���� cdLabel �ĵ���źŵ��ۺ���
    connect(cdLabel, &ClickLabel::clicked, this, &PlaySongInfoWidget::isToggleSongDetails);

    // ���ӻ�������������������źŵ��ۺ���
    connect(cacheDownloader, &CacheDownloader::downloadFinished, this, &PlaySongInfoWidget::onImageDownloaded);
}

// ��������
PlaySongInfoWidget::~PlaySongInfoWidget()
{
}

// ���ø����͸����ı�
void PlaySongInfoWidget::setSongText(const QString& song, const QString& single)
{
    QString show_song = "";
    QString show_single = "";

    // ������������ȴ���10���ضϲ����ʡ�Ժ�
    if (song.length() > 10) {
        show_song = song.mid(0, 9);
        show_song.append("...");
        songNameBtn->setText(show_song);
        songNameBtn->setToolTip(song);  // ������ʾΪ�����ĸ�����
    }
    else {
        songNameBtn->setText(song);
        songNameBtn->setToolTip(song);
    }

    // ������������ȴ���10���ضϲ����ʡ�Ժ�
    if (single.length() > 10) {
        show_single = single.mid(0, 9);
        show_single.append("...");
        singleNameBtn->setText(show_single);
        singleNameBtn->setToolTip(single);  // ������ʾΪ�����ĸ�����
    }
    else {
        singleNameBtn->setText(single);
        singleNameBtn->setToolTip(single);
    }
}

// ��ʼ������
void PlaySongInfoWidget::initWidget()
{
    h_mainLayout = new QHBoxLayout(this);  // ������ˮƽ����
    v_initLayout1 = new QVBoxLayout;  // ������ֱ����1
    v_initLayout2 = new QVBoxLayout;  // ������ֱ����2

    cdLabel = new ClickLabel(this);  // ���� CD ��ǩ
    cdLabel->setFixedSize(60, 60);  // ���ù̶���С
    cdLabel->setToolTip("չ��������ϸҳ");
    cdLabel->setCursor(Qt::PointingHandCursor);  // ���ù��Ϊ����
    cdLabel->setStyleSheet("QLabel{border-image:url(:/images/SongInfo/disk.png)}");  // ������ʽ

    songNameBtn = new QPushButton(this);  // �����������ư�ť
    songNameBtn->setFixedSize(150, 20);  // ���ù̶���С
    songNameBtn->setCursor(Qt::PointingHandCursor);  // ���ù��Ϊ����
    songNameBtn->setText("δ֪");  // ���ó�ʼ�ı�
    songNameBtn->adjustSize();  // ������С
    songNameBtn->setStyleSheet("QPushButton{background:transparent;border:none;font:13px}");  // ������ʽ

    singleNameBtn = new QPushButton(this);  // �����������ư�ť
    singleNameBtn->setFixedSize(150, 20);  // ���ù̶���С
    singleNameBtn->setCursor(Qt::PointingHandCursor);  // ���ù��Ϊ����
    singleNameBtn->setText("δ֪");  // ���ó�ʼ�ı�
    singleNameBtn->adjustSize();  // ������С
    singleNameBtn->setStyleSheet("QPushButton{background:transparent;border:none;font:13px}");  // ������ʽ

    // ����ť��ӵ���ֱ����1��
    v_initLayout1->addWidget(songNameBtn);
    v_initLayout1->addWidget(singleNameBtn);
    v_initLayout1->setMargin(0);  // ���ñ߾�Ϊ0

    loveBtn = new QPushButton(this);  // �����ղذ�ť
    loveBtn->setCursor(Qt::PointingHandCursor);  // ���ù��Ϊ����
    loveBtn->setFixedSize(25, 25);  // ���ù̶���С
    loveBtn->setStyleSheet("QPushButton{border-image:url(:/images/SongInfo/love.png)}");  // ������ʽ

    // ����ť��ӵ���ֱ����2��
    v_initLayout2->addWidget(loveBtn);

    // �����в�����ӵ���ˮƽ������
    h_mainLayout->addWidget(cdLabel);
    h_mainLayout->addLayout(v_initLayout1);
    h_mainLayout->addLayout(v_initLayout2);
    h_mainLayout->setMargin(0);  // ������ˮƽ���ֵı߾�Ϊ0
}

// ���¸�����Ϣ�ۺ���
void PlaySongInfoWidget::updateSongInfo(const MusicInfoData& songInfo)
{
    currentMusic = songInfo;
    setSongText(songInfo.musicName, songInfo.albumName);

    // �����Ƿ�ϲ������ loveBtn ��ͼ��
    isLoved = MusicInfoManager::instance().getMusicById(songInfo.id).isLove; // �����ݿ�һ��

    if (isLoved) {
        loveBtn->setStyleSheet("QPushButton{border-image:url(:/images/SongInfo/love1.png)}");
    }
    else {
        loveBtn->setStyleSheet("QPushButton{border-image:url(:/images/SongInfo/love.png)}");
    }

    // �ж�ͼƬ·���Ƿ�Ϊ����·��������Ǳ���·����ֱ��ʹ�ã���������ͼƬ
    if (songInfo.isLocal) {
        qDebug() << "PlaySongInfoWidget:ͼƬ·���ڱ���";
        if (QFileInfo::exists(songInfo.img)) {
            qDebug() << "PlaySongInfoWidget:ʹ��ͼƬ=" << songInfo.albumName;
            cdLabel->setPixmap(QPixmap(songInfo.img).scaled(cdLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
        else {
            qDebug() << "PlaySongInfoWidget:����ͼƬ�����ڣ�ʹ��Ĭ��ͼƬ:" << songInfo.id;
            cdLabel->setPixmap(QPixmap(":/images/default_album_art.png").scaled(cdLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }
    else {
        downloadImage(songInfo.img, songInfo.id);
    }
    loveBtn->repaint();  // ǿ���ػ�
    cdLabel->repaint();  // ǿ���ػ�
}

// ����ͼƬ
void PlaySongInfoWidget::downloadImage(const QString& imageUrl, const QString& id)
{
    cacheDownloader->download(imageUrl, id);
}

// ����ͼƬ������ɵĲۺ���
void PlaySongInfoWidget::onImageDownloaded(const QString& filePath, const QString& id)
{
    Q_UNUSED(id);  // ��� id �����������û��ʹ�ã����Ժ�����

    QPixmap pixmap(filePath);
    if (!pixmap.isNull()) {
        cdLabel->setPixmap(pixmap.scaled(cdLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    else {
        qDebug() << "���ػ����ͼƬʧ�ܣ�ʹ��Ĭ��ͼƬ";
        cdLabel->setPixmap(QPixmap(":/images/default_album_art.png").scaled(cdLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    cdLabel->repaint();  // ǿ�������ػ�
}

// �ղذ�ť����ۺ���
void PlaySongInfoWidget::onLoveBtnClicked()
{
    // �л� isLoved ״̬
    isLoved = !isLoved;

    // ���� isLoved ״̬�л���ťͼ��
    if (isLoved) {
        loveBtn->setStyleSheet("QPushButton{border-image:url(:/images/SongInfo/love1.png)}");
        emit loved(currentMusic);
    }
    else {
        loveBtn->setStyleSheet("QPushButton{border-image:url(:/images/SongInfo/love.png)}");
        emit nolove(currentMusic);
    }
    loveBtn->repaint();  // ǿ���ػ�
}

// �л�������ϸҳ
void PlaySongInfoWidget::isToggleSongDetails()
{
    isExpanded = !isExpanded;

    if (isExpanded) {
        cdLabel->setToolTip("���������ϸҳ");
    }
    else {
        cdLabel->setToolTip("չ��������ϸҳ");
    }

    emit toggleSongDetails(isExpanded);

    cdLabel->repaint();  // ǿ���ػ�
}
