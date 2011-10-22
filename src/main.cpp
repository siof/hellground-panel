/*
*    HG Players Panel - web panel for HellGround server Players
*    Copyright (C) 2011 HellGround Team : Siof, lukaasm,
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU Affero General Public License version 3 as
*    published by the Free Software Foundation.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Affero General Public License for more details.
*
*    You should have received a copy of the GNU Affero General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <WMenu>
#include <WStackedWidget>

#include <stdarg.h>

#include "defines.h"
#include "menu.h"
#include "database.h"

#include "jwsmtp/jwsmtp.h"

using namespace Wt;

void SendMail(WString& from, WString& to, WString& sub, WString& msg)
{
    jwsmtp::mailer mail;

    mail.addrecipient(to.toUTF8().c_str());
    mail.setsender(from.toUTF8().c_str());
    mail.setsubject(sub.toUTF8().c_str());
    mail.setmessage(msg.toUTF8().c_str());

    mail.setserver(MAIL_HOST);
    mail.username(MAIL_USER);
    mail.password(MAIL_PASS);

    mail.send();
}

WString GetExpansionName(SessionInfo * sess, int index)
{
    switch (index)
    {
        case EXPANSION_TBC:
            return sess->GetText(TXT_EXPANSION_TBC);
        case EXPANSION_WOTLK:
            return sess->GetText(TXT_EXPANSION_WOTLK);
        case EXPANSION_CATA:
            return sess->GetText(TXT_EXPANSION_CATACLYSM);
        case EXPANSION_PRETBC:
        default:
            return sess->GetText(TXT_EXPANSION_PRETBC);
    }
}

WString GetLocale(int index)
{
    switch (index)
    {
        case 0:
            return WString::fromUTF8("en");
        default:
            return WString::fromUTF8("unknown");
    }
}

int irand(int min, int max)
{
    return rand()%(max - min) + min;
}

void console(DebugFlags flag, char const* text, ...)
{
    if (DebugFlags(DEBUG_OPTIONS) & flag)
    {
        va_list args;
        va_start(args, text);
        vprintf(text, args);
        va_end(args);
    }
}

void Log(LogFlags flag, char const* text, ...)
{
    if (LogFlags(LOG_OPTIONS) & flag)
    {
        char buffer[strlen(text) + 250];

        va_list args;
        va_start(args, text);
        vsprintf(buffer, text, args);
        va_end(args);

        wApp->log("notice") << buffer;
    }
}

void GetTeleportPosition(int race, Location & loc)
{
    if (race < 0 || race > 11 || race == 9)
        return;

    int mod = 1;

    if (race > 8)
        ++mod;

    loc.mapId = RaceLocs[race - mod][0];
    loc.zone = RaceLocs[race - mod][1];
    loc.posX = RaceLocs[race - mod][2];
    loc.posY = RaceLocs[race - mod][3];
    loc.posZ = RaceLocs[race - mod][4];
}

std::string GetFormattedString(const char * format, ...)
{
    char buffer[strlen(format) + 1000];

    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);

    return std::string(buffer);
}

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
    void LoadLangTexts();
};

PlayersPanel::PlayersPanel(const WEnvironment& env)
: WApplication(env)
{
    // create all variables and set id/class for css use (based on css/layout.html)

    // div body
    page = new WContainerWidget(root());
    page->setContentAlignment(AlignCenter);
    content = new WStackedWidget();
    session = new SessionInfo();
    session->sessionIp = env.clientAddress();
    LoadLangTexts();

    setTitle(session->GetText(TXT_SITE_TITLE));

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

void PlayersPanel::LoadLangTexts()
{
    console(DEBUG_CODE, "Call void PlayersPanel::LoadLangTexts()");

    Database tmpDb;
    if (tmpDb.Connect(PANEL_DB_DATA, SQL_PANELDB))
    {
        tmpDb.SetQuery("SELECT id, lang_0, lang_1 FROM LangTexts");

        if (tmpDb.ExecuteQuery() > RETURN_EMPTY)
        {
            std::list<DatabaseRow*> tmpList = tmpDb.GetRows();

            for (std::list<DatabaseRow*>::const_iterator itr = tmpList.begin(); itr != tmpList.end(); ++itr)
            {
                DatabaseRow * row = *itr;

                if (row)
                {
                    LangText tmpText(row->fields[0].GetUInt32());

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
    console(DEBUG_CODE, "Call WApplication *createApplication(const WEnvironment& env)");
    // You could read information from the environment to decide
    // whether the user has permission to start a new application

    return new PlayersPanel(env);
}

int main(int argc, char **argv)
{
    return WRun(argc, argv, &createApplication);
}
