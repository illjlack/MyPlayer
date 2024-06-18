#pragma once
#pragma execution_character_set("utf-8")

#include <QLabel>
#include <QMouseEvent>

class ClickLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ClickLabel(QWidget* parent = nullptr) : QLabel(parent)
    {
    }
    ~ClickLabel() {};

signals:
    // 点击信号
    void clicked();

protected:
    // 鼠标点击事件处理
    void mousePressEvent(QMouseEvent* event) override {
        // 如果是左键点击，则发射 clicked 信号
        if (event->button() == Qt::LeftButton)
            emit clicked();

        // 调用父类的鼠标点击事件处理函数
        QLabel::mousePressEvent(event);
    }
};
