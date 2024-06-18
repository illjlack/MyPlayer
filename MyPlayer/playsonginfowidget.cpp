#include "PlaySongInfoWidget.h"
#include <QMouseEvent>
#include <QEvent>
#include <QtCore/QStandardPaths>
#include <QtCore/QFileInfo>
#include "MusicInfoManager.h"

// 构造函数
PlaySongInfoWidget::PlaySongInfoWidget(QWidget* parent)
    : QFrame(parent), isLoved(false), isExpanded(false), cacheDownloader(new CacheDownloader(this))
{
    setFixedHeight(80);  // 设置固定高度
    setFixedWidth(350);  // 设置固定宽度

    // 设置样式，背景色和边框
    setStyleSheet("QFrame{background:rgb(246,246,248);border-color:rgb(225,225,225);border-width: 0px 1px 0px 1px;border-style: solid;}");

    initWidget();  // 初始化部件

    // 连接 loveBtn 的点击信号到槽函数
    connect(loveBtn, &QPushButton::clicked, this, &PlaySongInfoWidget::onLoveBtnClicked);
    // 连接 cdLabel 的点击信号到槽函数
    connect(cdLabel, &ClickLabel::clicked, this, &PlaySongInfoWidget::isToggleSongDetails);

    // 连接缓存下载器的下载完成信号到槽函数
    connect(cacheDownloader, &CacheDownloader::downloadFinished, this, &PlaySongInfoWidget::onImageDownloaded);
}

// 析构函数
PlaySongInfoWidget::~PlaySongInfoWidget()
{
}

// 设置歌曲和歌手文本
void PlaySongInfoWidget::setSongText(const QString& song, const QString& single)
{
    QString show_song = "";
    QString show_single = "";

    // 如果歌曲名长度大于10，截断并添加省略号
    if (song.length() > 10) {
        show_song = song.mid(0, 9);
        show_song.append("...");
        songNameBtn->setText(show_song);
        songNameBtn->setToolTip(song);  // 设置提示为完整的歌曲名
    }
    else {
        songNameBtn->setText(song);
        songNameBtn->setToolTip(song);
    }

    // 如果歌手名长度大于10，截断并添加省略号
    if (single.length() > 10) {
        show_single = single.mid(0, 9);
        show_single.append("...");
        singleNameBtn->setText(show_single);
        singleNameBtn->setToolTip(single);  // 设置提示为完整的歌手名
    }
    else {
        singleNameBtn->setText(single);
        singleNameBtn->setToolTip(single);
    }
}

// 初始化部件
void PlaySongInfoWidget::initWidget()
{
    h_mainLayout = new QHBoxLayout(this);  // 创建主水平布局
    v_initLayout1 = new QVBoxLayout;  // 创建垂直布局1
    v_initLayout2 = new QVBoxLayout;  // 创建垂直布局2

    cdLabel = new ClickLabel(this);  // 创建 CD 标签
    cdLabel->setFixedSize(60, 60);  // 设置固定大小
    cdLabel->setToolTip("展开歌曲详细页");
    cdLabel->setCursor(Qt::PointingHandCursor);  // 设置光标为手型
    cdLabel->setStyleSheet("QLabel{border-image:url(:/images/SongInfo/disk.png)}");  // 设置样式

    songNameBtn = new QPushButton(this);  // 创建歌曲名称按钮
    songNameBtn->setFixedSize(150, 20);  // 设置固定大小
    songNameBtn->setCursor(Qt::PointingHandCursor);  // 设置光标为手型
    songNameBtn->setText("未知");  // 设置初始文本
    songNameBtn->adjustSize();  // 调整大小
    songNameBtn->setStyleSheet("QPushButton{background:transparent;border:none;font:13px}");  // 设置样式

    singleNameBtn = new QPushButton(this);  // 创建歌手名称按钮
    singleNameBtn->setFixedSize(150, 20);  // 设置固定大小
    singleNameBtn->setCursor(Qt::PointingHandCursor);  // 设置光标为手型
    singleNameBtn->setText("未知");  // 设置初始文本
    singleNameBtn->adjustSize();  // 调整大小
    singleNameBtn->setStyleSheet("QPushButton{background:transparent;border:none;font:13px}");  // 设置样式

    // 将按钮添加到垂直布局1中
    v_initLayout1->addWidget(songNameBtn);
    v_initLayout1->addWidget(singleNameBtn);
    v_initLayout1->setMargin(0);  // 设置边距为0

    loveBtn = new QPushButton(this);  // 创建收藏按钮
    loveBtn->setCursor(Qt::PointingHandCursor);  // 设置光标为手型
    loveBtn->setFixedSize(25, 25);  // 设置固定大小
    loveBtn->setStyleSheet("QPushButton{border-image:url(:/images/SongInfo/love.png)}");  // 设置样式

    // 将按钮添加到垂直布局2中
    v_initLayout2->addWidget(loveBtn);

    // 将所有布局添加到主水平布局中
    h_mainLayout->addWidget(cdLabel);
    h_mainLayout->addLayout(v_initLayout1);
    h_mainLayout->addLayout(v_initLayout2);
    h_mainLayout->setMargin(0);  // 设置主水平布局的边距为0
}

// 更新歌曲信息槽函数
void PlaySongInfoWidget::updateSongInfo(const MusicInfoData& songInfo)
{
    currentMusic = songInfo;
    setSongText(songInfo.musicName, songInfo.albumName);

    // 根据是否喜欢设置 loveBtn 的图标
    isLoved = MusicInfoManager::instance().getMusicById(songInfo.id).isLove; // 与数据库一致

    if (isLoved) {
        loveBtn->setStyleSheet("QPushButton{border-image:url(:/images/SongInfo/love1.png)}");
    }
    else {
        loveBtn->setStyleSheet("QPushButton{border-image:url(:/images/SongInfo/love.png)}");
    }

    // 判断图片路径是否为本地路径，如果是本地路径，直接使用，否则下载图片
    if (songInfo.isLocal) {
        qDebug() << "PlaySongInfoWidget:图片路径在本地";
        if (QFileInfo::exists(songInfo.img)) {
            qDebug() << "PlaySongInfoWidget:使用图片=" << songInfo.albumName;
            cdLabel->setPixmap(QPixmap(songInfo.img).scaled(cdLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
        else {
            qDebug() << "PlaySongInfoWidget:本地图片不存在，使用默认图片:" << songInfo.id;
            cdLabel->setPixmap(QPixmap(":/images/default_album_art.png").scaled(cdLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }
    else {
        downloadImage(songInfo.img, songInfo.id);
    }
    loveBtn->repaint();  // 强制重绘
    cdLabel->repaint();  // 强制重绘
}

// 下载图片
void PlaySongInfoWidget::downloadImage(const QString& imageUrl, const QString& id)
{
    cacheDownloader->download(imageUrl, id);
}

// 处理图片下载完成的槽函数
void PlaySongInfoWidget::onImageDownloaded(const QString& filePath, const QString& id)
{
    Q_UNUSED(id);  // 如果 id 在这个函数中没有使用，可以忽略它

    QPixmap pixmap(filePath);
    if (!pixmap.isNull()) {
        cdLabel->setPixmap(pixmap.scaled(cdLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    else {
        qDebug() << "加载缓存的图片失败，使用默认图片";
        cdLabel->setPixmap(QPixmap(":/images/default_album_art.png").scaled(cdLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    cdLabel->repaint();  // 强制立即重绘
}

// 收藏按钮点击槽函数
void PlaySongInfoWidget::onLoveBtnClicked()
{
    // 切换 isLoved 状态
    isLoved = !isLoved;

    // 根据 isLoved 状态切换按钮图标
    if (isLoved) {
        loveBtn->setStyleSheet("QPushButton{border-image:url(:/images/SongInfo/love1.png)}");
        emit loved(currentMusic);
    }
    else {
        loveBtn->setStyleSheet("QPushButton{border-image:url(:/images/SongInfo/love.png)}");
        emit nolove(currentMusic);
    }
    loveBtn->repaint();  // 强制重绘
}

// 切换歌曲详细页
void PlaySongInfoWidget::isToggleSongDetails()
{
    isExpanded = !isExpanded;

    if (isExpanded) {
        cdLabel->setToolTip("收起歌曲详细页");
    }
    else {
        cdLabel->setToolTip("展开歌曲详细页");
    }

    emit toggleSongDetails(isExpanded);

    cdLabel->repaint();  // 强制重绘
}
