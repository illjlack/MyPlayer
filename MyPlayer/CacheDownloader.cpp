#include "CacheDownloader.h"

// ���캯��
CacheDownloader::CacheDownloader(QObject* parent)
    : QObject(parent)
{
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, &QNetworkAccessManager::finished, this, &CacheDownloader::onDownloadFinished);
}

// ��������
CacheDownloader::~CacheDownloader()
{
    delete networkManager;
}

// ����ָ��URL���ļ�
void CacheDownloader::download(const QString& url, const QString& id)
{
    qDebug() << "CacheDownloader: ��ʼ���� - URL:" << url << "ID:" << id;
    QString cacheFilePath = generateCacheFilePath(url, id);

    // ��黺���ļ��Ƿ����
    if (QFile::exists(cacheFilePath)) {
        qDebug() << "CacheDownloader: �ļ��Ѵ��ڣ�ֱ��ʹ�û��� - ·��:" << cacheFilePath;
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


// �����������
void CacheDownloader::onDownloadFinished(QNetworkReply* reply)
{
    QString id = reply->property("id").toString();
    QString url = reply->property("url").toString();

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        QString cacheFilePath = generateCacheFilePath(url, id);

        saveToCache(cacheFilePath, data);
        emit downloadFinished(cacheFilePath, id);

        qDebug() << "CacheDownloader: ������� - ID:" << id << "����·��:" << cacheFilePath;
    }
    else {
        qWarning() << "CacheDownloader: ����ʧ�� - URL:" << url << "����:" << reply->errorString();
    }

    reply->deleteLater();
}

// �����ݱ��浽����
void CacheDownloader::saveToCache(const QString& filePath, const QByteArray& data)
{
    QFile cacheFile(filePath);
    if (cacheFile.open(QIODevice::WriteOnly)) {
        cacheFile.write(data);
        cacheFile.close();
        qDebug() << "CacheDownloader: ���ݱ��浽���� - ·��:" << filePath;
    }
    else {
        qWarning() << "CacheDownloader: �޷�д�뻺���ļ� - ·��:" << filePath;
    }
}

// �ӻ����м�������
QByteArray CacheDownloader::loadFromCache(const QString& url, const QString& id)
{
    QString cacheFilePath = generateCacheFilePath(url, id);
    QFile cacheFile(cacheFilePath);
    if (cacheFile.exists() && cacheFile.open(QIODevice::ReadOnly)) {
        QByteArray data = cacheFile.readAll();
        cacheFile.close();
        qDebug() << "CacheDownloader: �ӻ���������� - ·��:" << cacheFilePath;
        return data;
    }
    qWarning() << "CacheDownloader: �����ļ������ڻ��޷���ȡ - ·��:" << cacheFilePath;
    return QByteArray();
}

// ���ɻ����ļ�·��
QString CacheDownloader::generateCacheFilePath(const QString& url, const QString& id) const
{
    QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    if (!QDir(cacheDir).exists()) {
        QDir().mkpath(cacheDir);
    }
    QString fileName = QFileInfo(url).fileName();
    return cacheDir + "/" + id + "_" + fileName;
}
