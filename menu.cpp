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
    menu->addItem("Home", new DefaultPage(session));
    menu->addItem("test2sdfsfasfd", new WText("tescik2"));
    menu->addItem("test3", new WText("tescik3"));

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
    }
}

void HGMenu::SetPlLang()
{
    session->language = LANG_PL;
    RefreshMenuWidgets();
}

void HGMenu::SetEngLang()
{
    session->language = LANG_EN;
    RefreshMenuWidgets();
}
