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

    QPushButton* songNameBtn;  // �������ư�ť
    QPushButton* singleNameBtn;  // �������ư�ť
    QPushButton* loveBtn;  // �ղذ�ť
    ClickLabel* cdLabel;  // CD ��ǩ

    void setSongText(const QString& song, const QString& single);

public slots:
    void updateSongInfo(const MusicInfoData& songInfo);  // ���¸�����Ϣ�ۺ���

signals:
    void toggleSongDetails(bool isExpanded);
    void nolove(const MusicInfoData& songInfo);
    void loved(const MusicInfoData& songInfo);

public slots://��ʱ��BottomWidget������
    void isToggleSongDetails(); // CD ��ǩ��������л�������ϸҳ
private slots:
    void onLoveBtnClicked();  // �ղذ�ť����ۺ���
    
    void onImageDownloaded(const QString& filePath, const QString& id);  // ����ͼƬ������ɵĲۺ���

private:
    void initWidget();
    void downloadImage(const QString& imageUrl, const QString& id);  // ����ͼƬ
    MusicInfoData currentMusic;//��ǰ����
    QHBoxLayout* h_mainLayout;  // ��ˮƽ����
    QVBoxLayout* v_initLayout1;  // ��ʼ��ֱ����1
    QVBoxLayout* v_initLayout2;  // ��ʼ��ֱ����2

    bool isLoved;  // �Ƿ�ϲ��
    bool isExpanded;  // �Ƿ�չ����ϸҳ

    CacheDownloader* cacheDownloader;  // ����������
    MusicInfoManager* musicInfoManager;  // ���ݿ�
};
