#pragma once

#include <QStyledItemDelegate>
#include <QPainter>
#include <QCache>
#include "MusicInfoData.h"
#include "CacheDownloader.h"

class MusicListItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit MusicListItemDelegate(QObject* parent = nullptr);
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private slots:
    void onImageDownloaded(const QString& localPath, const QString& id, const QModelIndex& index) const;

private:
    mutable CacheDownloader* cacheDownloader; // ����������
    mutable QCache<QString, QPixmap> pixmapCache; // ͼƬ����
};
