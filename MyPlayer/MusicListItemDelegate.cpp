#include "MusicListItemDelegate.h"
#include "CommentEnum.h"
#include <QFile>
#include <QStandardPaths>
#include <QNetworkRequest>
#include <QDir>
#include <QDebug>

// 构造函数
MusicListItemDelegate::MusicListItemDelegate(QObject* parent)
    : QStyledItemDelegate(parent),
    cacheDownloader(new CacheDownloader(this)),
    pixmapCache(100) // 设置缓存大小
{
    connect(cacheDownloader, &CacheDownloader::downloadFinished, this, [this](const QString& localPath, const QString& id) {
        emit onImageDownloaded(localPath, id, QModelIndex());
        });
}

// 自定义绘制方法
void MusicListItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    qDebug() << "MusicListItemDelegate:绘制===============";
    if (!painter || !index.isValid()) {
        return; // 确保指针有效且索引有效
    }
    if (index.data(MusicInfoDataRole).canConvert<MusicInfoData>()) {
        MusicInfoData itemData = qvariant_cast<MusicInfoData>(index.data(MusicInfoDataRole));

        painter->save();

        QStyleOptionViewItem opt(option);
        initStyleOption(&opt, index);

        // 隔行灰色和选中深灰色
        if (option.state & QStyle::State_Selected) {
            opt.backgroundBrush = QBrush(QColor("#a0a0a0"));
        }
        else if (index.row() % 2 == 0) {
            opt.backgroundBrush = QBrush(QColor("#f0f0f0"));
        }
        else {
            opt.backgroundBrush = QBrush(QColor("white"));
        }

        // 绘制背景
        painter->setRenderHint(QPainter::Antialiasing);
        opt.widget->style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);

        QPixmap pixmap;
        bool isDefault=false;//是否为默认文件，是的话不缓存
        // 优先从缓存加载图片
        if (pixmapCache.contains(itemData.img)) {
            pixmap = *pixmapCache.object(itemData.img);
        }
        else {
            // 检查是否为本地图片
            if (itemData.isLocal) {
                if (!pixmap.load(itemData.img)) {
                    qDebug() << "MusicPlayListItemDelegate: 加载本地图片失败:" << itemData.img;
                    pixmap = QPixmap(":/images/default_album_art.png");
                    isDefault = true;
                }
            }
            else {
                // 检查缓存目录中是否已经存在该图片
                QString cacheFilePath = cacheDownloader->generateCacheFilePath(itemData.img, itemData.id);
                if (QFile::exists(cacheFilePath)) {
                    qDebug() << "MusicPlayListItemDelegate: 缓存文件存在:" << cacheFilePath;
                    if (!pixmap.load(cacheFilePath)) {
                        qDebug() << "MusicPlayListItemDelegate: 加载缓存图片失败:" << cacheFilePath;
                        pixmap = QPixmap(":/images/default_album_art.png");
                        isDefault = true;
                    }
                }
                else {
                    qDebug() << "MusicPlayListItemDelegate: 缓存文件不存在，开始下载:" << itemData.img;
                    // 下载远程图片
                    cacheDownloader->download(itemData.img, itemData.id);
                    // 先绘制无图版本
                    pixmap = QPixmap(":/images/default_album_art.png");
                    isDefault = true;
                }
            }

            if ((itemData.isLocal || !isDefault) && !pixmap.isNull()) {
                pixmapCache.insert(itemData.img, new QPixmap(pixmap));
            }
        }

        // 绘制图标
        QRect iconRect = option.rect.adjusted(5, 5, -option.rect.width() + 45, -5);
        painter->drawPixmap(iconRect, pixmap);

        // 绘制音乐名称
        QRect musicNameRect = option.rect.adjusted(50, 5, -5, -option.rect.height() / 2);
        painter->drawText(musicNameRect, Qt::AlignLeft | Qt::AlignVCenter, itemData.musicName);

        // 绘制专辑名称
        QRect albumNameRect = option.rect.adjusted(50, option.rect.height() / 2, -5, -5);
        painter->drawText(albumNameRect, Qt::AlignLeft | Qt::AlignVCenter, itemData.albumName);

        // 转换时间为 mm:ss 格式
        int minutes = itemData.duration / 60;
        int seconds = itemData.duration % 60;
        QString durationText = QString("%1:%2")
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'));

        // 绘制时长
        QRect durationRect = option.rect.adjusted(option.rect.width() - 55, 5, -5, -5);
        painter->drawText(durationRect, Qt::AlignRight | Qt::AlignVCenter, durationText);

        painter->restore();
    }
    else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

// 自定义尺寸提示方法
QSize MusicListItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return QSize(option.rect.width(), 60); // 设置高度为60像素
}

// 图片下载完成后的槽函数
void MusicListItemDelegate::onImageDownloaded(const QString& localPath, const QString& id, const QModelIndex& index) const
{
    qDebug() << "MusicListItemDelegate: 下载图片完成 - 路径:" << localPath << "ID:" << id;

    // 将下载的图片加入缓存
    QPixmap pixmap(localPath);
    if (!pixmap.isNull()) {
        pixmapCache.insert(localPath, new QPixmap(pixmap));
    }

    // 重新获取索引并更新视图
    if (index.isValid()) {
        QAbstractItemModel* model = const_cast<QAbstractItemModel*>(index.model());
        if (model) {
            model->dataChanged(index, index, { Qt::DecorationRole });
            qDebug() << "MusicListItemDelegate: 更新图片完成 - 索引:" << index;
        }
        else {
            qDebug() << "MusicListItemDelegate: 获取模型失败";
        }
    }
    else {
        qDebug() << "MusicListItemDelegate: 获取索引无效";
    }
}
