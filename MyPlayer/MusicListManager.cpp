#include "MusicListManager.h"
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QFileDialog>

// 构造函数
MusicListManager::MusicListManager(QObject* parent)
    : QObject(parent),
    musicLoader(new MusicLoader(this)),
    musicInfoManager(MusicInfoManager::instance())
{
    connect(musicLoader, &MusicLoader::musicDataLoaded, this, &MusicListManager::onMusicDataLoaded);
    connect(musicLoader, &MusicLoader::musicAddLocal, this, &MusicListManager::loadAllMusic);
    connect(musicLoader, &MusicLoader::musicAddRemote, this, &MusicListManager::loadAllMusic);
}

// 加载从数据库所有音乐列表
void MusicListManager::loadAllMusic()
{
    QList<MusicInfoData> allMusic = musicInfoManager.getAllMusic();

    localMusicList.clear();
    remoteMusicList.clear();
    favoriteMusicList.clear();

    for (const MusicInfoData& music : allMusic) {
        if (music.isLocal) {
            localMusicList.append(music);
        }
        else {
            remoteMusicList.append(music);
        }
        if (music.isLove) {
            favoriteMusicList.append(music);
        }
    }

    emit localMusicLoaded(localMusicList);
    qDebug() << "MusicListManager:加载本地音乐列表ed====================================初始";
    emit remoteMusicLoaded(remoteMusicList);
    qDebug() << "MusicListManager:加载远程音乐列表ed====================================初始";
    emit favoriteMusicLoaded(favoriteMusicList);
    qDebug() << "MusicListManager:加载喜爱列表ed";
}

// 加载本地音乐列表
void MusicListManager::loadLocalMusic(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    QJsonArray jsonArray = doc.object().value("list").toArray();

    QList<MusicInfoData> musicList;
    for (const QJsonValue& value : jsonArray) {
        QJsonObject obj = value.toObject();
        MusicInfoData music;
        music.isLocal = true;
        music.path = obj.value("url").toString();
        music.musicName = obj.value("musicName").toString();
        music.albumName = obj.value("albumName").toString();
        music.duration = obj.value("duration").toInt();
        music.filePath = obj.value("filePath").toString();
        music.fileType = obj.value("fileType").toString();
        music.lyric = obj.value("lyric").toString();
        music.img = obj.value("img").toString();
        music.generateId();
        musicList.append(music);
        musicInfoManager.addOrUpdateMusic(music); // 添加到数据库
    }

    localMusicList = musicList;
    emit localMusicLoaded(localMusicList);
}

// 加载远程音乐列表
void MusicListManager::loadRemoteMusic(const QString& url)
{
    musicLoader->loadFromUrl(url);
}

// 加载喜爱列表
void MusicListManager::loadFavoriteMusic()
{
    favoriteMusicList = musicInfoManager.getLovedMusic();
    emit favoriteMusicLoaded(favoriteMusicList);
    qDebug() << "MusicListManager:加载喜爱列表ed";
}

// 添加到喜爱列表
void MusicListManager::addToFavorites(const MusicInfoData& music)
{
    qDebug() << "MusicListManager:添加到喜爱列表" << music.albumName;
    musicInfoManager.setLoveStatus(music.id, true);
    favoriteMusicList = musicInfoManager.getLovedMusic();
    emit favoriteMusicLoaded(favoriteMusicList);
    qDebug() << "MusicListManager:加载喜爱列表ed";
}

// 从喜爱列表移除
void MusicListManager::removeFromFavorites(const MusicInfoData& music)
{
    musicInfoManager.setLoveStatus(music.id, false);
    favoriteMusicList = musicInfoManager.getLovedMusic();
    emit favoriteMusicLoaded(favoriteMusicList);
}

// 槽函数，处理音乐数据加载完成
void MusicListManager::onMusicDataLoaded(const QList<MusicInfoData>& musicList)
{
    for (const MusicInfoData& music : musicList) {
        musicInfoManager.addOrUpdateMusic(music); // 添加到数据库
    }
    //remoteMusicList = musicList;
    //emit remoteMusicLoaded(remoteMusicList);
}

void MusicListManager::onLoadLocalMusic() {
    QString filePath = QFileDialog::getOpenFileName(
        nullptr,
        "选择文件",
        "",
        "音频文件 (*.mp3 *.wav);;JSON 文件 (*.json)"
    );

    if (filePath.isEmpty()) {
        return; // 如果用户取消文件选择，直接返回
    }

    // 检查文件扩展名
    QFileInfo fileInfo(filePath);
    QString suffix = fileInfo.suffix().toLower();

    if (suffix == "mp3" || suffix == "wav") {
        musicLoader->loadFromLocalFile(filePath);
    }
    else if (suffix == "json") {
        musicLoader->loadFromLocalJson(filePath);
    }
    else {
        return;
    }
}
