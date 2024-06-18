#include "MiddleWidget.h"
#include <QMessageBox>

// MiddleWidget ��Ĺ��캯��
MiddleWidget::MiddleWidget(QWidget* parent) : QWidget(parent) {
    m_hmainLayout = new QHBoxLayout(this);  // ��ˮƽ����
    m_vListLayout = new QVBoxLayout(this);  // �б�ֱ����

    m_List = new MiddleWidgetLeftList(this);  // ����б����

    // ������б�����Ͳ��Ÿ�����Ϣ�����ӵ���ֱ������
    m_vListLayout->addWidget(m_List);
    m_vListLayout->setSpacing(0);  // ���ô�ֱ�������֮��ļ��Ϊ0

    // ��Ӽ��ذ�ť
    m_loadButton = new QPushButton("��ӱ��ظ���", this);
    m_vListLayout->addWidget(m_loadButton);

    // ���Ӽ��ذ�ť�ĵ���¼�
    connect(m_loadButton, &QPushButton::clicked, this, &MiddleWidget::onLoadButtonClicked);

    m_Stack = new MiddleWidgetRightStack(this);  // �Ҳ��ջ���

    // ��������б�ť�Ͷ�ջ����Ĳۺ���
    for (int i = 0; i < m_List->m_btnVec.size(); ++i) {
        connect(m_List->m_btnVec.at(i), &QPushButton::clicked, m_Stack, [=]() { m_Stack->setCurrentIndex(i); });
    }

    // ����ֱ���ֺͶ�ջ�����ӵ���ˮƽ������
    m_hmainLayout->addLayout(m_vListLayout);
    m_hmainLayout->addWidget(m_Stack);
    m_hmainLayout->setMargin(0);
    m_hmainLayout->setSpacing(0);
}

// ���ذ�ť�Ĳۺ���
void MiddleWidget::loadButtonResult(bool ok) {
    
    if (ok) {
        QMessageBox::information(this, "�ɹ�", "���ظ������سɹ���");
    }
    else {
        QMessageBox::warning(this, "ʧ��", "�ļ����Ͳ�֧�֣�");
    }
}
