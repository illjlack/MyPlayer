#include "MiddleWidgetRightBase.h"
#include <QVBoxLayout>
#include <QDebug>
#include <QAction>
#include "CommentEnum.h"

MiddleWidgetRightBase::MiddleWidgetRightBase(QWidget* parent)
    : QWidget(parent),
    listView(new QListView(this)),
    model(new QStandardItemModel(this)),
    contextMenu(new QMenu(this))
{
    QVBoxLayout* layout = new QVBoxLayout(this);  // ���ò���
    layout->addWidget(listView);
    setLayout(layout);

    listView->setModel(model);  // ����ģ��
    listView->setItemDelegate(new MusicListItemDelegate(this));  // �����Զ���ί��
    listView->setEditTriggers(QAbstractItemView::NoEditTriggers);  // ����ֻ��ģʽ

    connect(listView, &QListView::doubleClicked, this, &MiddleWidgetRightBase::handleItemDoubleClicked);  // ����˫���ź�
    connect(listView, &QListView::customContextMenuRequested, this, &MiddleWidgetRightBase::showContextMenu);  // �����Ҽ��˵��ź�

    listView->setContextMenuPolicy(Qt::CustomContextMenu);  // �����Զ��������Ĳ˵�

    // ���������Ĳ˵�����
    QAction* likeAction = new QAction("ϲ��", this);
    QAction* addAction = new QAction("��ӵ��б�", this);
    QAction* playAction = new QAction("����", this);

    // ��������ӵ������Ĳ˵�
    contextMenu->addAction(likeAction);
    contextMenu->addAction(addAction);
    contextMenu->addAction(playAction);

    // ���Ӷ������ۺ���
    connect(likeAction, &QAction::triggered, this, &MiddleWidgetRightBase::handleLike);
    connect(addAction, &QAction::triggered, this, &MiddleWidgetRightBase::handleAdd);
    connect(playAction, &QAction::triggered, this, &MiddleWidgetRightBase::handlePlay);
}

void MiddleWidgetRightBase::updateMusicList(const QList<MusicInfoData>& musicList)
{
    model->clear();  // ���������
    for (const MusicInfoData& music : musicList) {
        QStandardItem* item = new QStandardItem;
        item->setData(QVariant::fromValue(music), MusicInfoDataRole);
        model->appendRow(item);
    }
}

void MiddleWidgetRightBase::handleItemDoubleClicked(const QModelIndex& index)
{
    if (index.data(MusicInfoDataRole).canConvert<MusicInfoData>()) {
        MusicInfoData itemData = qvariant_cast<MusicInfoData>(index.data(MusicInfoDataRole));
        qDebug() << "˫�����ѡ������" << itemData.id;
        emit musicSelected(itemData);  // ����ѡ�������ź�
    }
}

void MiddleWidgetRightBase::showContextMenu(const QPoint& pos)
{
    // ��ȡ����������
    currentIndex = listView->indexAt(pos);
    if (currentIndex.isValid()) {
        // ��ʾ�����Ĳ˵�
        contextMenu->exec(listView->viewport()->mapToGlobal(pos));
    }
}

void MiddleWidgetRightBase::handleLike()
{
    if (currentIndex.isValid()) {
        MusicInfoData itemData = qvariant_cast<MusicInfoData>(currentIndex.data(MusicInfoDataRole));
        qDebug() << "Like action triggered for item:" << itemData.id;
        emit musicLiked(itemData);  // ����ϲ�������ź�
    }
}

void MiddleWidgetRightBase::handleAdd()
{
    if (currentIndex.isValid()) {
        MusicInfoData itemData = qvariant_cast<MusicInfoData>(currentIndex.data(MusicInfoDataRole));
        qDebug() << "Add action triggered for item:" << itemData.id;
        emit musicAddedToPlaylist(itemData);  // ����������ֵ��б��ź�
    }
}

void MiddleWidgetRightBase::handlePlay()
{
    if (currentIndex.isValid()) {
        MusicInfoData itemData = qvariant_cast<MusicInfoData>(currentIndex.data(MusicInfoDataRole));
        qDebug() << "Play action triggered for item:" << itemData.id;
        emit musicPlayRequested(itemData);  // �������������ź�
    }
}
