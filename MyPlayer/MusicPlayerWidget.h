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
    // ���µ�ǰ���ŵĸ�����Ϣ
    void updateCurrentSongInfo(const MusicInfoData& songInfo);

    // ���¸���ı�
    void updateLyrics(const QString& lyrics);

protected:
    void paintEvent(QPaintEvent* event) override;

private slots:
    void onImageDownloaded(const QString& localPath, const QString& id);  // ����ͼƬ�������

private:
    QLabel* imageLabel; // ר��ͼƬ��ǩ
    QTextBrowser* lyricsBrowser; // ��������
    QLabel* songInfoLabel; // ������Ϣ��ǩ
    CacheDownloader* cacheDownloader;  // ����������

    QPixmap backgroundPixmap; // ����ͼ��

    void applyBlurredBackground(const QPixmap& pixmap);  // Ӧ��ģ������
};