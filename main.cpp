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

#include <WApplication>
#include <WEnvironment>
#include <WMenu>
#include <WStackedWidget>

#include "defines.h"
#include "menu.h"
#include "database.h"

using namespace Wt;

class PlayersPanel : public WApplication
{
public:
    PlayersPanel(const WEnvironment& env);
    ~PlayersPanel();
private:
    WStackedWidget * content;       // container to show menu items after click (main content container)
    WContainerWidget * page;        // whole page container
    HGMenu * menu;                  // menu
    SessionInfo * session;          // store info about user session
    void GetLangTexts();
};

PlayersPanel::PlayersPanel(const WEnvironment& env)
: WApplication(env)
{
    setTitle(SITE_TITLE);

    // create all variables and set id/class for css use (based on css/layout.html)

    // div body
    page = new WContainerWidget(root());
    page->setContentAlignment(AlignCenter);
    content = new WStackedWidget();
    session = new SessionInfo();
    GetLangTexts();
    menu = new HGMenu(content, session);

    page->setId("body");

    // add header img
    page->addWidget(new WImage("img/header_top.jpg", page));

    // create and add div "header"
    WContainerWidget * tmpContainer = new WContainerWidget(page);
    tmpContainer->setId("header");

    // add div "character" do div "header"
    WContainerWidget * tmpContainer2 = new WContainerWidget(tmpContainer);
    tmpContainer2->setId("character");
    tmpContainer->addWidget(tmpContainer2);

    // add img logo to div "header"
    WImage * tmpImg = new WImage("img/logo.png", tmpContainer);
    tmpImg->setId("logo");
    tmpImg->setAlternateText(SITE_NAME);
    tmpContainer->addWidget(tmpImg);

    page->addWidget(tmpContainer);

    // reset variables
    tmpContainer = NULL;
    tmpContainer2 = NULL;
    tmpImg = NULL;

    // add empty div "header-bot" to div "body"
    tmpContainer = new WContainerWidget(page);
    tmpContainer->setId("header-bot");

    page->addWidget(tmpContainer);

    tmpContainer = NULL;

    // create and add div "content"
    tmpContainer = new WContainerWidget(page);
    tmpContainer->setId("content");
    tmpContainer->setStyleClass("content-collapsed");

    WTable * tmpTable = new WTable(tmpContainer);
    tmpTable->setStyleClass("content-table");
    tmpTable->setAttributeValue("align", "center");
    tmpTable->setAttributeValue("cellpadding", "0");
    tmpTable->setAttributeValue("cellspacing", "0");
    tmpTable->setAttributeValue("border", "0");

    tmpTable->elementAt(0, 0)->setId("sidebar");
    menu->setId("sidemenubar");
    tmpTable->elementAt(0, 0)->addWidget(menu);

    tmpTable->elementAt(0, 1)->setId("main");
    tmpTable->elementAt(0, 1)->setStyleClass("main-collapsed");
    content->setId("mainframe");
    content->setStyleClass("mainframe-collapsed");
    tmpTable->elementAt(0, 1)->addWidget(content);

    tmpContainer->addWidget(tmpTable);

    page->addWidget(tmpContainer);

    tmpTable = NULL;
    tmpContainer = NULL;

    // add div "footer"
    tmpContainer = new WContainerWidget(page);
    tmpContainer->setId("footer");

    tmpContainer2 = new WContainerWidget(tmpContainer);
    tmpContainer2->setId("chains");

    tmpContainer->addWidget(tmpContainer2);

    page->addWidget(tmpContainer);

    if (env.agentIsIElt(7))
        useStyleSheet("css/ie.css");
    else
        useStyleSheet("css/style.css");

    root()->addWidget(page);

    WBreak * tmpBreak = new WBreak(root());
    tmpBreak->clearSides();
    root()->addWidget(tmpBreak);

    tmpBreak = NULL;

    root()->addWidget(new WBreak());
    root()->addWidget(new WBreak());
}

PlayersPanel::~PlayersPanel()
{
    delete menu;
    delete content;
    delete session;
    delete page;
}

void PlayersPanel::GetLangTexts()
{

    if (Database * tmpDb = new Database(PANEL_SQL_HOST, PANEL_SQL_LOGIN, PANEL_SQL_PASS, SQL_PANELDB, PANEL_SQL_PORT))
    {
        tmpDb->SetQuery("SELECT id, lang_0, lang_1 FROM LangTexts");

        if (tmpDb->ExecuteQuery() > 0)
        {
            std::list<DatabaseRow*> tmpList = tmpDb->GetRows();

            for (std::list<DatabaseRow*>::const_iterator itr = tmpList.begin(); itr != tmpList.end(); ++itr)
            {
                DatabaseRow * row = *itr;

                if (row)
                {
                    Text tmpText(row->fields[0].GetUInt32());

                    for (int i = 0; i < LANG_COUNT; ++i)
                        tmpText.texts[i] = row->fields[i+1].GetWString();

                    session->langTexts[tmpText.textId] = tmpText;
                }
            }
        }
    }
}

WApplication *createApplication(const WEnvironment& env)
{
    // You could read information from the environment to decide
    // whether the user has permission to start a new application
    return new PlayersPanel(env);
}

int main(int argc, char **argv)
{
    return WRun(argc, argv, &createApplication);
}
