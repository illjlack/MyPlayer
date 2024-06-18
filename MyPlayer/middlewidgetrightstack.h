#pragma once
#pragma execution_character_set("utf-8")

#include <QWidget>
#include <QStackedWidget>
#include "MiddleWidgetRightBase.h"

// 本地音乐页面
class LocalMusicPage : public MiddleWidgetRightBase
{
    Q_OBJECT

public:
    explicit LocalMusicPage(QWidget* parent = nullptr);  // 构造函数
};

// 在线音乐页面
class OnlineMusicPage : public MiddleWidgetRightBase
{
    Q_OBJECT

public:
    explicit OnlineMusicPage(QWidget* parent = nullptr);  // 构造函数
};

// 喜爱音乐页面
class FavoritePage : public MiddleWidgetRightBase
{
    Q_OBJECT

public:
    explicit FavoritePage(QWidget* parent = nullptr);  // 构造函数
};

// 中间右侧堆栈页面
class MiddleWidgetRightStack : public QStackedWidget
{
    Q_OBJECT

public:
    explicit MiddleWidgetRightStack(QWidget* parent = nullptr);  // 构造函数

    LocalMusicPage* localMusicPage;  // 本地音乐页面
    OnlineMusicPage* onlineMusicPage;  // 在线音乐页面
    FavoritePage* favoritePage;  // 喜爱音乐页面
};
