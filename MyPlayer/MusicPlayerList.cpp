#include "MusicPlayerList.h"

MusicPlayerList::MusicPlayerList(QObject* parent)
    : QObject(parent),
    currentPlaylistIndex(0)
{
    musicPlayer = new MusicPlayer(this);
    lyricAnalyzer = new LyricAnalyzer(this);
    playlist = new QMediaPlaylist(this);

    playlist->setPlaybackMode(QMediaPlaylist::PlaybackMode::CurrentItemInLoop);//Ĭ��ѭ������

    connect(musicPlayer, &MusicPlayer::positionChanged, lyricAnalyzer, &LyricAnalyzer::updateLyrics);
    connect(musicPlayer, &MusicPlayer::positionChanged, this, &MusicPlayerList::positionChanged);

    connect(playlist, &QMediaPlaylist::currentIndexChanged, this, &MusicPlayerList::onPlaylistPositionChanged);
    connect(musicPlayer, &MusicPlayer::playMusicChanged, this, &MusicPlayerList::currentMusicChanged);
    connect(musicPlayer, &MusicPlayer::playMusicChanged, lyricAnalyzer, &LyricAnalyzer::loadLyrics);
    connect(musicPlayer, &MusicPlayer::playbackEnded, this, &MusicPlayerList::playNext);

    connect(lyricAnalyzer, &LyricAnalyzer::lyricsChanged, this, [this](const QString& lyricsHtml) {
        emit lyricsChanged(lyricsHtml);
        });
}

MusicPlayerList::~MusicPlayerList()
{
}

void MusicPlayerList::openUrl(const MusicInfoData& music)
{
    musicPlayer->setCurrentMusic(music);
}

void MusicPlayerList::setPlaylist(const QList<MusicInfoData>& playlistData)
{
    playlist->clear();
    this->playlistData = playlistData;
    for (const MusicInfoData& music : playlistData) {
        playlist->addMedia(QUrl(music.filePath));
    }
    playlist->setCurrentIndex(0);
    currentPlaylistIndex = 0;
    emit playlistChanged(playlistData); // ���䲥���б�ı��ź�
}

void MusicPlayerList::addMusic(const MusicInfoData& music)
{
    // ��������Ƿ��Ѵ���
    for (const MusicInfoData& existingMusic : playlistData) {
        if (existingMusic.id == music.id) {
            qDebug() << "�����Ѵ���:==" << music.id;
            return; // ����Ѵ��ڣ���ֱ�ӷ���
        }
    }

    qDebug() << "�������:==" << music.id;
    playlist->addMedia(QUrl(music.filePath));
    playlistData.append(music); // ��ӵ������б����ݼ���
    emit playlistChanged(playlistData); // ���䲥���б�ı��ź�
}

void MusicPlayerList::addAndPlayMusic(const MusicInfoData& music)
{
    // ��������Ƿ��Ѵ��ڣ����������ɾ��
    for (int i = 0; i < playlistData.size(); ++i) {
        if (playlistData[i].id == music.id) {
            qDebug() << "�����Ѵ��ڣ���ɾ��:==" << music.id;
            playlist->removeMedia(i);
            playlistData.removeAt(i);
            break;
        }
    }

    qDebug() << "��Ӳ���������:==" << music.id;

    // ��������ӵ������б�ĵ�һ��λ��
    playlist->insertMedia(0, QUrl(music.filePath));

    // ��������ӵ������б����ݼ��ϵĵ�һ��λ��
    playlistData.prepend(music);

    // ���䲥���б�ı��ź�
    emit playlistChanged(playlistData);

    // ���ò����б�ĵ�ǰ����Ϊ 0 ����ʼ����
    playlist->setCurrentIndex(0);

    musicPlayer->setCurrentMusic(music);

    play();
}

void MusicPlayerList::play()
{
    musicPlayer->play();
    emit playStateChange(true);
}

void MusicPlayerList::pause()
{
    musicPlayer->pause();
    emit playStateChange(false);
}

void MusicPlayerList::stop()
{
    musicPlayer->stop();
    emit playStateChange(false);
}

void MusicPlayerList::playNext()
{
    playlist->next(); // ��QMediaPlaylistѡ����һ����Ŀ
    currentPlaylistIndex = playlist->currentIndex(); // ��ȡ��ǰ���������µ�currentPlaylistIndex

    if (currentPlaylistIndex >= 0 && currentPlaylistIndex < playlistData.size()) {
        openUrl(playlistData[currentPlaylistIndex]);
        emit playStateChange(true);
        emit playlistChanged(playlistData); // ���䲥���б�ı��ź�
    }
}


void MusicPlayerList::playPrevious()
{
    playlist->previous(); // ��QMediaPlaylistѡ����һ����Ŀ
    currentPlaylistIndex = playlist->currentIndex(); // ��ȡ��ǰ���������µ�currentPlaylistIndex

    if (currentPlaylistIndex >= 0 && currentPlaylistIndex < playlistData.size()) {
        openUrl(playlistData[currentPlaylistIndex]);
        emit playStateChange(true);
        emit playlistChanged(playlistData); // ���䲥���б�ı��ź�
    }
}


void MusicPlayerList::setVolume(int volume)
{
    musicPlayer->setVolume(volume);
}

void MusicPlayerList::setPosition(qint64 position)
{
    musicPlayer->setPosition(position);
}

void MusicPlayerList::loadLyrics(const MusicInfoData& music)
{
    lyricAnalyzer->loadLyrics(music);
}

void MusicPlayerList::onPositionChanged(qint64 position)
{
    lyricAnalyzer->updateLyrics(position);
}

void MusicPlayerList::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia) {
        playNext();
    }
}

void MusicPlayerList::onPlaylistPositionChanged(int position)
{
    currentPlaylistIndex = position;
    if (position >= 0 && position < playlistData.size()) {
        openUrl(playlistData[currentPlaylistIndex]);
        emit playlistChanged(playlistData); // ���䲥���б�ı��ź�
    }
}

//�ı䲥��ģʽ
void MusicPlayerList::changePlayMode(PlayMode playMode)
{
    qDebug() << "MusicPlayerList:����ģʽ�ı�=================" <<
        "==ѭ��:" << (playMode == PlayMode::Loop) <<
        "==���:" << (playMode == PlayMode::Random) <<
        "==˳��:" << (playMode == PlayMode::Sequential) <<
        "==����ѭ��:" << (playMode == PlayMode::SingleLoop);
    playlist->setPlaybackMode(convertToQMediaPlaylistPlayMode(playMode));
}

void MusicPlayerList::playStateChanged(bool playing) {
    if (playing) {
        musicPlayer->play();
    }
    else {
        musicPlayer->pause();
    }
}

void MusicPlayerList::setPlaylistPosition(const MusicInfoData& music) {
    int pos = playlistData.indexOf(music);
    playlist->setCurrentIndex(pos);
}
