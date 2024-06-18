#pragma once
#pragma execution_character_set("utf-8")

#include <QString>
#include <QMetaType>

// ������Ϣ�ṹ�壬���ڴ洢��������
struct MusicInfoData {
    QString id;         // ����ΨһID (hashKey)
    bool isLocal;       // �Ƿ�Ϊ��������
    QString musicName;  // ��������
    QString albumName;  // ר������
    int duration;       // ����ʱ�䣨�룩
    QString path;       // ����·����URL
    QString filePath;   // �ļ�·����URL
    QString lyric;      // ����ļ�·����URL
    QString img;        // ͼƬ�ļ�·����URL
    QString fileType;   // �ļ����ͣ����� mp3, wav, flac �ȣ�
    bool isLove;        // �Ƿ�ϲ��

    MusicInfoData()
        : isLocal(false),
        isLove(false),
        duration(0) {}

    void generateId() {
        id = musicName + "_" + albumName + "_" + (isLocal ? '1' : '0') + "_"+QString::number(duration) ;
    }

    // ���� == ��������QList��Ҫ
    bool operator==(const MusicInfoData& other) const {
        return id == other.id;
    }
};

// ���Զ�����������ע��ΪԪ����
Q_DECLARE_METATYPE(MusicInfoData)
