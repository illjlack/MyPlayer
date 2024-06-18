#pragma once
#pragma execution_character_set("utf-8")

#include <QWidget>
#include <QListView>
#include <QStandardItemModel>
#include <QMenu>
#include "MusicInfoData.h"
#include "MusicListItemDelegate.h"

class MiddleWidgetRightBase : public QWidget
{
    Q_OBJECT

public:
    explicit MiddleWidgetRightBase(QWidget* parent = nullptr);  // 构造函数
    void updateMusicList(const QList<MusicInfoData>& musicList);  // 更新音乐列表

signals:
    void musicSelected(const MusicInfoData& itemData);  // 选中音乐时发出信号
    void musicLiked(const MusicInfoData& itemData);  // 喜爱音乐时发出信号
    void musicAddedToPlaylist(const MusicInfoData& itemData);  // 添加音乐到列表时发出信号
    void musicPlayRequested(const MusicInfoData& itemData);  // 播放音乐时发出信号

protected slots:
    virtual void handleItemDoubleClicked(const QModelIndex& index);  // 处理项双击事件
    void showContextMenu(const QPoint& pos);  // 显示右键上下文菜单

private slots:
    void handleLike();  // 处理喜爱动作
    void handleAdd();  // 处理添加到列表动作
    void handlePlay();  // 处理播放动作

protected:
    QListView* listView;  // 列表视图控件
    QStandardItemModel* model;  // 数据模型
    QMenu* contextMenu;  // 上下文菜单
    QModelIndex currentIndex;  // 当前索引
};
