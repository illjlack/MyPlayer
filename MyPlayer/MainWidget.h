#pragma once
#pragma execution_character_set("utf-8")

#include <QWidget>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QStackedWidget>
#include <QLocalServer>
#include <QLocalSocket>
#include "MiddleWidget.h" // 中部歌曲信息显示部件类
#include "BottomWidget.h" // 底部控制栏类
#include "MusicPlayerWidget.h" // 播放页面
#include "PlayListWidget.h"
#include "MusicListManager.h" // 管理本地，远端，喜爱音乐
#include "MusicPlayerList.h" // 正在播放列表，播放

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget* parent = nullptr);
    ~MainWidget();

protected:
    void resizeEvent(QResizeEvent* event) override;
    void closeEvent(QCloseEvent* event) override; // 隐藏原来的关闭窗口定义
    void showEvent(QShowEvent* event) override;

private slots:
    void toggleMiddleAndMusicPlayer(bool isExpanded);
    void togglePlayList();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void showMessage();
    void messageClicked();
    void restoreWindow();
    void handleNewConnection();

private:
    MiddleWidget* m_MiddleWidget; // 中部歌曲歌单信息显示部件
    BottomWidget* m_BottomWidget; // 底部控制栏部件
    MusicPlayerWidget* m_MusicPlayerWidget; // 播放页面
    QStackedWidget* m_StackedWidget;
    PlayListWidget* m_PlayListWidget;

    bool playListVisible;

    // Model 实例
    MusicListManager* m_MusicListManager;
    MusicPlayerList* m_MusicPlayerList;

    // 系统托盘图标和菜单
    QSystemTrayIcon* trayIcon;
    QMenu* trayIconMenu;
    QAction* minimizeAction;
    QAction* restoreAction;
    QAction* quitAction;

    QLocalServer* localServer;
};
