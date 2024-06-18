#include "MiddleWidgetRightStack.h"
#include <QDebug>
#include "CommentEnum.h"

// LocalMusicPage实现
LocalMusicPage::LocalMusicPage(QWidget* parent)
    : MiddleWidgetRightBase(parent)  // 调用基类构造函数
{
}

// OnlineMusicPage实现
OnlineMusicPage::OnlineMusicPage(QWidget* parent)
    : MiddleWidgetRightBase(parent)  // 调用基类构造函数
{
}

// FavoritePage实现
FavoritePage::FavoritePage(QWidget* parent)
    : MiddleWidgetRightBase(parent)  // 调用基类构造函数
{
}

// MiddleWidgetRightStack实现
MiddleWidgetRightStack::MiddleWidgetRightStack(QWidget* parent)
    : QStackedWidget(parent)
{
    localMusicPage = new LocalMusicPage(this);
    onlineMusicPage = new OnlineMusicPage(this);
    favoritePage = new FavoritePage(this);

    addWidget(localMusicPage);
    addWidget(onlineMusicPage);
    addWidget(favoritePage);
}
