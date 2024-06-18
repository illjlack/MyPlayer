#include "MusicPlayerWidget.h"
#include <QGraphicsBlurEffect>
#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

// 构造函数
MusicPlayerWidget::MusicPlayerWidget(QWidget* parent)
    : QWidget(parent),
    imageLabel(new QLabel(this)),
    lyricsBrowser(new QTextBrowser(this)),
    songInfoLabel(new QLabel(this)),
    cacheDownloader(new CacheDownloader(this))
{
    // 设置窗口透明
    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("background: transparent;");

    // 创建主水平布局
    QHBoxLayout* mainLayout = new QHBoxLayout(this);

    // 添加左侧弹性间隔
    mainLayout->addStretch(1);

    // 专辑图片
    QPixmap songImage(200, 200); // 创建一个空白图片
    songImage.fill(Qt::gray); // 将图片填充为灰色
    imageLabel->setPixmap(songImage);
    imageLabel->setFixedSize(200, 200); // 设置图片大小
    imageLabel->setStyleSheet("background: transparent;");

    // 歌词显示
    lyricsBrowser->setText("歌词加载中...");
    lyricsBrowser->setFixedSize(300, 200); // 设置歌词显示的固定大小
    lyricsBrowser->setStyleSheet("background: transparent; border: none; font-size: 18px; color: white; text-align: center;");

    // 歌曲信息标签
    songInfoLabel->setStyleSheet("background: transparent; color: white; font-size: 16px;");
    songInfoLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    // 创建一个包含图片的垂直布局
    QVBoxLayout* imageLayout = new QVBoxLayout();
    imageLayout->addStretch(1);
    imageLayout->addWidget(imageLabel, 2);
    imageLayout->addStretch(1);

    // 创建一个包含歌词的垂直布局
    QVBoxLayout* lyricsLayout = new QVBoxLayout();
    lyricsLayout->addStretch(1);
    lyricsLayout->addWidget(songInfoLabel);
    lyricsLayout->addWidget(lyricsBrowser, 2);
    lyricsLayout->addStretch(1);

    // 将垂直布局添加到主水平布局
    mainLayout->addLayout(imageLayout, 2);
    mainLayout->addStretch(2); // 添加间隔
    mainLayout->addLayout(lyricsLayout, 2);

    // 添加右侧弹性间隔
    mainLayout->addStretch(1);

    // 设置缓存下载完成的信号与槽
    connect(cacheDownloader, &CacheDownloader::downloadFinished, this, &MusicPlayerWidget::onImageDownloaded);
}

// 更新当前播放的歌曲信息
void MusicPlayerWidget::updateCurrentSongInfo(const MusicInfoData& songInfo)
{
    qDebug() << "更新当前播放的歌曲信息: " << songInfo.musicName;

    // 设置歌曲名和专辑名
    QString songInfoText = QString("<h2>%1</h2><h3>%2</h3>").arg(songInfo.musicName, songInfo.albumName);
    songInfoLabel->setText(songInfoText);
    songInfoLabel->update();// 强制重绘
    // 设置专辑图片
    if (songInfo.isLocal) {
        QPixmap albumImage;
        if (!albumImage.load(songInfo.img)) {
            qDebug() << "加载本地图片失败: " << songInfo.img;
            albumImage.load(":/images/default_album_art.png"); // 使用默认图片
        }
        imageLabel->setPixmap(albumImage.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        imageLabel->update();// 强制重绘
        applyBlurredBackground(albumImage);
    }
    else {
        cacheDownloader->download(songInfo.img, songInfo.id);
    }
}

// 处理图片下载完成的槽函数
void MusicPlayerWidget::onImageDownloaded(const QString& localPath, const QString& id)
{
    qDebug() << "图片下载完成: " << localPath;

    QPixmap albumImage;
    if (!albumImage.load(localPath)) {
        qDebug() << "加载下载的图片失败: " << localPath;
        albumImage.load(":/images/default_album_art.png"); // 使用默认图片
    }
    imageLabel->setPixmap(albumImage.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    imageLabel->update();// 强制重绘
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

// 更新歌词文本
void MusicPlayerWidget::updateLyrics(const QString& lyrics)
{
    qDebug() << "更新歌词文本";
    lyricsBrowser->setHtml(lyrics);
    lyricsBrowser->update();  // 强制重绘
}
