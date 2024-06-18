#pragma once
#pragma execution_character_set("utf-8")

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextBrowser>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsBlurEffect>
#include <QPainter>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "MusicInfoData.h"
#include "CacheDownloader.h"

class MusicPlayerWidget : public QWidget {
    Q_OBJECT
public:
    MusicPlayerWidget(QWidget* parent = nullptr);

public slots:
    // 更新当前播放的歌曲信息
    void updateCurrentSongInfo(const MusicInfoData& songInfo);

    // 更新歌词文本
    void updateLyrics(const QString& lyrics);

protected:
    void paintEvent(QPaintEvent* event) override;

private slots:
    void onImageDownloaded(const QString& localPath, const QString& id);  // 处理图片下载完成

private:
    QLabel* imageLabel; // 专辑图片标签
    QTextBrowser* lyricsBrowser; // 歌词浏览器
    QLabel* songInfoLabel; // 歌曲信息标签
    CacheDownloader* cacheDownloader;  // 缓存下载器

    QPixmap backgroundPixmap; // 背景图像

    void applyBlurredBackground(const QPixmap& pixmap);  // 应用模糊背景
};