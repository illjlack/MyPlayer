#pragma once
#pragma execution_character_set("utf-8")

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "MiddleWidgetLeftList.h"
#include "MiddleWidgetRightStack.h"

// MiddleWidget 类定义，中间组件
class MiddleWidget : public QWidget
{
	Q_OBJECT
public:
	// 构造函数
	explicit MiddleWidget(QWidget* parent = nullptr);

	MiddleWidgetLeftList* m_List;  // 左侧列表组件
	MiddleWidgetRightStack* m_Stack;  // 右侧堆栈组件
	QPushButton* m_loadButton;
private:
	QHBoxLayout* m_hmainLayout;  // 主水平布局
	QVBoxLayout* m_vListLayout;

signals:
	void onLoadButtonClicked();

public slots:
	
	void loadButtonResult(bool ok);

};
