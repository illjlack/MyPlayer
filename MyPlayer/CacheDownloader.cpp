#include "CacheDownloader.h"

// 构造函数
CacheDownloader::CacheDownloader(QObject* parent)
    : QObject(parent)
{
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, &QNetworkAccessManager::finished, this, &CacheDownloader::onDownloadFinished);
}

// 析构函数
CacheDownloader::~CacheDownloader()
{
    delete networkManager;
}

// 下载指定URL的文件
void CacheDownloader::download(const QString& url, const QString& id)
{
    qDebug() << "CacheDownloader: 开始下载 - URL:" << url << "ID:" << id;
    QString cacheFilePath = generateCacheFilePath(url, id);

    // 检查缓存文件是否存在
    if (QFile::exists(cacheFilePath)) {
        qDebug() << "CacheDownloader: 文件已存在，直接使用缓存 - 路径:" << cacheFilePath;
        emit downloadFinished(cacheFilePath, id);
    }
    else {
        QNetworkRequest request;
        request.setUrl(QUrl(url));
        QNetworkReply* reply = networkManager->get(request);
        reply->setProperty("id", id);
        reply->setProperty("url", url);
    }
}


// 处理下载完成
void CacheDownloader::onDownloadFinished(QNetworkReply* reply)
{
    QString id = reply->property("id").toString();
    QString url = reply->property("url").toString();

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        QString cacheFilePath = generateCacheFilePath(url, id);

        saveToCache(cacheFilePath, data);
        emit downloadFinished(cacheFilePath, id);

        qDebug() << "CacheDownloader: 下载完成 - ID:" << id << "缓存路径:" << cacheFilePath;
    }
    else {
        qWarning() << "CacheDownloader: 下载失败 - URL:" << url << "错误:" << reply->errorString();
    }

    reply->deleteLater();
}

// 将数据保存到缓存
void CacheDownloader::saveToCache(const QString& filePath, const QByteArray& data)
{
    QFile cacheFile(filePath);
    if (cacheFile.open(QIODevice::WriteOnly)) {
        cacheFile.write(data);
        cacheFile.close();
        qDebug() << "CacheDownloader: 数据保存到缓存 - 路径:" << filePath;
    }
    else {
        qWarning() << "CacheDownloader: 无法写入缓存文件 - 路径:" << filePath;
    }
}

// 从缓存中加载数据
QByteArray CacheDownloader::loadFromCache(const QString& url, const QString& id)
{
    QString cacheFilePath = generateCacheFilePath(url, id);
    QFile cacheFile(cacheFilePath);
    if (cacheFile.exists() && cacheFile.open(QIODevice::ReadOnly)) {
        QByteArray data = cacheFile.readAll();
        cacheFile.close();
        qDebug() << "CacheDownloader: 从缓存加载数据 - 路径:" << cacheFilePath;
        return data;
    }
    qWarning() << "CacheDownloader: 缓存文件不存在或无法读取 - 路径:" << cacheFilePath;
    return QByteArray();
}

// 生成缓存文件路径
QString CacheDownloader::generateCacheFilePath(const QString& url, const QString& id) const
{
    QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    if (!QDir(cacheDir).exists()) {
        QDir().mkpath(cacheDir);
    }
    QString fileName = QFileInfo(url).fileName();
    return cacheDir + "/" + id + "_" + fileName;
}
