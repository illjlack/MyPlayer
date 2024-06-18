#pragma once
#pragma execution_character_set("utf-8")

#include <QFrame>
#include <QPushButton>
#include <QVBoxLayout>
#include <QButtonGroup>

class MiddleWidgetLeftList : public QFrame
{
    Q_OBJECT
public:
    explicit MiddleWidgetLeftList(QWidget *parent = nullptr);

    QPushButton *localMusicBtn, *onlineMusicBtn, *favoriteBtn;  // 本地音乐、网络音乐、喜爱按钮
    QVector<QPushButton*> m_btnVec;  // 按钮向量

private:
    void initTopWidget();  // 初始化顶部部件
    void initBtnVec();  // 初始化按钮向量
    void setButtonStyle(QPushButton* button);  // 设置按钮样式

    QButtonGroup *mutexBtnGroup;  // 按钮组，用于互斥选择
};

