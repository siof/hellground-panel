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

void SendMail(const WString& from, const WString& to, const WString& sub, const WString& msg)
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

WString GetRaceName(SessionInfo * sess, int index)
{
    if (!sess)
        return WString::fromUTF8("unknown");

    switch (index)
    {
        case RACE_HUMAN:
            return sess->GetText(TXT_LBL_RACE_HUMAN);
        case RACE_ORC:
            return sess->GetText(TXT_LBL_RACE_ORC);
        case RACE_DWARF:
            return sess->GetText(TXT_LBL_RACE_DWARF);
        case RACE_NIGHT_ELF:
            return sess->GetText(TXT_LBL_RACE_NIGHT_ELF);
        case RACE_UNDEAD:
            return sess->GetText(TXT_LBL_RACE_UNDEAD);
        case RACE_TAUREN:
            return sess->GetText(TXT_LBL_RACE_TAUREN);
        case RACE_GNOME:
            return sess->GetText(TXT_LBL_RACE_GNOME);
        case RACE_TROLL:
            return sess->GetText(TXT_LBL_RACE_TROLL);
        case RACE_BLOOD_ELF:
            return sess->GetText(TXT_LBL_RACE_BLOOD_ELF);
        case RACE_DRAENEI:
            return sess->GetText(TXT_LBL_RACE_DRAENEI);
        default:
            return sess->GetText(TXT_LBL_UNKNOWN);
    }
}

WString GetClassName(SessionInfo * sess, int index)
{
    if (!sess)
        return WString::fromUTF8("unknown");

    switch (index)
    {
        case CLASS_WARRIOR:
            return sess->GetText(TXT_LBL_CLASS_WARRIOR);
        case CLASS_PALADIN:
            return sess->GetText(TXT_LBL_CLASS_PALADIN);
        case CLASS_HUNTER:
            return sess->GetText(TXT_LBL_CLASS_HUNTER);
        case CLASS_ROGUE:
            return sess->GetText(TXT_LBL_CLASS_ROGUE);
        case CLASS_PRIEST:
            return sess->GetText(TXT_LBL_CLASS_PRIEST);
        case CLASS_SHAMAN:
            return sess->GetText(TXT_LBL_CLASS_SHAMAN);
        case CLASS_MAGE:
            return sess->GetText(TXT_LBL_CLASS_MAGE);
        case CLASS_WARLOCK:
            return sess->GetText(TXT_LBL_CLASS_WARLOCK);
        case CLASS_DRUID:
            return sess->GetText(TXT_LBL_CLASS_DRUID);
        default:
            return sess->GetText(TXT_LBL_UNKNOWN);
    }
}

WString GetQuestStatus(SessionInfo * sess, int index, bool rewarded)
{
    if (!sess)
       return WString::fromUTF8("unknown");

    if (rewarded)
        return sess->GetText(TXT_LBL_QUEST_STATUS_REWARDED);

    switch (index)
    {
        case 0:
            return sess->GetText(TXT_LBL_QUEST_STATUS_NONE);
        case 1:
            return sess->GetText(TXT_LBL_QUEST_STATUS_COMPLETE);
        case 2:
            return sess->GetText(TXT_LBL_QUEST_STATUS_UNAVAILABLE);
        case 3:
            return sess->GetText(TXT_LBL_QUEST_STATUS_INCOMPLETE);
        case 4:
            return sess->GetText(TXT_LBL_QUEST_STATUS_AVAILABLE);
        default:
            return sess->GetText(TXT_LBL_UNKNOWN);
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

ConflictSide GetSide(const uint8 & race)
{
    switch (race)
    {
        case RACE_BLOOD_ELF:
        case RACE_ORC:
        case RACE_TAUREN:
        case RACE_TROLL:
        case RACE_UNDEAD:
            return SIDE_HORDE;
        case RACE_DRAENEI:
        case RACE_DWARF:
        case RACE_GNOME:
        case RACE_HUMAN:
        case RACE_NIGHT_ELF:
            return SIDE_ALLIANCE;
        default:
            return SIDE_UNKNOWN;
    }
}

bool SameSide(const uint8 & race1, const uint8 & race2)
{
    return GetSide(race1) == GetSide(race2);
}

uint32 CalculateTalentCost(uint32 lastCost, uint32 months)
{
    // The first time reset costs 1 gold
    if (lastCost < 1*GOLD)
        return 1*GOLD;
    // then 5 gold
    else if (lastCost < 5*GOLD)
        return 5*GOLD;
    // After that it increases in increments of 5 gold
    else if (lastCost < 10*GOLD)
        return 10*GOLD;
    else
    {
        if (months > 0)
        {
            // This cost will be reduced by a rate of 5 gold per month
            int64 new_cost = lastCost - 5*GOLD*months;
            // to a minimum of 10 gold.
            return (new_cost < 10*GOLD ? 10*GOLD : new_cost);
        }
        else
        {
            // After that it increases in increments of 5 gold
            uint32 new_cost = lastCost + 5*GOLD;
            // until it hits a cap of 50 gold.
            if (new_cost > 50*GOLD)
                new_cost = 50*GOLD;
            return new_cost;
        }
    }
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

//    content->setTransitionAnimation(WAnimation(WAnimation::SlideInFromBottom, WAnimation::EaseIn), true);

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

        if (tmpDb.ExecuteQuery() > DB_RESULT_EMPTY)
        {
            std::list<DatabaseRow*> tmpList = tmpDb.GetRows();

            tmpDb.Disconnect();

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

std::map<uint32, SpellInfo> spells;

int main(int argc, char **argv)
{
    srand(time(NULL));

    Database * db = new Database();
    db->SetLogging(false);
    db->Connect(PANEL_DB_DATA, SQL_PANELDB);
    db->ExecuteQuery("SELECT entry, name FROM spells");

    std::list<DatabaseRow*> rows = db->GetRows();

    uint32 tmpEntry;
    for (std::list<DatabaseRow*>::const_iterator itr = rows.begin(); itr != rows.end(); ++itr)
    {
        tmpEntry = (*itr)->fields[0].GetUInt32();
        spells[tmpEntry].entry = tmpEntry;
        spells[tmpEntry].name = (*itr)->fields[1].GetWString();
    }

    delete db;
    db = NULL;

    return WRun(argc, argv, &createApplication);
}
