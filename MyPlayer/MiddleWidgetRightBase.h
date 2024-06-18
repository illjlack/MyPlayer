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
    explicit MiddleWidgetRightBase(QWidget* parent = nullptr);  // ���캯��
    void updateMusicList(const QList<MusicInfoData>& musicList);  // ���������б�

signals:
    void musicSelected(const MusicInfoData& itemData);  // ѡ������ʱ�����ź�
    void musicLiked(const MusicInfoData& itemData);  // ϲ������ʱ�����ź�
    void musicAddedToPlaylist(const MusicInfoData& itemData);  // ������ֵ��б�ʱ�����ź�
    void musicPlayRequested(const MusicInfoData& itemData);  // ��������ʱ�����ź�

protected slots:
    virtual void handleItemDoubleClicked(const QModelIndex& index);  // ������˫���¼�
    void showContextMenu(const QPoint& pos);  // ��ʾ�Ҽ������Ĳ˵�

private slots:
    void handleLike();  // ����ϲ������
    void handleAdd();  // ������ӵ��б���
    void handlePlay();  // �����Ŷ���

protected:
    QListView* listView;  // �б���ͼ�ؼ�
    QStandardItemModel* model;  // ����ģ��
    QMenu* contextMenu;  // �����Ĳ˵�
    QModelIndex currentIndex;  // ��ǰ����
};
