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
    QVBoxLayout* layout = new QVBoxLayout(this);  // 设置布局
    layout->addWidget(listView);
    setLayout(layout);

    listView->setModel(model);  // 设置模型
    listView->setItemDelegate(new MusicListItemDelegate(this));  // 设置自定义委托
    listView->setEditTriggers(QAbstractItemView::NoEditTriggers);  // 设置只读模式

    connect(listView, &QListView::doubleClicked, this, &MiddleWidgetRightBase::handleItemDoubleClicked);  // 连接双击信号
    connect(listView, &QListView::customContextMenuRequested, this, &MiddleWidgetRightBase::showContextMenu);  // 连接右键菜单信号

    listView->setContextMenuPolicy(Qt::CustomContextMenu);  // 启用自定义上下文菜单

    // 创建上下文菜单动作
    QAction* likeAction = new QAction("喜爱", this);
    QAction* addAction = new QAction("添加到列表", this);
    QAction* playAction = new QAction("播放", this);

    // 将动作添加到上下文菜单
    contextMenu->addAction(likeAction);
    contextMenu->addAction(addAction);
    contextMenu->addAction(playAction);

    // 连接动作到槽函数
    connect(likeAction, &QAction::triggered, this, &MiddleWidgetRightBase::handleLike);
    connect(addAction, &QAction::triggered, this, &MiddleWidgetRightBase::handleAdd);
    connect(playAction, &QAction::triggered, this, &MiddleWidgetRightBase::handlePlay);
}

void MiddleWidgetRightBase::updateMusicList(const QList<MusicInfoData>& musicList)
{
    model->clear();  // 清除现有项
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
        qDebug() << "双击点击选中音乐" << itemData.id;
        emit musicSelected(itemData);  // 发出选中音乐信号
    }
}

void MiddleWidgetRightBase::showContextMenu(const QPoint& pos)
{
    // 获取点击项的索引
    currentIndex = listView->indexAt(pos);
    if (currentIndex.isValid()) {
        // 显示上下文菜单
        contextMenu->exec(listView->viewport()->mapToGlobal(pos));
    }
}

void MiddleWidgetRightBase::handleLike()
{
    if (currentIndex.isValid()) {
        MusicInfoData itemData = qvariant_cast<MusicInfoData>(currentIndex.data(MusicInfoDataRole));
        qDebug() << "Like action triggered for item:" << itemData.id;
        emit musicLiked(itemData);  // 发出喜爱音乐信号
    }
}

void MiddleWidgetRightBase::handleAdd()
{
    if (currentIndex.isValid()) {
        MusicInfoData itemData = qvariant_cast<MusicInfoData>(currentIndex.data(MusicInfoDataRole));
        qDebug() << "Add action triggered for item:" << itemData.id;
        emit musicAddedToPlaylist(itemData);  // 发出添加音乐到列表信号
    }
}

void MiddleWidgetRightBase::handlePlay()
{
    if (currentIndex.isValid()) {
        MusicInfoData itemData = qvariant_cast<MusicInfoData>(currentIndex.data(MusicInfoDataRole));
        qDebug() << "Play action triggered for item:" << itemData.id;
        emit musicPlayRequested(itemData);  // 发出播放音乐信号
    }
}
