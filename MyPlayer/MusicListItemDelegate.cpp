#include "MusicListItemDelegate.h"
#include "CommentEnum.h"
#include <QFile>
#include <QStandardPaths>
#include <QNetworkRequest>
#include <QDir>
#include <QDebug>

// ���캯��
MusicListItemDelegate::MusicListItemDelegate(QObject* parent)
    : QStyledItemDelegate(parent),
    cacheDownloader(new CacheDownloader(this)),
    pixmapCache(100) // ���û����С
{
    connect(cacheDownloader, &CacheDownloader::downloadFinished, this, [this](const QString& localPath, const QString& id) {
        emit onImageDownloaded(localPath, id, QModelIndex());
        });
}

// �Զ�����Ʒ���
void MusicListItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    qDebug() << "MusicListItemDelegate:����===============";
    if (!painter || !index.isValid()) {
        return; // ȷ��ָ����Ч��������Ч
    }
    if (index.data(MusicInfoDataRole).canConvert<MusicInfoData>()) {
        MusicInfoData itemData = qvariant_cast<MusicInfoData>(index.data(MusicInfoDataRole));

        painter->save();

        QStyleOptionViewItem opt(option);
        initStyleOption(&opt, index);

        // ���л�ɫ��ѡ�����ɫ
        if (option.state & QStyle::State_Selected) {
            opt.backgroundBrush = QBrush(QColor("#a0a0a0"));
        }
        else if (index.row() % 2 == 0) {
            opt.backgroundBrush = QBrush(QColor("#f0f0f0"));
        }
        else {
            opt.backgroundBrush = QBrush(QColor("white"));
        }

        // ���Ʊ���
        painter->setRenderHint(QPainter::Antialiasing);
        opt.widget->style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);

        QPixmap pixmap;
        bool isDefault=false;//�Ƿ�ΪĬ���ļ����ǵĻ�������
        // ���ȴӻ������ͼƬ
        if (pixmapCache.contains(itemData.img)) {
            pixmap = *pixmapCache.object(itemData.img);
        }
        else {
            // ����Ƿ�Ϊ����ͼƬ
            if (itemData.isLocal) {
                if (!pixmap.load(itemData.img)) {
                    qDebug() << "MusicPlayListItemDelegate: ���ر���ͼƬʧ��:" << itemData.img;
                    pixmap = QPixmap(":/images/default_album_art.png");
                    isDefault = true;
                }
            }
            else {
                // ��黺��Ŀ¼���Ƿ��Ѿ����ڸ�ͼƬ
                QString cacheFilePath = cacheDownloader->generateCacheFilePath(itemData.img, itemData.id);
                if (QFile::exists(cacheFilePath)) {
                    qDebug() << "MusicPlayListItemDelegate: �����ļ�����:" << cacheFilePath;
                    if (!pixmap.load(cacheFilePath)) {
                        qDebug() << "MusicPlayListItemDelegate: ���ػ���ͼƬʧ��:" << cacheFilePath;
                        pixmap = QPixmap(":/images/default_album_art.png");
                        isDefault = true;
                    }
                }
                else {
                    qDebug() << "MusicPlayListItemDelegate: �����ļ������ڣ���ʼ����:" << itemData.img;
                    // ����Զ��ͼƬ
                    cacheDownloader->download(itemData.img, itemData.id);
                    // �Ȼ�����ͼ�汾
                    pixmap = QPixmap(":/images/default_album_art.png");
                    isDefault = true;
                }
            }

            if ((itemData.isLocal || !isDefault) && !pixmap.isNull()) {
                pixmapCache.insert(itemData.img, new QPixmap(pixmap));
            }
        }

        // ����ͼ��
        QRect iconRect = option.rect.adjusted(5, 5, -option.rect.width() + 45, -5);
        painter->drawPixmap(iconRect, pixmap);

        // ������������
        QRect musicNameRect = option.rect.adjusted(50, 5, -5, -option.rect.height() / 2);
        painter->drawText(musicNameRect, Qt::AlignLeft | Qt::AlignVCenter, itemData.musicName);

        // ����ר������
        QRect albumNameRect = option.rect.adjusted(50, option.rect.height() / 2, -5, -5);
        painter->drawText(albumNameRect, Qt::AlignLeft | Qt::AlignVCenter, itemData.albumName);

        // ת��ʱ��Ϊ mm:ss ��ʽ
        int minutes = itemData.duration / 60;
        int seconds = itemData.duration % 60;
        QString durationText = QString("%1:%2")
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'));

        // ����ʱ��
        QRect durationRect = option.rect.adjusted(option.rect.width() - 55, 5, -5, -5);
        painter->drawText(durationRect, Qt::AlignRight | Qt::AlignVCenter, durationText);

        painter->restore();
    }
    else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

// �Զ���ߴ���ʾ����
QSize MusicListItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return QSize(option.rect.width(), 60); // ���ø߶�Ϊ60����
}

// ͼƬ������ɺ�Ĳۺ���
void MusicListItemDelegate::onImageDownloaded(const QString& localPath, const QString& id, const QModelIndex& index) const
{
    qDebug() << "MusicListItemDelegate: ����ͼƬ��� - ·��:" << localPath << "ID:" << id;

    // �����ص�ͼƬ���뻺��
    QPixmap pixmap(localPath);
    if (!pixmap.isNull()) {
        pixmapCache.insert(localPath, new QPixmap(pixmap));
    }

    // ���»�ȡ������������ͼ
    if (index.isValid()) {
        QAbstractItemModel* model = const_cast<QAbstractItemModel*>(index.model());
        if (model) {
            model->dataChanged(index, index, { Qt::DecorationRole });
            qDebug() << "MusicListItemDelegate: ����ͼƬ��� - ����:" << index;
        }
        else {
            qDebug() << "MusicListItemDelegate: ��ȡģ��ʧ��";
        }
    }
    else {
        qDebug() << "MusicListItemDelegate: ��ȡ������Ч";
    }
}
