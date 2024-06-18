#pragma once
#pragma execution_character_set("utf-8")

#include <QWidget>
#include <QStackedWidget>
#include "MiddleWidgetRightBase.h"

// ��������ҳ��
class LocalMusicPage : public MiddleWidgetRightBase
{
    Q_OBJECT

public:
    explicit LocalMusicPage(QWidget* parent = nullptr);  // ���캯��
};

// ��������ҳ��
class OnlineMusicPage : public MiddleWidgetRightBase
{
    Q_OBJECT

public:
    explicit OnlineMusicPage(QWidget* parent = nullptr);  // ���캯��
};

// ϲ������ҳ��
class FavoritePage : public MiddleWidgetRightBase
{
    Q_OBJECT

public:
    explicit FavoritePage(QWidget* parent = nullptr);  // ���캯��
};

// �м��Ҳ��ջҳ��
class MiddleWidgetRightStack : public QStackedWidget
{
    Q_OBJECT

public:
    explicit MiddleWidgetRightStack(QWidget* parent = nullptr);  // ���캯��

    LocalMusicPage* localMusicPage;  // ��������ҳ��
    OnlineMusicPage* onlineMusicPage;  // ��������ҳ��
    FavoritePage* favoritePage;  // ϲ������ҳ��
};
