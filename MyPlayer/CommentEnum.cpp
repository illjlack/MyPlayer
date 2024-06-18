#include "CommentEnum.h"

// ��ȡ��һ������ģʽ�ĺ���ʵ��
PlayMode getNextPlayMode(PlayMode currentMode) {
    switch (currentMode) {
    case PlayMode::Sequential:
        return PlayMode::Random;
    case PlayMode::Random:
        return PlayMode::SingleLoop;
    case PlayMode::SingleLoop:
        return PlayMode::Loop;
    case PlayMode::Loop: 
        return PlayMode::Sequential;
    default:
        return PlayMode::Sequential;
    }
}

// ת��ΪQMediaPlaylist����ģʽ�ĺ���ʵ��
QMediaPlaylist::PlaybackMode convertToQMediaPlaylistPlayMode(PlayMode mode) {
    switch (mode) {
    case PlayMode::Sequential:
        return QMediaPlaylist::Sequential;
    case PlayMode::Random:
        return QMediaPlaylist::Random;
    case PlayMode::SingleLoop:
        return QMediaPlaylist::CurrentItemInLoop;
    case PlayMode::Loop: 
        return QMediaPlaylist::Loop;
    default:
        return QMediaPlaylist::Sequential;
    }
}
