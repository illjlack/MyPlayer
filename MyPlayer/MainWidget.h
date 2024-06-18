#pragma once
#pragma execution_character_set("utf-8")

#include <QWidget>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QStackedWidget>
#include <QLocalServer>
#include <QLocalSocket>
#include "MiddleWidget.h" // �в�������Ϣ��ʾ������
#include "BottomWidget.h" // �ײ���������
#include "MusicPlayerWidget.h" // ����ҳ��
#include "PlayListWidget.h"
#include "MusicListManager.h" // �����أ�Զ�ˣ�ϲ������
#include "MusicPlayerList.h" // ���ڲ����б�����

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget* parent = nullptr);
    ~MainWidget();

protected:
    void resizeEvent(QResizeEvent* event) override;
    void closeEvent(QCloseEvent* event) override; // ����ԭ���Ĺرմ��ڶ���
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
    MiddleWidget* m_MiddleWidget; // �в������赥��Ϣ��ʾ����
    BottomWidget* m_BottomWidget; // �ײ�����������
    MusicPlayerWidget* m_MusicPlayerWidget; // ����ҳ��
    QStackedWidget* m_StackedWidget;
    PlayListWidget* m_PlayListWidget;

    bool playListVisible;

    // Model ʵ��
    MusicListManager* m_MusicListManager;
    MusicPlayerList* m_MusicPlayerList;

    // ϵͳ����ͼ��Ͳ˵�
    QSystemTrayIcon* trayIcon;
    QMenu* trayIconMenu;
    QAction* minimizeAction;
    QAction* restoreAction;
    QAction* quitAction;

    QLocalServer* localServer;
};
