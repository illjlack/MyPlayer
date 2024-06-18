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

// ��ʼ�����ݿ�
void MusicInfoManager::initDatabase() {
    QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + QDir::separator() + "music_info.db";

    // �������ݿ��ļ����ڵ��ļ���
    QDir dbDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    if (!dbDir.exists()) {
        if (!dbDir.mkpath(".")) {
            qDebug() << "�������ݿ��ļ���ʧ�ܡ�";
            showError("�������ݿ��ļ���ʧ�ܣ�����������Ϣ��ϲ���޷�����");
            return;
        }
    }

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);

    if (!db.open()) {
        // ������ݿ��ļ��Ƿ���ڲ��ɶ�д
        QFile dbFile(dbPath);
        if (!dbFile.exists() || !dbFile.isReadable() || !dbFile.isWritable()) {
            qDebug() << "���ݿ��ļ������ã�����ʹ���ڴ����ݿ⡣";
            showError("���ݿ����\nԭ��:" + db.lastError().text() + "\nδ���, ĳЩ���Բ��ܴ���(�����ǵ�һ��)��������Ȩ������, ���ڳ����ڴ��������ݿ�\n(���ݾ����������Գɹ��������ݿ�)");
            db.setDatabaseName(":memory:");
        }
    }

    if (!db.open()) {
        qDebug() << "���ݿ��ʧ��:" << db.lastError().text();
        showError("�ڴ����ݿ�Ҳû�ɹ������ݾ����������Դ����������ݿ�" + db.lastError().text());
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
        "filePath TEXT," // ֧�ָ����ļ�����
        "fileType TEXT," // �ļ������ֶ�
        "lyric TEXT,"
        "img TEXT,"
        "isLove INTEGER)" // ϲ���ֶ�
    );

    if (!query.exec()) {
        qDebug() << "������ʧ��:\nԭ��" << query.lastError().text();
        showError("���ݿ����ʧ�ܣ�����������Ϣ��ϲ���޷�����");
    }
}


void MusicInfoManager::showError(const QString& message) {
    QMessageBox::critical(nullptr, "���ݿ����", message
    );
}

// ��ӻ����������Ϣ
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
        //query.bindValue(":isLove", music.isLove); // ϲ�������ڱ��أ�����Զ�˸���
        query.bindValue(":id", music.id);

        if (!query.exec()) {
            qDebug() << "��������ʧ��:" << query.lastError().text();
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
            qDebug() << "�������ʧ��:" << query.lastError().text();
            return false;
        }
    }

    return true;
}

// ��ȡ����������Ϣ
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

// ��ȡ����ϲ����������Ϣ
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

// ����ID��ȡ������Ϣ
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
        qDebug() << "ͨ��ID��ѯ����ʧ��:" << query.lastError().text();
    }

    return music;
}

// �������ֵ�ϲ��״̬
bool MusicInfoManager::setLoveStatus(const QString& id, bool isLove) {
    QSqlQuery query;
    query.prepare("UPDATE Music SET isLove = :isLove WHERE id = :id");
    query.bindValue(":isLove", isLove);
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "����ϲ��״̬ʧ��:" << query.lastError().text();
        return false;
    }
    else {
        qDebug() << "����ϲ��״̬�ɹ�:" << id;
    }

    return true;
}

// ��������Ƿ����
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
        qDebug() << "ͨ��ID��ѯ����ʧ��:" << query.lastError().text();
    }

    return false;
}
