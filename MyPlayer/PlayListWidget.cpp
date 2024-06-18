#include "PlayListWidget.h"
#include <QVBoxLayout>
#include <QAction>
#include <QDebug>
#include "CommentEnum.h"

PlayListWidget::PlayListWidget(QWidget* parent)
    : QWidget(parent),
    listView(new QListView(this)),
    model(new QStandardItemModel(this)),
    contextMenu(new QMenu(this))
{
    qDebug() << "PlayListWidget: 构造函数开始执行";

    // 设置布局
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(listView);
    setLayout(layout);

    // 设置只读模式
    listView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 设置自定义委托和模型
    MusicPlayListItemDelegate* itemDelegate = new MusicPlayListItemDelegate(this);
    listView->setItemDelegate(itemDelegate);
    listView->setModel(model);

    // 设置右键菜单
    listView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(listView, &QListView::customContextMenuRequested, this, &PlayListWidget::showContextMenu);

    // 连接信号和槽
    connect(listView, &QListView::clicked, this, &PlayListWidget::handleItemClicked);

    // 创建上下文菜单动作
    QAction* playAction = new QAction("播放", this);
    QAction* likeAction = new QAction("喜爱", this);

    // 将动作添加到上下文菜单
    contextMenu->addAction(playAction);
    contextMenu->addAction(likeAction);

    // 连接动作到槽函数
    connect(likeAction, &QAction::triggered, this, &PlayListWidget::handleLike);
    connect(playAction, &QAction::triggered, this, &PlayListWidget::handlePlay);

    qDebug() << "PlayListWidget: 构造函数执行完毕";
}

void PlayListWidget::handleItemClicked(const QModelIndex& index)
{
    qDebug() << "PlayListWidget: handleItemClicked 函数被调用";
    if (index.data(MusicInfoDataRole).canConvert<MusicInfoData>()) {
        MusicInfoData itemData = qvariant_cast<MusicInfoData>(index.data(MusicInfoDataRole));
        emit itemClicked(itemData);
        qDebug() << "PlayListWidget: itemClicked 信号发出, 数据为:" << itemData.musicName;
    }
    else {
        qDebug() << "PlayListWidget: 无效的数据角色";
    }
}

void PlayListWidget::showContextMenu(const QPoint& pos)
{
    qDebug() << "PlayListWidget: showContextMenu 函数被调用";
    // 获取点击项的索引
    currentIndex = listView->indexAt(pos);
    if (currentIndex.isValid()) {
        qDebug() << "PlayListWidget: 有效的项被点击，显示上下文菜单";
        // 显示上下文菜单
        contextMenu->exec(listView->viewport()->mapToGlobal(pos));
    }
    else {
        qDebug() << "PlayListWidget: 无效的项被点击";
    }
}

void PlayListWidget::handleLike()
{
    qDebug() << "PlayListWidget: handleLike 函数被调用";
    if (currentIndex.isValid()) {
        MusicInfoData itemData = qvariant_cast<MusicInfoData>(currentIndex.data(MusicInfoDataRole));
        emit musicLiked(itemData);
        qDebug() << "PlayListWidget: 喜爱信号发出, 数据为:" << itemData.musicName;
    }
    else {
        qDebug() << "PlayListWidget: 当前索引无效";
    }
}

void PlayListWidget::handlePlay()
{
    qDebug() << "PlayListWidget: handlePlay 函数被调用";
    if (currentIndex.isValid()) {
        MusicInfoData itemData = qvariant_cast<MusicInfoData>(currentIndex.data(MusicInfoDataRole));
        emit musicPlayRequested(itemData);
        qDebug() << "PlayListWidget: 播放信号发出, 数据为:" << itemData.musicName;
    }
    else {
        qDebug() << "PlayListWidget: 当前索引无效";
    }
}

void PlayListWidget::updatePlayList(const QList<MusicInfoData>& newMusicList)
{
    qDebug() << "PlayListWidget: updatePlayList 函数被调用";
    loadPlayList(newMusicList);
}

void PlayListWidget::loadPlayList(const QList<MusicInfoData>& musicList)
{
    qDebug() << "PlayListWidget: loadPlayList 函数被调用";
    model->clear();
    for (const MusicInfoData& music : musicList) {
        QStandardItem* item = new QStandardItem;
        item->setData(QVariant::fromValue(music), MusicInfoDataRole);
        model->appendRow(item);
    }
    qDebug() << "PlayListWidget: 播放列表加载完毕";
}
