#include "MusicInfoManager.h"
#include <QDebug>
#include <QStandardPaths>
#include <QDir>
#include <QMessageBox>

MusicInfoManager& MusicInfoManager::instance() {
    static MusicInfoManager instance;
    return instance;
}

MusicInfoManager::MusicInfoManager(QObject* parent) : QObject(parent) {
    initDatabase();
}

MusicInfoManager::~MusicInfoManager() {
    if (db.isOpen()) {
        db.close();
    }
}

// 初始化数据库
void MusicInfoManager::initDatabase() {
    QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + QDir::separator() + "music_info.db";

    // 创建数据库文件所在的文件夹
    QDir dbDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    if (!dbDir.exists()) {
        if (!dbDir.mkpath(".")) {
            qDebug() << "创建数据库文件夹失败。";
            showError("创建数据库文件夹失败，本地音乐信息和喜爱无法保存");
            return;
        }
    }

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);

    if (!db.open()) {
        // 检查数据库文件是否存在并可读写
        QFile dbFile(dbPath);
        if (!dbFile.exists() || !dbFile.isReadable() || !dbFile.isWritable()) {
            qDebug() << "数据库文件不可用，尝试使用内存数据库。";
            showError("数据库错误\n原因:" + db.lastError().text() + "\n未解决, 某些电脑不能创建(可能是第一次)，或许是权限问题, 已在尝试内存运行数据库\n(根据经验重启可以成功创建数据库)");
            db.setDatabaseName(":memory:");
        }
    }

    if (!db.open()) {
        qDebug() << "数据库打开失败:" << db.lastError().text();
        showError("内存数据库也没成功，根据经验重启可以创建本地数据库" + db.lastError().text());
        return;
    }

    QSqlQuery query;
    query.prepare(
        "CREATE TABLE IF NOT EXISTS Music ("
        "id TEXT PRIMARY KEY,"
        "isLocal INTEGER,"
        "musicName TEXT,"
        "albumName TEXT,"
        "duration INTEGER,"
        "path TEXT,"
        "filePath TEXT," // 支持更多文件类型
        "fileType TEXT," // 文件类型字段
        "lyric TEXT,"
        "img TEXT,"
        "isLove INTEGER)" // 喜爱字段
    );

    if (!query.exec()) {
        qDebug() << "创建表失败:\n原因：" << query.lastError().text();
        showError("数据库表创建失败，本地音乐信息和喜爱无法保存");
    }
}


void MusicInfoManager::showError(const QString& message) {
    QMessageBox::critical(nullptr, "数据库错误", message
    );
}

// 添加或更新音乐信息
bool MusicInfoManager::addOrUpdateMusic(const MusicInfoData& music) {
    if (isMusicExists(music.id)) {
        QSqlQuery query;
        query.prepare(
            "UPDATE Music SET isLocal = :isLocal, musicName = :musicName, albumName = :albumName, "
            "duration = :duration, path = :path, filePath = :filePath, fileType = :fileType, lyric = :lyric, img = :img WHERE id = :id"
        );
        query.bindValue(":isLocal", music.isLocal);
        query.bindValue(":musicName", music.musicName);
        query.bindValue(":albumName", music.albumName);
        query.bindValue(":duration", music.duration);
        query.bindValue(":path", music.path);
        query.bindValue(":filePath", music.filePath);
        query.bindValue(":fileType", music.fileType);
        query.bindValue(":lyric", music.lyric);
        query.bindValue(":img", music.img);
        //query.bindValue(":isLove", music.isLove); // 喜爱保存在本地，不随远端更改
        query.bindValue(":id", music.id);

        if (!query.exec()) {
            qDebug() << "更新音乐失败:" << query.lastError().text();
            return false;
        }
    }
    else {
        QSqlQuery query;
        query.prepare(
            "INSERT INTO Music (id, isLocal, musicName, albumName, duration, path, filePath, fileType, lyric, img, isLove) "
            "VALUES (:id, :isLocal, :musicName, :albumName, :duration, :path, :filePath, :fileType, :lyric, :img, :isLove)"
        );
        query.bindValue(":id", music.id);
        query.bindValue(":isLocal", music.isLocal);
        query.bindValue(":musicName", music.musicName);
        query.bindValue(":albumName", music.albumName);
        query.bindValue(":duration", music.duration);
        query.bindValue(":path", music.path);
        query.bindValue(":filePath", music.filePath);
        query.bindValue(":fileType", music.fileType);
        query.bindValue(":lyric", music.lyric);
        query.bindValue(":img", music.img);
        query.bindValue(":isLove", music.isLove);

        if (!query.exec()) {
            qDebug() << "添加音乐失败:" << query.lastError().text();
            return false;
        }
    }

    return true;
}

// 获取所有音乐信息
QList<MusicInfoData> MusicInfoManager::getAllMusic() {
    QList<MusicInfoData> musicList;
    QSqlQuery query("SELECT * FROM Music");

    while (query.next()) {
        MusicInfoData music;
        music.id = query.value("id").toString();
        music.isLocal = query.value("isLocal").toBool();
        music.musicName = query.value("musicName").toString();
        music.albumName = query.value("albumName").toString();
        music.duration = query.value("duration").toInt();
        music.path = query.value("path").toString();
        music.filePath = query.value("filePath").toString();
        music.fileType = query.value("fileType").toString();
        music.lyric = query.value("lyric").toString();
        music.img = query.value("img").toString();
        music.isLove = query.value("isLove").toBool();

        musicList.append(music);
    }

    return musicList;
}

// 获取所有喜爱的音乐信息
QList<MusicInfoData> MusicInfoManager::getLovedMusic() {
    QList<MusicInfoData> musicList;
    QSqlQuery query("SELECT * FROM Music WHERE isLove = 1");

    while (query.next()) {
        MusicInfoData music;
        music.id = query.value("id").toString();
        music.isLocal = query.value("isLocal").toBool();
        music.musicName = query.value("musicName").toString();
        music.albumName = query.value("albumName").toString();
        music.duration = query.value("duration").toInt();
        music.path = query.value("path").toString();
        music.filePath = query.value("filePath").toString();
        music.fileType = query.value("fileType").toString();
        music.lyric = query.value("lyric").toString();
        music.img = query.value("img").toString();
        music.isLove = query.value("isLove").toBool();

        musicList.append(music);
    }

    return musicList;
}

// 根据ID获取音乐信息
MusicInfoData MusicInfoManager::getMusicById(const QString& id) {
    MusicInfoData music;
    QSqlQuery query;
    query.prepare("SELECT * FROM Music WHERE id = :id");
    query.bindValue(":id", id);

    if (query.exec()) {
        if (query.next()) {
            music.id = query.value("id").toString();
            music.isLocal = query.value("isLocal").toBool();
            music.musicName = query.value("musicName").toString();
            music.albumName = query.value("albumName").toString();
            music.duration = query.value("duration").toInt();
            music.path = query.value("path").toString();
            music.filePath = query.value("filePath").toString();
            music.fileType = query.value("fileType").toString();
            music.lyric = query.value("lyric").toString();
            music.img = query.value("img").toString();
            music.isLove = query.value("isLove").toBool();
        }
    }
    else {
        qDebug() << "通过ID查询音乐失败:" << query.lastError().text();
    }

    return music;
}

// 设置音乐的喜爱状态
bool MusicInfoManager::setLoveStatus(const QString& id, bool isLove) {
    QSqlQuery query;
    query.prepare("UPDATE Music SET isLove = :isLove WHERE id = :id");
    query.bindValue(":isLove", isLove);
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "设置喜爱状态失败:" << query.lastError().text();
        return false;
    }
    else {
        qDebug() << "设置喜爱状态成功:" << id;
    }

    return true;
}

// 检查音乐是否存在
bool MusicInfoManager::isMusicExists(const QString& id) {
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM Music WHERE id = :id");
    query.bindValue(":id", id);

    if (query.exec()) {
        if (query.next() && query.value(0).toInt() > 0) {
            return true;
        }
    }
    else {
        qDebug() << "通过ID查询音乐失败:" << query.lastError().text();
    }

    return false;
}
