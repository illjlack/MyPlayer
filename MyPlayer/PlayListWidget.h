#pragma once
#pragma execution_character_set("utf-8")

#include <QWidget>
#include <QListView>
#include <QStandardItemModel>
#include <QMenu>
#include "MusicPlayListItemDelegate.h"
#include "MusicInfoData.h"

class PlayListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlayListWidget(QWidget* parent = nullptr);  // 构造函数
    void updatePlayList(const QList<MusicInfoData>& newMusicList);  // 更新播放列表

private slots:
    void handleItemClicked(const QModelIndex& index);  // 处理项点击事件
    void showContextMenu(const QPoint& pos);  // 显示右键上下文菜单
    void handleLike();  // 处理喜爱动作
    void handlePlay();  // 处理播放动作

signals:
    void itemClicked(const MusicInfoData& itemData);  // 当项被点击时发出信号
    void musicLiked(const MusicInfoData& itemData);  // 喜爱音乐时发出信号
    void musicPlayRequested(const MusicInfoData& itemData);  // 播放音乐时发出信号

private:
    void loadPlayList(const QList<MusicInfoData>& musicList);  // 加载播放列表

    QListView* listView;  // 列表视图控件
    QStandardItemModel* model;  // 标准项模型
    QMenu* contextMenu;  // 上下文菜单
    QModelIndex currentIndex;  // 当前索引
};
