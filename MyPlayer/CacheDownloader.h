#pragma once
#pragma execution_character_set("utf-8")

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

class CacheDownloader : public QObject
{
    Q_OBJECT

public:
    explicit CacheDownloader(QObject* parent = nullptr);
    ~CacheDownloader();

    void download(const QString& url, const QString& id); // ����ָ��URL���ļ�
    QByteArray loadFromCache(const QString& url, const QString& id); // �ӻ����м�������
    QString generateCacheFilePath(const QString& url, const QString& id) const; // ���ɻ����ļ�·��

signals:
    void downloadFinished(const QString& localPath, const QString& id); // ��������ź�

private slots:
    void onDownloadFinished(QNetworkReply* reply); // �����������

private:
    void saveToCache(const QString& filePath, const QByteArray& data); // �����ݱ��浽����

    QNetworkAccessManager* networkManager; // ������ʹ�����
};
