#include "MusicPlayerList.h"

MusicPlayerList::MusicPlayerList(QObject* parent)
    : QObject(parent),
    currentPlaylistIndex(0)
{
    musicPlayer = new MusicPlayer(this);
    lyricAnalyzer = new LyricAnalyzer(this);
    playlist = new QMediaPlaylist(this);

    playlist->setPlaybackMode(QMediaPlaylist::PlaybackMode::CurrentItemInLoop);//默认循环播放

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
    emit playlistChanged(playlistData); // 发射播放列表改变信号
}

void MusicPlayerList::addMusic(const MusicInfoData& music)
{
    // 检查音乐是否已存在
    for (const MusicInfoData& existingMusic : playlistData) {
        if (existingMusic.id == music.id) {
            qDebug() << "音乐已存在:==" << music.id;
            return; // 如果已存在，则直接返回
        }
    }

    qDebug() << "添加音乐:==" << music.id;
    playlist->addMedia(QUrl(music.filePath));
    playlistData.append(music); // 添加到播放列表数据集合
    emit playlistChanged(playlistData); // 发射播放列表改变信号
}

void MusicPlayerList::addAndPlayMusic(const MusicInfoData& music)
{
    // 检查音乐是否已存在，如果存在则删除
    for (int i = 0; i < playlistData.size(); ++i) {
        if (playlistData[i].id == music.id) {
            qDebug() << "音乐已存在，先删除:==" << music.id;
            playlist->removeMedia(i);
            playlistData.removeAt(i);
            break;
        }
    }

    qDebug() << "添加并播放音乐:==" << music.id;

    // 将音乐添加到播放列表的第一个位置
    playlist->insertMedia(0, QUrl(music.filePath));

    // 将音乐添加到播放列表数据集合的第一个位置
    playlistData.prepend(music);

    // 发射播放列表改变信号
    emit playlistChanged(playlistData);

    // 设置播放列表的当前索引为 0 并开始播放
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
    playlist->next(); // 让QMediaPlaylist选择下一个曲目
    currentPlaylistIndex = playlist->currentIndex(); // 获取当前索引并更新到currentPlaylistIndex

    if (currentPlaylistIndex >= 0 && currentPlaylistIndex < playlistData.size()) {
        openUrl(playlistData[currentPlaylistIndex]);
        emit playStateChange(true);
        emit playlistChanged(playlistData); // 发射播放列表改变信号
    }
}


void MusicPlayerList::playPrevious()
{
    playlist->previous(); // 让QMediaPlaylist选择上一个曲目
    currentPlaylistIndex = playlist->currentIndex(); // 获取当前索引并更新到currentPlaylistIndex

    if (currentPlaylistIndex >= 0 && currentPlaylistIndex < playlistData.size()) {
        openUrl(playlistData[currentPlaylistIndex]);
        emit playStateChange(true);
        emit playlistChanged(playlistData); // 发射播放列表改变信号
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
        emit playlistChanged(playlistData); // 发射播放列表改变信号
    }
}

//改变播放模式
void MusicPlayerList::changePlayMode(PlayMode playMode)
{
    qDebug() << "MusicPlayerList:播放模式改变=================" <<
        "==循环:" << (playMode == PlayMode::Loop) <<
        "==随机:" << (playMode == PlayMode::Random) <<
        "==顺序:" << (playMode == PlayMode::Sequential) <<
        "==单曲循环:" << (playMode == PlayMode::SingleLoop);
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
