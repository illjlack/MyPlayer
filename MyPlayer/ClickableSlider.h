#pragma once
#pragma execution_character_set("utf-8")

#include <QSlider>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOptionSlider>

// 定义一个继承自 QSlider 的类 ClickableSlider
class ClickableSlider : public QSlider {
    Q_OBJECT

public:
    // 构造函数，接收滑块的方向和父控件指针
    ClickableSlider(Qt::Orientation orientation, QWidget* parent = nullptr)
        : QSlider(orientation, parent), icon(":/images/BottomPlay/slider_icon.png") {
        // 设置样式表
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
    void sliderClicked(int value); // 定义一个信号，用于在滑块被点击时发射

protected:
    // 重写鼠标按下事件处理函数
    void mousePressEvent(QMouseEvent* event) override {
        QSlider::mousePressEvent(event);
    }

    // 重写鼠标移动事件处理函数
    void mouseMoveEvent(QMouseEvent* event) override {
        QSlider::mouseMoveEvent(event);
    }

    // 重写鼠标释放事件处理函数
    void mouseReleaseEvent(QMouseEvent* event) override {
        // 定义一个变量用于存储计算出的滑块值
        int value;

        // 判断滑块的方向
        if (orientation() == Qt::Vertical) {
            // 如果是垂直方向，计算方法是根据鼠标点击位置的 y 坐标
            // 从最小值到最大值进行比例转换
            value = minimum() + ((maximum() - minimum()) * (height() - event->y())) / height();
        }
        else {
            // 如果是水平方向，计算方法是根据鼠标点击位置的 x 坐标
            // 从最小值到最大值进行比例转换
            value = minimum() + ((maximum() - minimum()) * event->x()) / width();
        }

        // 设置滑块的新位置
        setValue(value);

        // 发射滑块点击信号
        emit sliderClicked(value);

        // 确保调用基类的鼠标释放事件，处理其他默认行为
        QSlider::mouseReleaseEvent(event);
    }

    // 重写绘制事件处理函数
    void paintEvent(QPaintEvent* event) override {
        QSlider::paintEvent(event);

        // 绘制小图标
        QPainter painter(this);
        QStyleOptionSlider opt;
        initStyleOption(&opt);

        QRect sliderRect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);

        // 计算图标位置
        int iconX = sliderRect.center().x() - icon.width() / 2;
        int iconY = sliderRect.top() - icon.height(); // 图标绘制在滑块上方

        painter.drawPixmap(iconX, iconY, icon);
    }

private:
    QPixmap icon; // 小图标
};
