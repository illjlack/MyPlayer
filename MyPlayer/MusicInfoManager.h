#pragma once
#pragma execution_character_set("utf-8")

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include "MusicInfoData.h"

class MusicInfoManager : public QObject {
    Q_OBJECT

public:
    static MusicInfoManager& instance();
    ~MusicInfoManager();

    // ��ӻ����������Ϣ
    bool addOrUpdateMusic(const MusicInfoData& music);

    // ��ȡ����������Ϣ
    QList<MusicInfoData> getAllMusic();

    // ��ȡ����ϲ����������Ϣ
    QList<MusicInfoData> getLovedMusic();

    // ����ID��ȡ������Ϣ
    MusicInfoData getMusicById(const QString& id);

    // �������ֵ�ϲ��״̬
    bool setLoveStatus(const QString& id, bool isLove);

    // ��������Ƿ����
    bool isMusicExists(const QString& id);

private:
    explicit MusicInfoManager(QObject* parent = nullptr);
    QSqlDatabase db;

    void initDatabase();

    // ��ֹ�����͸�ֵ
    MusicInfoManager(const MusicInfoManager&) = delete;
    MusicInfoManager& operator=(const MusicInfoManager&) = delete;

    void showError(const QString& message);
};
