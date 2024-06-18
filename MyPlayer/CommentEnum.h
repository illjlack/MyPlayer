#pragma once
#pragma execution_character_set("utf-8")

#include <QObject>
#include <QMediaPlaylist>

// �Զ����ɫö��
enum CustomRoles {
    MusicInfoDataRole = Qt::UserRole + 1
};

// ����ģʽö��
enum class PlayMode {
    SingleLoop = 1,
    Sequential = 2,
    Random = 3,
    Loop = 4 
};

// ��ȡ��һ������ģʽ�ĺ�������
PlayMode getNextPlayMode(PlayMode currentMode);

// ת��ΪQMediaPlaylist����ģʽ�ĺ�������
QMediaPlaylist::PlaybackMode convertToQMediaPlaylistPlayMode(PlayMode mode);
