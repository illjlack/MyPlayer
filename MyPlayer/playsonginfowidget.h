#pragma once
#pragma execution_character_set("utf-8")

#include <QFrame>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPixmap>

#include "ClickLabel.h"
#include "MusicInfoData.h"
#include "MusicInfoManager.h"
#include "CacheDownloader.h"

class PlaySongInfoWidget : public QFrame
{
    Q_OBJECT
public:
    explicit PlaySongInfoWidget(QWidget* parent = nullptr);
    ~PlaySongInfoWidget();

    QPushButton* songNameBtn;  // 歌曲名称按钮
    QPushButton* singleNameBtn;  // 歌手名称按钮
    QPushButton* loveBtn;  // 收藏按钮
    ClickLabel* cdLabel;  // CD 标签

    void setSongText(const QString& song, const QString& single);

public slots:
    void updateSongInfo(const MusicInfoData& songInfo);  // 更新歌曲信息槽函数

signals:
    void toggleSongDetails(bool isExpanded);
    void nolove(const MusicInfoData& songInfo);
    void loved(const MusicInfoData& songInfo);

public slots://暂时让BottomWidget访问下
    void isToggleSongDetails(); // CD 标签被点击，切换歌曲详细页
private slots:
    void onLoveBtnClicked();  // 收藏按钮点击槽函数
    
    void onImageDownloaded(const QString& filePath, const QString& id);  // 处理图片下载完成的槽函数

private:
    void initWidget();
    void downloadImage(const QString& imageUrl, const QString& id);  // 下载图片
    MusicInfoData currentMusic;//当前音乐
    QHBoxLayout* h_mainLayout;  // 主水平布局
    QVBoxLayout* v_initLayout1;  // 初始垂直布局1
    QVBoxLayout* v_initLayout2;  // 初始垂直布局2

    bool isLoved;  // 是否喜爱
    bool isExpanded;  // 是否展开详细页

    CacheDownloader* cacheDownloader;  // 缓存下载器
    MusicInfoManager* musicInfoManager;  // 数据库
};
