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
    // ����ź�
    void clicked();

protected:
    // ������¼�����
    void mousePressEvent(QMouseEvent* event) override {
        // ����������������� clicked �ź�
        if (event->button() == Qt::LeftButton)
            emit clicked();

        // ���ø����������¼�������
        QLabel::mousePressEvent(event);
    }
};
