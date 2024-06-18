#include "MainWidget.h"
#include <QApplication>
#include <QPushButton>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QIcon>

MainWidget::MainWidget(QWidget* parent)
    : QWidget(parent), playListVisible(false) {
    // 创建并初始化组件
    m_MiddleWidget = new MiddleWidget(this);
    m_BottomWidget = new BottomWidget(this);
    m_MusicPlayerWidget = new MusicPlayerWidget(this);
    m_PlayListWidget = new PlayListWidget(this);

    // 使用QStackedWidget来管理MiddleWidget和MusicPlayerWidget
    m_StackedWidget = new QStackedWidget(this);
    m_StackedWidget->addWidget(m_MiddleWidget);
    m_StackedWidget->addWidget(m_MusicPlayerWidget);

    resize(1280, 720); // 设置窗口为1280x720

    // 设置布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_StackedWidget);
    mainLayout->addWidget(m_BottomWidget);
    setLayout(mainLayout);

    // 设置播放列表窗口
    m_PlayListWidget->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    m_PlayListWidget->hide();

    // 初始化Model
    m_MusicListManager = new MusicListManager(this);
    m_MusicPlayerList = new MusicPlayerList(this);

    // 连接BottomWidget的信号到MainWidget的槽
    connect(m_BottomWidget->m_PlaySongInfoWidget, &PlaySongInfoWidget::toggleSongDetails, this, &MainWidget::toggleMiddleAndMusicPlayer);

    // 连接BottomWidget的按钮信号到MainWidget的槽
    connect(m_BottomWidget->listBtn, &QPushButton::clicked, this, &MainWidget::togglePlayList);

    // 连接Model的信号到槽函数
    // MusicListManager与列表
    connect(m_PlayListWidget, &PlayListWidget::itemClicked, m_MusicPlayerList, &MusicPlayerList::setPlaylistPosition);
    connect(m_MiddleWidget->m_Stack->onlineMusicPage, &MiddleWidgetRightBase::musicSelected, m_MusicPlayerList, &MusicPlayerList::addAndPlayMusic);
    connect(m_MiddleWidget->m_Stack->localMusicPage, &MiddleWidgetRightBase::musicSelected, m_MusicPlayerList, &MusicPlayerList::addAndPlayMusic);
    connect(m_MiddleWidget->m_Stack->favoritePage, &MiddleWidgetRightBase::musicSelected, m_MusicPlayerList, &MusicPlayerList::addAndPlayMusic);
    // 列表变动
    connect(m_MusicListManager, &MusicListManager::localMusicLoaded, m_MiddleWidget->m_Stack->localMusicPage, &MiddleWidgetRightBase::updateMusicList);
    connect(m_MusicListManager, &MusicListManager::remoteMusicLoaded, m_MiddleWidget->m_Stack->onlineMusicPage, &MiddleWidgetRightBase::updateMusicList);
    connect(m_MusicListManager, &MusicListManager::favoriteMusicLoaded, m_MiddleWidget->m_Stack->favoritePage, &MiddleWidgetRightBase::updateMusicList);

    // MusicPlayerList与播放列表
    connect(m_MusicPlayerList, &MusicPlayerList::playlistChanged, m_PlayListWidget, &PlayListWidget::updatePlayList);

    // MusicPlayerList与底部按钮
    // 主动
    connect(m_BottomWidget, &BottomWidget::playStateChanged, m_MusicPlayerList, &MusicPlayerList::playStateChanged);
    connect(m_BottomWidget, &BottomWidget::volumeChanged, m_MusicPlayerList, &MusicPlayerList::setVolume);
    connect(m_BottomWidget, &BottomWidget::playNextClicked, m_MusicPlayerList, &MusicPlayerList::playNext);
    connect(m_BottomWidget, &BottomWidget::playPreviousClicked, m_MusicPlayerList, &MusicPlayerList::playPrevious);
    connect(m_BottomWidget, &BottomWidget::progressChanged, m_MusicPlayerList, &MusicPlayerList::setPosition);
    connect(m_BottomWidget, &BottomWidget::playModeChanged, m_MusicPlayerList, &MusicPlayerList::changePlayMode);
    // 被动
    connect(m_MusicPlayerList, &MusicPlayerList::currentMusicChanged, m_BottomWidget, &BottomWidget::onMusicChanged);
    connect(m_MusicPlayerList, &MusicPlayerList::playStateChange, m_BottomWidget, &BottomWidget::onPlayStateChanged);
    connect(m_MusicPlayerList, &MusicPlayerList::positionChanged, m_BottomWidget, &BottomWidget::onProgressChanged);
    connect(m_MusicPlayerList, &MusicPlayerList::lyricsChanged, m_MusicPlayerWidget, &MusicPlayerWidget::updateLyrics);
    connect(m_MusicPlayerList, &MusicPlayerList::currentMusicChanged, m_MusicPlayerWidget, &MusicPlayerWidget::updateCurrentSongInfo);

    // 右键菜单的绑定
    connect(m_PlayListWidget, &PlayListWidget::musicLiked, m_MusicListManager, &MusicListManager::addToFavorites);
    connect(m_PlayListWidget, &PlayListWidget::musicPlayRequested, m_MusicPlayerList, &MusicPlayerList::setPlaylistPosition);

    connect(m_MiddleWidget->m_Stack->onlineMusicPage, &MiddleWidgetRightBase::musicLiked, m_MusicListManager, &MusicListManager::addToFavorites);
    connect(m_MiddleWidget->m_Stack->onlineMusicPage, &MiddleWidgetRightBase::musicPlayRequested, m_MusicPlayerList, &MusicPlayerList::addAndPlayMusic);
    connect(m_MiddleWidget->m_Stack->onlineMusicPage, &MiddleWidgetRightBase::musicAddedToPlaylist, m_MusicPlayerList, &MusicPlayerList::addMusic);
    connect(m_MiddleWidget->m_Stack->localMusicPage, &MiddleWidgetRightBase::musicLiked, m_MusicListManager, &MusicListManager::addToFavorites);
    connect(m_MiddleWidget->m_Stack->localMusicPage, &MiddleWidgetRightBase::musicPlayRequested, m_MusicPlayerList, &MusicPlayerList::addAndPlayMusic);
    connect(m_MiddleWidget->m_Stack->localMusicPage, &MiddleWidgetRightBase::musicAddedToPlaylist, m_MusicPlayerList, &MusicPlayerList::addMusic);
    connect(m_MiddleWidget->m_Stack->favoritePage, &MiddleWidgetRightBase::musicLiked, m_MusicListManager, &MusicListManager::addToFavorites);
    connect(m_MiddleWidget->m_Stack->favoritePage, &MiddleWidgetRightBase::musicPlayRequested, m_MusicPlayerList, &MusicPlayerList::addAndPlayMusic);
    connect(m_MiddleWidget->m_Stack->favoritePage, &MiddleWidgetRightBase::musicAddedToPlaylist, m_MusicPlayerList, &MusicPlayerList::addMusic);

    // 喜爱
    connect(m_BottomWidget->m_PlaySongInfoWidget, &PlaySongInfoWidget::loved, m_MusicListManager, &MusicListManager::addToFavorites);
    connect(m_BottomWidget->m_PlaySongInfoWidget, &PlaySongInfoWidget::nolove, m_MusicListManager, &MusicListManager::removeFromFavorites);

    connect(m_MiddleWidget, &MiddleWidget::onLoadButtonClicked, m_MusicListManager, &MusicListManager::onLoadLocalMusic);
    connect(m_MusicListManager->musicLoader, &MusicLoader::result, m_MiddleWidget, &MiddleWidget::loadButtonResult);

    // 创建托盘图标和菜单
    trayIcon = new QSystemTrayIcon(this);
    trayIconMenu = new QMenu(this);

    minimizeAction = new QAction(tr("最小化"), this);
    restoreAction = new QAction(tr("还原"), this);
    quitAction = new QAction(tr("退出"), this);

    connect(minimizeAction, &QAction::triggered, this, &QWidget::showMinimized);
    connect(restoreAction, &QAction::triggered, this, &MainWidget::restoreWindow);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(QIcon(":/images/tray_icon.png")); // 使用图标
    trayIcon->setToolTip(tr("音乐播放器"));

    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWidget::iconActivated);

    trayIcon->show();

    // 加载本地音乐，远程音乐和喜爱音乐
    m_MusicListManager->loadAllMusic(); // 数据库
    // 硬编码访问固定远端，后面可以在数据库添加
    m_MusicListManager->loadRemoteMusic("https://gitee.com/MarkYangUp/music/raw/master/孤勇者/music.json");
    m_MusicListManager->loadRemoteMusic("https://gitee.com/MarkYangUp/music/raw/master/风雨无阻/music.json");

    // 创建本地服务器
    localServer = new QLocalServer(this);
    if (localServer->listen("UniqueApplicationKey:MyPlayer^...^")) {
        connect(localServer, &QLocalServer::newConnection, this, &MainWidget::handleNewConnection);
    }
}

MainWidget::~MainWidget() {
    // 清理
    if (localServer) {
        localServer->close();
        delete localServer;
    }
}

void MainWidget::toggleMiddleAndMusicPlayer(bool isExpanded) {
    m_StackedWidget->setCurrentIndex(isExpanded ? 1 : 0);
}

void MainWidget::togglePlayList() {
    if (playListVisible) {
        m_PlayListWidget->hide();
    }
    else {
        m_PlayListWidget->setFixedSize(300, this->height() - m_BottomWidget->height()); // 设置播放列表的大小
        // 获取播放列表窗口的大小
        int width = m_PlayListWidget->width();
        int height = m_PlayListWidget->height();

        // 将播放列表窗口移动到合适的位置
        int x = this->width() - width; // 右侧对齐
        int y = this->height() - m_BottomWidget->height() - height; // 底部对齐
        m_PlayListWidget->setGeometry(x, y, width, height);
        m_PlayListWidget->show();
        m_PlayListWidget->raise(); // 将播放列表窗口提升到最前面
    }
    playListVisible = !playListVisible;
}

void MainWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    if (playListVisible) {
        m_PlayListWidget->setFixedSize(300, this->height() - m_BottomWidget->height()); // 设置播放列表的大小
        // 获取播放列表窗口的大小
        int width = m_PlayListWidget->width();
        int height = m_PlayListWidget->height();

        // 将播放列表窗口移动到合适的位置
        int x = this->width() - width; // 右侧对齐
        int y = this->height() - m_BottomWidget->height() - height; // 底部对齐
        m_PlayListWidget->setGeometry(x, y, width, height);
    }
}

void MainWidget::iconActivated(QSystemTrayIcon::ActivationReason reason) {
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        restoreWindow();
        break;
    case QSystemTrayIcon::MiddleClick:
        showMessage();
        break;
    default:
        break;
    }
}

void MainWidget::showMessage() {
    trayIcon->showMessage(tr("托盘消息"),
        tr("这是一个托盘消息示例。"),
        QSystemTrayIcon::Information,
        5000);
}

void MainWidget::messageClicked() {
    QMessageBox::information(nullptr, tr("消息点击"), tr("你点击了消息。"));
}

void MainWidget::closeEvent(QCloseEvent* event) {
    if (trayIcon->isVisible()) {
        hide();  // 隐藏窗口
        event->ignore();  // 忽略关闭事件
    }
}

void MainWidget::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    update(); // 重新绘制窗口
}

void MainWidget::restoreWindow() {
    showNormal();
    activateWindow();
    raise();
}

void MainWidget::handleNewConnection() {
    QLocalSocket* socket = localServer->nextPendingConnection();
    if (socket) {
        socket->waitForReadyRead(1000);
        restoreWindow();
        socket->close();
        delete socket;
    }
}
