#pragma once
#pragma execution_character_set("utf-8")

#include <QObject>
#include <QMediaPlayer>
#include <QBuffer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QCache>
#include "MusicInfoData.h"

#define BUFFER_SIZE 1024 * 1024 // 1 MB
#define CACHE_CAPACITY 100      // 缓存容量

// MusicPlayer类负责管理缓存和本地播放
class MusicPlayer : public QObject
{
    Q_OBJECT

public:
    MusicPlayer(QObject* parent = nullptr);
    ~MusicPlayer();

    void openUrl(const QString& url); // 打开在线音乐文件
    void play(); // 播放音乐
    void pause(); // 暂停播放
    void stop(); // 停止播放
    void setVolume(int volume); // 设置音量
    void setPosition(qint64 position); // 设置播放进度
    void setCurrentMusic(const MusicInfoData& music); // 设置当前播放音乐

public slots:
    //void positionChanged(qint64 position); // 播放位置变化信号
    //void durationChanged(qint64 duration); // 播放时长变化信号

private slots:
    void onNetworkReply(QNetworkReply* reply); // 处理网络回复
    void onBufferStatusChanged(int percent); // 处理缓冲状态改变
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status); // 处理媒体状态改变
    void onHeadRequestFinished(QNetworkReply* reply); // 处理HEAD请求的响应

private:
    void sendHeadRequest(const QString& url); // 发送HEAD请求
    void downloadRange(qint64 start); // 下载指定范围的数据     
    void saveToCache(const QString& url, qint64 position, const QByteArray& data); // 将数据保存到缓存
    QByteArray loadFromCache(const QString& url, qint64 position); // 从缓存中加载数据
    inline QString generateCacheKey(const QString& url, qint64 position) const; // 生成缓存键
    inline QString generateRequestId() const; // 生成唯一请求标识符
    void appendToBuffer(QByteArray&& data); // 将数据追加到缓冲区
    void useCachedData(QByteArray&& data); // 使用缓存数据
    void prepareBuffer(); // 准备缓冲区
    qint64 parseStartFromRangeHeader(const QByteArray& contentRange) const; // 解析Content-Range头部以获取起始位置
    inline QString generateBufferSetKey(qint64 position) const; // 生成缓冲区集合键

    QMediaPlayer* player; // 媒体播放器
    QNetworkAccessManager* networkManager; // 网络访问管理器
    QCache<QString, QByteArray> cache; // 缓存
    QBuffer* currentBuffer; // 当前缓冲区
    QString currentUrl; // 当前URL
    QString currentRequestId; // 当前请求ID
    qint64 bufferPosition; // 缓冲位置
    qint64 fileSize; // 文件的总长度
    qint64 totalDuration; // 音频的总时长
    QSet<QString> bufferingSet; // 缓冲区集合
    MusicInfoData currentMusic; // 当前播放的音乐信息
};
