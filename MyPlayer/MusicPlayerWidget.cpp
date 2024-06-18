#include "MusicPlayerWidget.h"
#include <QGraphicsBlurEffect>
#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

// ���캯��
MusicPlayerWidget::MusicPlayerWidget(QWidget* parent)
    : QWidget(parent),
    imageLabel(new QLabel(this)),
    lyricsBrowser(new QTextBrowser(this)),
    songInfoLabel(new QLabel(this)),
    cacheDownloader(new CacheDownloader(this))
{
    // ���ô���͸��
    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("background: transparent;");

    // ������ˮƽ����
    QHBoxLayout* mainLayout = new QHBoxLayout(this);

    // �����൯�Լ��
    mainLayout->addStretch(1);

    // ר��ͼƬ
    QPixmap songImage(200, 200); // ����һ���հ�ͼƬ
    songImage.fill(Qt::gray); // ��ͼƬ���Ϊ��ɫ
    imageLabel->setPixmap(songImage);
    imageLabel->setFixedSize(200, 200); // ����ͼƬ��С
    imageLabel->setStyleSheet("background: transparent;");

    // �����ʾ
    lyricsBrowser->setText("��ʼ�����...");
    lyricsBrowser->setFixedSize(300, 200); // ���ø����ʾ�Ĺ̶���С
    lyricsBrowser->setStyleSheet("background: transparent; border: none; font-size: 18px; color: white; text-align: center;");

    // ������Ϣ��ǩ
    songInfoLabel->setStyleSheet("background: transparent; color: white; font-size: 16px;");
    songInfoLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    // ����һ������ͼƬ�Ĵ�ֱ����
    QVBoxLayout* imageLayout = new QVBoxLayout();
    imageLayout->addStretch(1);
    imageLayout->addWidget(imageLabel, 2);
    imageLayout->addStretch(1);

    // ����һ��������ʵĴ�ֱ����
    QVBoxLayout* lyricsLayout = new QVBoxLayout();
    lyricsLayout->addStretch(1);
    lyricsLayout->addWidget(songInfoLabel);
    lyricsLayout->addWidget(lyricsBrowser, 2);
    lyricsLayout->addStretch(1);

    // ����ֱ������ӵ���ˮƽ����
    mainLayout->addLayout(imageLayout, 2);
    mainLayout->addStretch(2); // ��Ӽ��
    mainLayout->addLayout(lyricsLayout, 2);

    // ����Ҳ൯�Լ��
    mainLayout->addStretch(1);

    // ���û���������ɵ��ź����
    connect(cacheDownloader, &CacheDownloader::downloadFinished, this, &MusicPlayerWidget::onImageDownloaded);
}

// ���µ�ǰ���ŵĸ�����Ϣ
void MusicPlayerWidget::updateCurrentSongInfo(const MusicInfoData& songInfo)
{
    qDebug() << "���µ�ǰ���ŵĸ�����Ϣ: " << songInfo.musicName;

    // ���ø�������ר����
    QString songInfoText = QString("<h2>%1</h2><h3>%2</h3>").arg(songInfo.musicName, songInfo.albumName);
    songInfoLabel->setText(songInfoText);
    songInfoLabel->update();// ǿ���ػ�
    // ����ר��ͼƬ
    if (songInfo.isLocal) {
        QPixmap albumImage;
        if (!albumImage.load(songInfo.img)) {
            qDebug() << "���ر���ͼƬʧ��: " << songInfo.img;
            albumImage.load(":/images/default_album_art.png"); // ʹ��Ĭ��ͼƬ
        }
        imageLabel->setPixmap(albumImage.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        imageLabel->update();// ǿ���ػ�
        applyBlurredBackground(albumImage);
    }
    else {
        cacheDownloader->download(songInfo.img, songInfo.id);
    }
}

// ����ͼƬ������ɵĲۺ���
void MusicPlayerWidget::onImageDownloaded(const QString& localPath, const QString& id)
{
    qDebug() << "ͼƬ�������: " << localPath;

    QPixmap albumImage;
    if (!albumImage.load(localPath)) {
        qDebug() << "�������ص�ͼƬʧ��: " << localPath;
        albumImage.load(":/images/default_album_art.png"); // ʹ��Ĭ��ͼƬ
    }
    imageLabel->setPixmap(albumImage.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    imageLabel->update();// ǿ���ػ�
    applyBlurredBackground(albumImage);
}

void MusicPlayerWidget::applyBlurredBackground(const QPixmap& pixmap)
{
    QImage img = pixmap.toImage();
    QGraphicsBlurEffect* blurEffect = new QGraphicsBlurEffect;
    blurEffect->setBlurRadius(250);

    QGraphicsScene scene;
    QGraphicsPixmapItem item;
    item.setPixmap(QPixmap::fromImage(img));
    item.setGraphicsEffect(blurEffect);
    scene.addItem(&item);

    QImage result(img.size(), QImage::Format_ARGB32);
    result.fill(Qt::transparent);

    QPainter painter(&result);
    scene.render(&painter);
    painter.end();

    backgroundPixmap = QPixmap::fromImage(result);
    update();
}

void MusicPlayerWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    if (!backgroundPixmap.isNull()) {
        painter.drawPixmap(rect(), backgroundPixmap);
    }
    QWidget::paintEvent(event);
}

// ���¸���ı�
void MusicPlayerWidget::updateLyrics(const QString& lyrics)
{
    qDebug() << "���¸���ı�";
    lyricsBrowser->setHtml(lyrics);
    lyricsBrowser->update();  // ǿ���ػ�
}
