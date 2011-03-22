#include "menu.h"
#include "pages/default.h"

HGMenu::HGMenu(WStackedWidget * menuContents, SessionInfo * sess, WContainerWidget *parent)
: WContainerWidget(parent)
{
    session = sess;
    this->menuContents = menuContents;
    container = this;

    menu = new WMenu(menuContents, Wt::Vertical, this);
    menu->setRenderAsList(true);
    menu->addItem("Home", new DefaultPage(session));
    menu->addItem("test2sdfsfasfd", new WText("tescik2"));
    menu->addItem("test3", new WText("tescik3"));

    addWidget(menu);
}

HGMenu::~HGMenu()
{
    delete menu;
}
