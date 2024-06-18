#pragma once
#pragma execution_character_set("utf-8")

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "MiddleWidgetLeftList.h"
#include "MiddleWidgetRightStack.h"

// MiddleWidget �ඨ�壬�м����
class MiddleWidget : public QWidget
{
	Q_OBJECT
public:
	// ���캯��
	explicit MiddleWidget(QWidget* parent = nullptr);

	MiddleWidgetLeftList* m_List;  // ����б����
	MiddleWidgetRightStack* m_Stack;  // �Ҳ��ջ���
	QPushButton* m_loadButton;
private:
	QHBoxLayout* m_hmainLayout;  // ��ˮƽ����
	QVBoxLayout* m_vListLayout;

signals:
	void onLoadButtonClicked();

public slots:
	
	void loadButtonResult(bool ok);

};
