#pragma once
#pragma execution_character_set("utf-8")

#include <QSlider>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOptionSlider>

// ����һ���̳��� QSlider ���� ClickableSlider
class ClickableSlider : public QSlider {
    Q_OBJECT

public:
    // ���캯�������ջ���ķ���͸��ؼ�ָ��
    ClickableSlider(Qt::Orientation orientation, QWidget* parent = nullptr)
        : QSlider(orientation, parent), icon(":/images/BottomPlay/slider_icon.png") {
        // ������ʽ��
        setStyleSheet(R"(
            QSlider::groove:horizontal {
                border: 1px solid #bbb;
                background: #eee;
                height: 10px;
                border-radius: 5px;
                margin: 0px;
            }
            QSlider::handle:horizontal {
                background: #fff;
                border: 1px solid #888;
                width: 20px;
                height: 20px;
                margin: -5px 0;
                border-radius: 10px;
            }
            QSlider::sub-page:horizontal {
                background: rgb(102,183,255);
                border: 1px solid #bbb;
                height: 10px;
                border-radius: 5px;
                margin: 0px;
            }
            QSlider::add-page:horizontal {
                background: #c2c2c4;
                border: 1px solid #bbb;
                height: 10px;
                border-radius: 5px;
                margin: 0px;
            }
            QSlider::groove:vertical {
                border: 1px solid #bbb;
                background: #eee;
                width: 10px;
                border-radius: 5px;
                margin: 0px;
            }
            QSlider::handle:vertical {
                background: #fff;
                border: 1px solid #888;
                width: 20px;
                height: 20px;
                margin: 0 -5px;
                border-radius: 10px;
            }
            QSlider::sub-page:vertical {
                background: rgb(102,183,255);
                border: 1px solid #bbb;
                width: 10px;
                border-radius: 5px;
                margin: 0px;
            }
            QSlider::add-page:vertical {
                background: #c2c2c4;
                border: 1px solid #bbb;
                width: 10px;
                border-radius: 5px;
                margin: 0px;
            }
        )");
    }

signals:
    void sliderClicked(int value); // ����һ���źţ������ڻ��鱻���ʱ����

protected:
    // ��д��갴���¼�������
    void mousePressEvent(QMouseEvent* event) override {
        QSlider::mousePressEvent(event);
    }

    // ��д����ƶ��¼�������
    void mouseMoveEvent(QMouseEvent* event) override {
        QSlider::mouseMoveEvent(event);
    }

    // ��д����ͷ��¼�������
    void mouseReleaseEvent(QMouseEvent* event) override {
        // ����һ���������ڴ洢������Ļ���ֵ
        int value;

        // �жϻ���ķ���
        if (orientation() == Qt::Vertical) {
            // ����Ǵ�ֱ���򣬼��㷽���Ǹ��������λ�õ� y ����
            // ����Сֵ�����ֵ���б���ת��
            value = minimum() + ((maximum() - minimum()) * (height() - event->y())) / height();
        }
        else {
            // �����ˮƽ���򣬼��㷽���Ǹ��������λ�õ� x ����
            // ����Сֵ�����ֵ���б���ת��
            value = minimum() + ((maximum() - minimum()) * event->x()) / width();
        }

        // ���û������λ��
        setValue(value);

        // ���们�����ź�
        emit sliderClicked(value);

        // ȷ�����û��������ͷ��¼�����������Ĭ����Ϊ
        QSlider::mouseReleaseEvent(event);
    }

    // ��д�����¼�������
    void paintEvent(QPaintEvent* event) override {
        QSlider::paintEvent(event);

        // ����Сͼ��
        QPainter painter(this);
        QStyleOptionSlider opt;
        initStyleOption(&opt);

        QRect sliderRect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);

        // ����ͼ��λ��
        int iconX = sliderRect.center().x() - icon.width() / 2;
        int iconY = sliderRect.top() - icon.height(); // ͼ������ڻ����Ϸ�

        painter.drawPixmap(iconX, iconY, icon);
    }

private:
    QPixmap icon; // Сͼ��
};
