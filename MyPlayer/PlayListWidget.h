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
    explicit PlayListWidget(QWidget* parent = nullptr);  // ���캯��
    void updatePlayList(const QList<MusicInfoData>& newMusicList);  // ���²����б�

private slots:
    void handleItemClicked(const QModelIndex& index);  // ���������¼�
    void showContextMenu(const QPoint& pos);  // ��ʾ�Ҽ������Ĳ˵�
    void handleLike();  // ����ϲ������
    void handlePlay();  // �����Ŷ���

signals:
    void itemClicked(const MusicInfoData& itemData);  // ������ʱ�����ź�
    void musicLiked(const MusicInfoData& itemData);  // ϲ������ʱ�����ź�
    void musicPlayRequested(const MusicInfoData& itemData);  // ��������ʱ�����ź�

private:
    void loadPlayList(const QList<MusicInfoData>& musicList);  // ���ز����б�

    QListView* listView;  // �б���ͼ�ؼ�
    QStandardItemModel* model;  // ��׼��ģ��
    QMenu* contextMenu;  // �����Ĳ˵�
    QModelIndex currentIndex;  // ��ǰ����
};
