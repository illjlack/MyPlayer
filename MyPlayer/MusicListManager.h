#pragma once
#pragma execution_character_set("utf-8")

#include <QObject>
#include "MusicLoader.h"
#include "MusicInfoManager.h"

// �����б�����࣬������������֡�Զ�����ֺ�ϲ���б�
class MusicListManager : public QObject
{
    Q_OBJECT

public:
    MusicLoader* musicLoader;  // ���ּ�����

    explicit MusicListManager(QObject* parent = nullptr);

    // �����ݿ�������������б�
    void loadAllMusic();

    // ���ر��������б�
    void loadLocalMusic(const QString& filePath);

    // ����Զ�������б�
    void loadRemoteMusic(const QString& url);

    // ����ϲ���б�
    void loadFavoriteMusic();

    // ��ӵ�ϲ���б�
    void addToFavorites(const MusicInfoData& music);

    // ��ϲ���б��Ƴ�
    void removeFromFavorites(const MusicInfoData& music);

signals:
    // �źţ��������б����ʱ����
    void localMusicLoaded(const QList<MusicInfoData>& musicList);
    void remoteMusicLoaded(const QList<MusicInfoData>& musicList);
    void favoriteMusicLoaded(const QList<MusicInfoData>& musicList);

public slots:
    void onLoadLocalMusic();

private slots:
    // �ۺ����������������ݼ������
    void onMusicDataLoaded(const QList<MusicInfoData>& musicList);

private:
    
    MusicInfoManager& musicInfoManager; // ������Ϣ������
    QList<MusicInfoData> localMusicList;  // ���������б�
    QList<MusicInfoData> remoteMusicList; // Զ�������б�
    QList<MusicInfoData> favoriteMusicList; // ϲ�������б�
};
