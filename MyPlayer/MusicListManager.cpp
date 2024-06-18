#include "MusicListManager.h"
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QFileDialog>

// ���캯��
MusicListManager::MusicListManager(QObject* parent)
    : QObject(parent),
    musicLoader(new MusicLoader(this)),
    musicInfoManager(MusicInfoManager::instance())
{
    connect(musicLoader, &MusicLoader::musicDataLoaded, this, &MusicListManager::onMusicDataLoaded);
    connect(musicLoader, &MusicLoader::musicAddLocal, this, &MusicListManager::loadAllMusic);
    connect(musicLoader, &MusicLoader::musicAddRemote, this, &MusicListManager::loadAllMusic);
}

// ���ش����ݿ����������б�
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
    qDebug() << "MusicListManager:���ر��������б�ed====================================��ʼ";
    emit remoteMusicLoaded(remoteMusicList);
    qDebug() << "MusicListManager:����Զ�������б�ed====================================��ʼ";
    emit favoriteMusicLoaded(favoriteMusicList);
    qDebug() << "MusicListManager:����ϲ���б�ed";
}

// ���ر��������б�
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
        musicInfoManager.addOrUpdateMusic(music); // ��ӵ����ݿ�
    }

    localMusicList = musicList;
    emit localMusicLoaded(localMusicList);
}

// ����Զ�������б�
void MusicListManager::loadRemoteMusic(const QString& url)
{
    musicLoader->loadFromUrl(url);
}

// ����ϲ���б�
void MusicListManager::loadFavoriteMusic()
{
    favoriteMusicList = musicInfoManager.getLovedMusic();
    emit favoriteMusicLoaded(favoriteMusicList);
    qDebug() << "MusicListManager:����ϲ���б�ed";
}

// ��ӵ�ϲ���б�
void MusicListManager::addToFavorites(const MusicInfoData& music)
{
    qDebug() << "MusicListManager:��ӵ�ϲ���б�" << music.albumName;
    musicInfoManager.setLoveStatus(music.id, true);
    favoriteMusicList = musicInfoManager.getLovedMusic();
    emit favoriteMusicLoaded(favoriteMusicList);
    qDebug() << "MusicListManager:����ϲ���б�ed";
}

// ��ϲ���б��Ƴ�
void MusicListManager::removeFromFavorites(const MusicInfoData& music)
{
    musicInfoManager.setLoveStatus(music.id, false);
    favoriteMusicList = musicInfoManager.getLovedMusic();
    emit favoriteMusicLoaded(favoriteMusicList);
}

// �ۺ����������������ݼ������
void MusicListManager::onMusicDataLoaded(const QList<MusicInfoData>& musicList)
{
    for (const MusicInfoData& music : musicList) {
        musicInfoManager.addOrUpdateMusic(music); // ��ӵ����ݿ�
    }
    //remoteMusicList = musicList;
    //emit remoteMusicLoaded(remoteMusicList);
}

void MusicListManager::onLoadLocalMusic() {
    QString filePath = QFileDialog::getOpenFileName(
        nullptr,
        "ѡ���ļ�",
        "",
        "��Ƶ�ļ� (*.mp3 *.wav);;JSON �ļ� (*.json)"
    );

    if (filePath.isEmpty()) {
        return; // ����û�ȡ���ļ�ѡ��ֱ�ӷ���
    }

    // ����ļ���չ��
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
