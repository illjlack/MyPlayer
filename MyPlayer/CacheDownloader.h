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

    void download(const QString& url, const QString& id); // 下载指定URL的文件
    QByteArray loadFromCache(const QString& url, const QString& id); // 从缓存中加载数据
    QString generateCacheFilePath(const QString& url, const QString& id) const; // 生成缓存文件路径

signals:
    void downloadFinished(const QString& localPath, const QString& id); // 下载完成信号

private slots:
    void onDownloadFinished(QNetworkReply* reply); // 处理下载完成

private:
    void saveToCache(const QString& filePath, const QByteArray& data); // 将数据保存到缓存

    QNetworkAccessManager* networkManager; // 网络访问管理器
};
