#include "MiddleWidgetRightStack.h"
#include <QDebug>
#include "CommentEnum.h"

// LocalMusicPageʵ��
LocalMusicPage::LocalMusicPage(QWidget* parent)
    : MiddleWidgetRightBase(parent)  // ���û��๹�캯��
{
}

// OnlineMusicPageʵ��
OnlineMusicPage::OnlineMusicPage(QWidget* parent)
    : MiddleWidgetRightBase(parent)  // ���û��๹�캯��
{
}

// FavoritePageʵ��
FavoritePage::FavoritePage(QWidget* parent)
    : MiddleWidgetRightBase(parent)  // ���û��๹�캯��
{
}

// MiddleWidgetRightStackʵ��
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
