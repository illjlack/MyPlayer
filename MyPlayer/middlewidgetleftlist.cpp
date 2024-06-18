#include "MiddleWidgetLeftList.h"

// ���캯��
MiddleWidgetLeftList::MiddleWidgetLeftList(QWidget* parent) : QFrame(parent)
{
    setFixedWidth(200);  // ���ù̶����

    // ʹ��ί�й�������ʼ������
    initTopWidget();  // ��ʼ����������
    initBtnVec();  // ��ʼ����ť����

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(localMusicBtn);
    mainLayout->addSpacing(20); // ���Ӽ��
    mainLayout->addWidget(onlineMusicBtn);
    mainLayout->addSpacing(20); // ���Ӽ��
    mainLayout->addWidget(favoriteBtn);
    mainLayout->addStretch();
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    setLayout(mainLayout);  // ����������
}

// ��ʼ����������
void MiddleWidgetLeftList::initTopWidget()
{
    localMusicBtn = new QPushButton("��������", this);
    onlineMusicBtn = new QPushButton("��������", this);
    favoriteBtn = new QPushButton("ϲ��", this);

    // ʹ�� setButtonStyle ����������ʽ
    setButtonStyle(localMusicBtn);
    setButtonStyle(onlineMusicBtn);
    setButtonStyle(favoriteBtn);
}

// ��ʼ����ť����
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

// ���ð�ť��ʽ
void MiddleWidgetLeftList::setButtonStyle(QPushButton* button)
{
    button->setCheckable(true);
    button->setStyleSheet(
        "QPushButton {"
        "    background: transparent;"
        "    border: none;"
        "    font: 18px;"  // ��������
        "    text-align: left;"  // �����
        "    padding-left: 20px;"  // ������߾�
        "    height: 40px;"  // �̶��߶�
        "}"
        "QPushButton:checked {"
        "    font-weight: bold;"
        "    color: #1DB954;"  // ����ѡ����ɫ
        "}"
        "QPushButton:hover {"
        "    background-color: #f0f0f0;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #e0e0e0;"
        "}"
    );
}
