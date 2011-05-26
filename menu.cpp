/*
*    HG Players Panel - web panel for HellGround server Players
*    Copyright (C) 2011 HellGround Team : Siof, lukaasm,
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU Affero General Public License as
*    published by the Free Software Foundation, either version 3 of the
*    License, or (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Affero General Public License for more details.
*
*    You should have received a copy of the GNU Affero General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "menu.h"
#include "pages/default.h"
#include "database.h"

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
    menu->setRenderAsList(false);
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

void HGMenu::RefreshActiveMenuWidget()
{
    menuContents->currentWidget()->refresh();
}

void HGMenu::LogMeIn()
{
    Database * db;
    if (!(db = new Database(SQL_HOST, SQL_LOGIN, SQL_PASSWORD, SQL_REALMDB, 0)))
    {
        errorPage->SetErrorMsg(ERROR_DB_CONNECT);
        menu->select(errorPageMenuItem);
        RefreshActiveMenuWidget();
        return;
    }

    // simple login check ... should be replaced with something better

    std::string escapedLogin = db->EscapeString(login->text());
    std::string escapedPass = db->EscapeString(pass->text());

    std::string query = "SELECT username, sha_pass_hash, id, gmlevel, email, joindate, last_ip, last_login, locked, expansion FROM account WHERE username = '";
    query += escapedLogin;
    query += "' AND sha_pass_hash = SHA1(UPPER('";
    query += escapedLogin;
    query += ":";
    query += escapedPass;
    query += "'));";

    printf("\n\nquery: %s\n\n", query.c_str());

    db->SetQuery(query);

    printf("\n\nquery: %s\n\n", db->GetQuery().c_str());

    if (db->ExecuteQuery() > 0)
    {
        DatabaseRow * row = db->GetRow(0);
        if (row)
        {
            session->login = row->fields[0].GetWString();
            session->pass = row->fields[1].GetWString();
            session->accid = row->fields[2].GetUInt64();
            session->accLvl = row->fields[3].GetAccountLevel();
            session->email = row->fields[4].GetWString();
            session->joinDate = row->fields[5].GetWString();
            session->lastIp = row->fields[6].GetWString();
            session->lastLogin = row->fields[7].GetWString();
            session->locked = row->fields[8].GetBool();
            session->expansion = row->fields[9].GetInt();

            login->setText("");
            pass->setText("");

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

            refresh();
            errorPageMenuItem->hide();
            menu->select(0);
            RefreshActiveMenuWidget();
        }
        else
        {
            errorPage->SetErrorMsg(ERROR_ROW_NOT_FOUND);
            menu->select(errorPageMenuItem);
            RefreshActiveMenuWidget();
        }
    }
    else
    {
        #ifdef SHOW_MYSQL_ERRORS
        errorPage->SetAdditionalErrorMsg(db->GetError());
        #endif

        errorPage->SetErrorMsg(ERROR_LOGIN);
        menu->select(errorPageMenuItem);
        RefreshActiveMenuWidget();
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

    errorPage = new ErrorPage(session);
    errorPageMenuItem = new WMenuItem("error", errorPage);
    errorPageMenuItem->hide();
    errorPageMenuItem->disable();
    menu->addItem(errorPageMenuItem);
}

void HGMenu::refresh()
{
    ShowMenuOptions();

    WContainerWidget::refresh();
}
