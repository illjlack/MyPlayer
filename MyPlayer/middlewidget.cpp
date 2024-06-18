#include "MiddleWidget.h"
#include <QMessageBox>

// MiddleWidget 类的构造函数
MiddleWidget::MiddleWidget(QWidget* parent) : QWidget(parent) {
    m_hmainLayout = new QHBoxLayout(this);  // 主水平布局
    m_vListLayout = new QVBoxLayout(this);  // 列表垂直布局

    m_List = new MiddleWidgetLeftList(this);  // 左侧列表组件

    // 将左侧列表组件和播放歌曲信息组件添加到垂直布局中
    m_vListLayout->addWidget(m_List);
    m_vListLayout->setSpacing(0);  // 设置垂直布局组件之间的间距为0

    // 添加加载按钮
    m_loadButton = new QPushButton("添加本地歌曲", this);
    m_vListLayout->addWidget(m_loadButton);

    // 连接加载按钮的点击事件
    connect(m_loadButton, &QPushButton::clicked, this, &MiddleWidget::onLoadButtonClicked);

    m_Stack = new MiddleWidgetRightStack(this);  // 右侧堆栈组件

    // 连接左侧列表按钮和堆栈组件的槽函数
    for (int i = 0; i < m_List->m_btnVec.size(); ++i) {
        connect(m_List->m_btnVec.at(i), &QPushButton::clicked, m_Stack, [=]() { m_Stack->setCurrentIndex(i); });
    }

    // 将垂直布局和堆栈组件添加到主水平布局中
    m_hmainLayout->addLayout(m_vListLayout);
    m_hmainLayout->addWidget(m_Stack);
    m_hmainLayout->setMargin(0);
    m_hmainLayout->setSpacing(0);
}

// 加载按钮的槽函数
void MiddleWidget::loadButtonResult(bool ok) {
    
    if (ok) {
        QMessageBox::information(this, "成功", "本地歌曲加载成功！");
    }
    else {
        QMessageBox::warning(this, "失败", "文件类型不支持！");
    }
}
