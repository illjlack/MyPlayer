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

    QPushButton *localMusicBtn, *onlineMusicBtn, *favoriteBtn;  // �������֡��������֡�ϲ����ť
    QVector<QPushButton*> m_btnVec;  // ��ť����

private:
    void initTopWidget();  // ��ʼ����������
    void initBtnVec();  // ��ʼ����ť����
    void setButtonStyle(QPushButton* button);  // ���ð�ť��ʽ

    QButtonGroup *mutexBtnGroup;  // ��ť�飬���ڻ���ѡ��
};

