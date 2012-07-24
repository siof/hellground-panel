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

#include "main.h"

#include <stdarg.h>

#include <boost/algorithm/string/case_conv.hpp>

#include <jwsmtp/jwsmtp.h>

#include <Wt/WEnvironment>
#include <Wt/WImage>
#include <Wt/WMenu>
#include <Wt/WMenuItem>
#include <Wt/WOverlayLoadingIndicator>
#include <Wt/WPushButton>
#include <Wt/WStackedWidget>
#include <Wt/WTable>
#include <Wt/WText>
#include <Wt/WTemplate>
#include <Wt/Utils>

#include "defines.h"
#include "database.h"
#include "menu.h"
#include "LangsWidget.h"
#include "login.h"

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

WString GetExpansionName(int index)
{
    switch (index)
    {
        case EXPANSION_TBC:
            return Wt::WString::tr(TXT_EXPANSION_TBC);
        case EXPANSION_WOTLK:
            return Wt::WString::tr(TXT_EXPANSION_WOTLK);
        case EXPANSION_CATA:
            return Wt::WString::tr(TXT_EXPANSION_CATACLYSM);
        case EXPANSION_PRETBC:
        default:
            return Wt::WString::tr(TXT_EXPANSION_CLASSIC);
    }
}

WString GetLocale(int index)
{
    switch (index)
    {
        case 0:
            return Wt::WString::tr("locale.enus");
        case 1:
            return Wt::WString::tr("locale.kokr");
        case 2:
            return Wt::WString::tr("locale.frfr");
        case 3:
            return Wt::WString::tr("locale.dede");
        case 4:
            return Wt::WString::tr("locale.zhcn");
        case 5:
            return Wt::WString::tr("locale.zhtw");
        case 6:
            return Wt::WString::tr("locale.eses");
        case 7:
            return Wt::WString::tr("locale.esmx");
        case 8:
            return Wt::WString::tr("locale.ruru");
        default:
            return Wt::WString::tr("unknown");
    }
}

WString GetRaceName(int index)
{
    switch (index)
    {
        case RACE_HUMAN:
            return Wt::WString::tr(TXT_RACE_HUMAN);
        case RACE_ORC:
            return Wt::WString::tr(TXT_RACE_ORC);
        case RACE_DWARF:
            return Wt::WString::tr(TXT_RACE_DWARF);
        case RACE_NIGHT_ELF:
            return Wt::WString::tr(TXT_RACE_NIGHT_ELF);
        case RACE_UNDEAD:
            return Wt::WString::tr(TXT_RACE_UNDEAD);
        case RACE_TAUREN:
            return Wt::WString::tr(TXT_RACE_TAUREN);
        case RACE_GNOME:
            return Wt::WString::tr(TXT_RACE_GNOME);
        case RACE_TROLL:
            return Wt::WString::tr(TXT_RACE_TROLL);
        case RACE_BLOOD_ELF:
            return Wt::WString::tr(TXT_RACE_BLOOD_ELF);
        case RACE_DRAENEI:
            return Wt::WString::tr(TXT_RACE_DRAENEI);
        default:
            return Wt::WString::tr(TXT_GEN_UNKNOWN);
    }
}

WString GetClassName(int index)
{
    switch (index)
    {
        case CLASS_WARRIOR:
            return Wt::WString::tr(TXT_CLASS_WARRIOR);
        case CLASS_PALADIN:
            return Wt::WString::tr(TXT_CLASS_PALADIN);
        case CLASS_HUNTER:
            return Wt::WString::tr(TXT_CLASS_HUNTER);
        case CLASS_ROGUE:
            return Wt::WString::tr(TXT_CLASS_ROGUE);
        case CLASS_PRIEST:
            return Wt::WString::tr(TXT_CLASS_PRIEST);
        case CLASS_SHAMAN:
            return Wt::WString::tr(TXT_CLASS_SHAMAN);
        case CLASS_MAGE:
            return Wt::WString::tr(TXT_CLASS_MAGE);
        case CLASS_WARLOCK:
            return Wt::WString::tr(TXT_CLASS_WARLOCK);
        case CLASS_DRUID:
            return Wt::WString::tr(TXT_CLASS_DRUID);
        default:
            return Wt::WString::tr(TXT_GEN_UNKNOWN);
    }
}

WString GetQuestStatus(int index, bool rewarded)
{
    if (rewarded)
        return Wt::WString::tr(TXT_QUEST_STATUS_REWARDED);

    switch (index)
    {
        case 0:
            return Wt::WString::tr(TXT_QUEST_STATUS_NONE);
        case 1:
            return Wt::WString::tr(TXT_QUEST_STATUS_COMPLETE);
        case 2:
            return Wt::WString::tr(TXT_QUEST_STATUS_UNAVAILABLE);
        case 3:
            return Wt::WString::tr(TXT_QUEST_STATUS_INCOMPLETE);
        case 4:
            return Wt::WString::tr(TXT_QUEST_STATUS_AVAILABLE);
        default:
            return Wt::WString::tr(TXT_GEN_UNKNOWN);
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
            uint64 reduction = 5*GOLD*months;
            if (reduction > lastCost)
                reduction = lastCost;

            int64 new_cost = lastCost - reduction;
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

std::string GetUpperSHA1(std::string & txt)
{
    std::string upper = boost::algorithm::to_upper_copy(txt);
    std::string sha1d = Wt::Utils::hexEncode(Wt::Utils::sha1(upper));

    return sha1d;
}

Wt::WString WGetUpperSHA1(std::string & txt)
{
    return Wt::WString::fromUTF8(GetUpperSHA1(txt));
}

PlayersPanel::PlayersPanel(const WEnvironment& env)
    : WApplication(env)
{
    session = new SessionInfo();
    session->sessionIp = env.clientAddress();

    setLoadingIndicator(new Wt::WOverlayLoadingIndicator("loading"));

    setTitle(Wt::WString::tr(TXT_SITE_TITLE));

    useStyleSheet("res/style.css");
    root()->setStyleClass("main");

    messageResourceBundle().use("langs/panel");

    // page creation
    templ = new WTemplate();

    content =  new Wt::WStackedWidget();
    langs = new LangsWidget();
    menu = CreateMenu();
    login = new LoginWidget(session, templ);

    templ->bindWidget("add-langs", langs);
    templ->bindWidget("add-login", login);
    templ->bindWidget("add-menu", menu);
    templ->bindWidget("add-content", content);
    templ->bindWidget("add-profile", new WText("testus profile"));

    templ->addFunction("tr", &WTemplate::Functions::tr);
    templ->setCondition("if-loggedin", false);
    templ->setCondition("if-notlogged", true);

    templ->setTemplateText(
        "<div class=\"head\" ></div>"
        "<div class=\"sidebar\" >"

            "<div class=\"profile-top\"></div>"
            "<div class=\"profile\" >"
//                "${<if-loggedin>}"
//                    "${add-profile}"
//                "${</if-loggedin>}"
                "${<if-notlogged>}"
                    "${add-login}"
                "${</if-notlogged>}"
            "</div>"
            "<div class=\"profile-bottom\"></div>"

            "<div class=\"langs-top\"></div>"
            "<div class=\"langs\" >${add-langs}</div>"
            "<div class=\"langs-bottom\"></div>"

            "<div class=\"menu-top\"></div>"
            "<div class=\"menu\">${add-menu}</div>"
            "<div class=\"menu-bottom\"></div>"

        "</div>"
        "<div class=\"content\" >${add-content}</div> "
        "<div class=\"footer\" >${tr:site.footer}</div>");

    root()->addWidget(templ);
}

Wt::WMenu * PlayersPanel::CreateMenu(Wt::Orientation ori)
{
    menu = new HGMenu(content, session, templ, ori);

    return menu;
}

PlayersPanel::~PlayersPanel()
{
    delete content;
    delete session;
}

WApplication *createApplication(const Wt::WEnvironment& env)
{
    console(DEBUG_CODE, "Call WApplication *createApplication(const WEnvironment& env)");
    // You could read information from the environment to decide
    // whether the user has permission to start a new application

    PlayersPanel * tmpPanel = new PlayersPanel(env);

    return tmpPanel;
}

int main(int argc, char **argv)
{
    srand(time(NULL));
    return WRun(argc, argv, &createApplication);
}
