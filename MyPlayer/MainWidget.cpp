#include "MainWidget.h"
#include <QApplication>
#include <QPushButton>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QIcon>

MainWidget::MainWidget(QWidget* parent)
    : QWidget(parent), playListVisible(false) {
    // ��������ʼ�����
    m_MiddleWidget = new MiddleWidget(this);
    m_BottomWidget = new BottomWidget(this);
    m_MusicPlayerWidget = new MusicPlayerWidget(this);
    m_PlayListWidget = new PlayListWidget(this);

    // ʹ��QStackedWidget������MiddleWidget��MusicPlayerWidget
    m_StackedWidget = new QStackedWidget(this);
    m_StackedWidget->addWidget(m_MiddleWidget);
    m_StackedWidget->addWidget(m_MusicPlayerWidget);

    resize(1280, 720); // ���ô���Ϊ1280x720

    // ���ò���
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_StackedWidget);
    mainLayout->addWidget(m_BottomWidget);
    setLayout(mainLayout);

    // ���ò����б���
    m_PlayListWidget->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    m_PlayListWidget->hide();

    // ��ʼ��Model
    m_MusicListManager = new MusicListManager(this);
    m_MusicPlayerList = new MusicPlayerList(this);

    // ����BottomWidget���źŵ�MainWidget�Ĳ�
    connect(m_BottomWidget->m_PlaySongInfoWidget, &PlaySongInfoWidget::toggleSongDetails, this, &MainWidget::toggleMiddleAndMusicPlayer);

    // ����BottomWidget�İ�ť�źŵ�MainWidget�Ĳ�
    connect(m_BottomWidget->listBtn, &QPushButton::clicked, this, &MainWidget::togglePlayList);

    // ����Model���źŵ��ۺ���
    // MusicListManager���б�
    connect(m_PlayListWidget, &PlayListWidget::itemClicked, m_MusicPlayerList, &MusicPlayerList::setPlaylistPosition);
    connect(m_MiddleWidget->m_Stack->onlineMusicPage, &MiddleWidgetRightBase::musicSelected, m_MusicPlayerList, &MusicPlayerList::addAndPlayMusic);
    connect(m_MiddleWidget->m_Stack->localMusicPage, &MiddleWidgetRightBase::musicSelected, m_MusicPlayerList, &MusicPlayerList::addAndPlayMusic);
    connect(m_MiddleWidget->m_Stack->favoritePage, &MiddleWidgetRightBase::musicSelected, m_MusicPlayerList, &MusicPlayerList::addAndPlayMusic);
    // �б�䶯
    connect(m_MusicListManager, &MusicListManager::localMusicLoaded, m_MiddleWidget->m_Stack->localMusicPage, &MiddleWidgetRightBase::updateMusicList);
    connect(m_MusicListManager, &MusicListManager::remoteMusicLoaded, m_MiddleWidget->m_Stack->onlineMusicPage, &MiddleWidgetRightBase::updateMusicList);
    connect(m_MusicListManager, &MusicListManager::favoriteMusicLoaded, m_MiddleWidget->m_Stack->favoritePage, &MiddleWidgetRightBase::updateMusicList);

    // MusicPlayerList�벥���б�
    connect(m_MusicPlayerList, &MusicPlayerList::playlistChanged, m_PlayListWidget, &PlayListWidget::updatePlayList);

    // MusicPlayerList��ײ���ť
    // ����
    connect(m_BottomWidget, &BottomWidget::playStateChanged, m_MusicPlayerList, &MusicPlayerList::playStateChanged);
    connect(m_BottomWidget, &BottomWidget::volumeChanged, m_MusicPlayerList, &MusicPlayerList::setVolume);
    connect(m_BottomWidget, &BottomWidget::playNextClicked, m_MusicPlayerList, &MusicPlayerList::playNext);
    connect(m_BottomWidget, &BottomWidget::playPreviousClicked, m_MusicPlayerList, &MusicPlayerList::playPrevious);
    connect(m_BottomWidget, &BottomWidget::progressChanged, m_MusicPlayerList, &MusicPlayerList::setPosition);
    connect(m_BottomWidget, &BottomWidget::playModeChanged, m_MusicPlayerList, &MusicPlayerList::changePlayMode);
    // ����
    connect(m_MusicPlayerList, &MusicPlayerList::currentMusicChanged, m_BottomWidget, &BottomWidget::onMusicChanged);
    connect(m_MusicPlayerList, &MusicPlayerList::playStateChange, m_BottomWidget, &BottomWidget::onPlayStateChanged);
    connect(m_MusicPlayerList, &MusicPlayerList::positionChanged, m_BottomWidget, &BottomWidget::onProgressChanged);
    connect(m_MusicPlayerList, &MusicPlayerList::lyricsChanged, m_MusicPlayerWidget, &MusicPlayerWidget::updateLyrics);
    connect(m_MusicPlayerList, &MusicPlayerList::currentMusicChanged, m_MusicPlayerWidget, &MusicPlayerWidget::updateCurrentSongInfo);

    // �Ҽ��˵��İ�
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

    // ϲ��
    connect(m_BottomWidget->m_PlaySongInfoWidget, &PlaySongInfoWidget::loved, m_MusicListManager, &MusicListManager::addToFavorites);
    connect(m_BottomWidget->m_PlaySongInfoWidget, &PlaySongInfoWidget::nolove, m_MusicListManager, &MusicListManager::removeFromFavorites);

    connect(m_MiddleWidget, &MiddleWidget::onLoadButtonClicked, m_MusicListManager, &MusicListManager::onLoadLocalMusic);
    connect(m_MusicListManager->musicLoader, &MusicLoader::result, m_MiddleWidget, &MiddleWidget::loadButtonResult);

    // ��������ͼ��Ͳ˵�
    trayIcon = new QSystemTrayIcon(this);
    trayIconMenu = new QMenu(this);

    minimizeAction = new QAction(tr("��С��"), this);
    restoreAction = new QAction(tr("��ԭ"), this);
    quitAction = new QAction(tr("�˳�"), this);

    connect(minimizeAction, &QAction::triggered, this, &QWidget::showMinimized);
    connect(restoreAction, &QAction::triggered, this, &MainWidget::restoreWindow);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(QIcon(":/images/tray_icon.png")); // ʹ��ͼ��
    trayIcon->setToolTip(tr("���ֲ�����"));

    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWidget::iconActivated);

    trayIcon->show();

    // ���ر������֣�Զ�����ֺ�ϲ������
    m_MusicListManager->loadAllMusic(); // ���ݿ�
    // Ӳ������ʹ̶�Զ�ˣ�������������ݿ����
    m_MusicListManager->loadRemoteMusic("https://gitee.com/MarkYangUp/music/raw/master/������/music.json");
    m_MusicListManager->loadRemoteMusic("https://gitee.com/MarkYangUp/music/raw/master/��������/music.json");

    // �������ط�����
    localServer = new QLocalServer(this);
    if (localServer->listen("UniqueApplicationKey:MyPlayer^...^")) {
        connect(localServer, &QLocalServer::newConnection, this, &MainWidget::handleNewConnection);
    }
}

MainWidget::~MainWidget() {
    // ����
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
        m_PlayListWidget->setFixedSize(300, this->height() - m_BottomWidget->height()); // ���ò����б�Ĵ�С
        // ��ȡ�����б��ڵĴ�С
        int width = m_PlayListWidget->width();
        int height = m_PlayListWidget->height();

        // �������б����ƶ������ʵ�λ��
        int x = this->width() - width; // �Ҳ����
        int y = this->height() - m_BottomWidget->height() - height; // �ײ�����
        m_PlayListWidget->setGeometry(x, y, width, height);
        m_PlayListWidget->show();
        m_PlayListWidget->raise(); // �������б�����������ǰ��
    }
    playListVisible = !playListVisible;
}

void MainWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    if (playListVisible) {
        m_PlayListWidget->setFixedSize(300, this->height() - m_BottomWidget->height()); // ���ò����б�Ĵ�С
        // ��ȡ�����б��ڵĴ�С
        int width = m_PlayListWidget->width();
        int height = m_PlayListWidget->height();

        // �������б����ƶ������ʵ�λ��
        int x = this->width() - width; // �Ҳ����
        int y = this->height() - m_BottomWidget->height() - height; // �ײ�����
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
    trayIcon->showMessage(tr("������Ϣ"),
        tr("����һ��������Ϣʾ����"),
        QSystemTrayIcon::Information,
        5000);
}

void MainWidget::messageClicked() {
    QMessageBox::information(nullptr, tr("��Ϣ���"), tr("��������Ϣ��"));
}

void MainWidget::closeEvent(QCloseEvent* event) {
    if (trayIcon->isVisible()) {
        hide();  // ���ش���
        event->ignore();  // ���Թر��¼�
    }
}

void MainWidget::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    update(); // ���»��ƴ���
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
