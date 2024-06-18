#include "MiddleWidgetLeftList.h"

// 构造函数
MiddleWidgetLeftList::MiddleWidgetLeftList(QWidget* parent) : QFrame(parent)
{
    setFixedWidth(200);  // 设置固定宽度

    // 使用委托构造来初始化部件
    initTopWidget();  // 初始化顶部部件
    initBtnVec();  // 初始化按钮向量

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(localMusicBtn);
    mainLayout->addSpacing(20); // 增加间隔
    mainLayout->addWidget(onlineMusicBtn);
    mainLayout->addSpacing(20); // 增加间隔
    mainLayout->addWidget(favoriteBtn);
    mainLayout->addStretch();
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    setLayout(mainLayout);  // 设置主布局
}

// 初始化顶部部件
void MiddleWidgetLeftList::initTopWidget()
{
    localMusicBtn = new QPushButton("本地音乐", this);
    onlineMusicBtn = new QPushButton("网络音乐", this);
    favoriteBtn = new QPushButton("喜爱", this);

    // 使用 setButtonStyle 函数设置样式
    setButtonStyle(localMusicBtn);
    setButtonStyle(onlineMusicBtn);
    setButtonStyle(favoriteBtn);
}

// 初始化按钮向量
void MiddleWidgetLeftList::initBtnVec()
{
    m_btnVec << localMusicBtn << onlineMusicBtn << favoriteBtn;

    mutexBtnGroup = new QButtonGroup(this);
    mutexBtnGroup->setExclusive(true);

    for (auto btn : m_btnVec)
    {
        mutexBtnGroup->addButton(btn);
    }
}

// 设置按钮样式
void MiddleWidgetLeftList::setButtonStyle(QPushButton* button)
{
    button->setCheckable(true);
    button->setStyleSheet(
        "QPushButton {"
        "    background: transparent;"
        "    border: none;"
        "    font: 18px;"  // 增大字体
        "    text-align: left;"  // 左对齐
        "    padding-left: 20px;"  // 增加左边距
        "    height: 40px;"  // 固定高度
        "}"
        "QPushButton:checked {"
        "    font-weight: bold;"
        "    color: #1DB954;"  // 更改选中颜色
        "}"
        "QPushButton:hover {"
        "    background-color: #f0f0f0;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #e0e0e0;"
        "}"
    );
}
