#include "menu.h"
#include "pages/default.h"

HGMenu::HGMenu(WStackedWidget * menuContents, SessionInfo * sess, WContainerWidget *parent)
: WContainerWidget(parent)
{
    session = sess;
    this->menuContents = menuContents;
    container = this;

    WContainerWidget * tmpCont = new WContainerWidget();
    tmpCont->setContentAlignment(AlignCenter);
    plLang = new WPushButton("PL");
    plLang->clicked().connect(this, &HGMenu::SetPlLang);
    enLang = new WPushButton("EN");
    enLang->clicked().connect(this, &HGMenu::SetEngLang);

    tmpCont->addWidget(plLang);
    tmpCont->addWidget(enLang);

    addWidget(tmpCont);
    addWidget(new WBreak());
    addWidget(new WBreak());

    tmpCont = NULL;

    login = new WLineEdit();
    login->setEchoMode(WLineEdit::Normal);

    pass = new WLineEdit();
    pass->setEchoMode(WLineEdit::Password);

    btn = new WPushButton("log me in");
    btn->clicked().connect(this, &HGMenu::LogMeIn);

    breakTab = new WBreak*[3];
    for (int i = 0; i < 3; i++)
        breakTab[i] = new WBreak();

    addWidget(login);
    addWidget(breakTab[0]);
    addWidget(pass);
    addWidget(breakTab[1]);
    addWidget(btn);
    addWidget(breakTab[2]);

    menu = new WMenu(menuContents, Wt::Vertical, this);
    menu->setRenderAsList(true);
    ShowMenuOptions();

    addWidget(menu);
}

HGMenu::~HGMenu()
{
    delete menu;
    delete login;
    delete pass;
    delete btn;

    for (int i = 0; i < 3; i++)
        delete breakTab[i];

    delete [] breakTab;
}

void HGMenu::RefreshMenuWidgets()
{
    for (int i = 0; i < menuContents->count(); ++i)
        menuContents->widget(i)->refresh();
}

void HGMenu::LogMeIn()
{
    // just for test - must be replaced with normal auth check
    if (login->text() == "test" && pass->text() == "test")
    {
        login->setHidden(true);
        pass->setHidden(true);
        btn->setHidden(true);
        login->setDisabled(true);
        pass->setDisabled(true);
        btn->setDisabled(true);

        removeWidget(login);
        removeWidget(pass);
        removeWidget(btn);

        for (int i = 0; i < 3; ++i)
        {
            breakTab[i]->setHidden(true);
            breakTab[i]->setDisabled(true);
            removeWidget(breakTab[i]);
        }

        session->login = login->text();
        session->pass = pass->text();
        session->accid = 1;

        RefreshMenuWidgets();

        login->setText("");
        pass->setText("");
        refresh();
    }
}

void HGMenu::SetPlLang()
{
    session->language = LANG_PL;
    RefreshMenuWidgets();
    refresh();
}

void HGMenu::SetEngLang()
{
    session->language = LANG_EN;
    RefreshMenuWidgets();
    refresh();
}

void HGMenu::ShowMenuOptions()
{
    std::vector<WMenuItem*>::const_iterator itr = menu->items().begin();
    while(itr != menu->items().end())
    {
        WMenuItem * tmp = *itr;
        menu->removeItem(tmp);
        delete tmp;
    }

    menu->addItem("Home", new DefaultPage(session));

    switch (session->language)
    {
        case LANG_PL:
            menu->addItem("test2sdfsfasfd PL", new WText("tescik2"));

            if (session->accid)
            {
                menu->addItem("test3 PL", new WText("tescik3"));

            }
            break;

        case LANG_EN:
            menu->addItem("test2sdfsfasfd EN", new WText("tescik2"));

            if (session->accid)
            {
                menu->addItem("test3 EN", new WText("tescik3"));

            }
            break;
    }
}

void HGMenu::refresh()
{
    ShowMenuOptions();

    WContainerWidget::refresh();
}
