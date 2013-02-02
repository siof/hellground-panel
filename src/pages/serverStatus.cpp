/*
*    HG Players Panel - web panel for HellGround server Players
*    Copyright (C) 2011-2012 HellGround Team : Siof, lukaasm,
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

/********************************************//**
 * \addtogroup Pages
 * \{
 *
 * \addtogroup ServerStatusPage Server status page
 * \{
 *
 * \file serverStatus.cpp
 * This file contains code needed to parse and show server status.
 *
 ***********************************************/

#include "serverStatus.h"

#include <iostream>
#include <sstream>

#include <Wt/Http/Client>
#include <Wt/Http/Message>
#include <Wt/WBreak>
#include <Wt/WTable>
#include <Wt/WText>
#include <Wt/WTimer>

#include "../config.h"
#include "../misc.h"

/********************************************//**
 * \brief Creates new ServerStatusPage object.
 *
 * \param sess      Contains current session informations.
 * \param parent    Parent container type object in which this container should be placed.
 *
 ***********************************************/

ServerStatusPage::ServerStatusPage(Wt::WContainerWidget * parent)
: Wt::WContainerWidget(parent)
{
    Misc::Console(DEBUG_CODE, "%s\n", __FUNCTION__);
    timer = new Wt::WTimer();
    timer->setInterval(60000);
    timer->timeout().connect(this, &ServerStatusPage::RunUpdateStatus);

    realms = new Wt::WTable*[sConfig.GetConfig(CONFIG_REALMS_COUNT)];
    texts = new Wt::WText**[sConfig.GetConfig(CONFIG_REALMS_COUNT)];
    clients = new Wt::Http::Client*[sConfig.GetConfig(CONFIG_REALMS_COUNT)];

    setStyleClass("page statuswidget");

    CreateStatusPage();
}

ServerStatusPage::~ServerStatusPage()
{
    // clears and deletes widgets
    clear();

    // null tables and delete them
    for (int i = 0; i < sConfig.GetConfig(CONFIG_REALMS_COUNT); ++i)
    {
        realms[i] = nullptr;
        clients[i] = nullptr;

        for (int j = SERVER_STATUS_TEXT_REALM; j < SERVER_STATUS_TEXT_SLOT_COUNT; ++j)
            texts[i][j] = nullptr;

        delete [] texts[i];
        texts[i] = nullptr;
    }

    delete [] realms;
    delete [] texts;
    delete [] clients;
}

/********************************************//**
 * \brief Overloads WContainerWidget::refresh() for automatic content change.
 *
 * This function can delete old and create new content.
 * In most situations this is used for content update or language change ;)
 *
 ***********************************************/

void ServerStatusPage::refresh()
{
    if (isHidden() || isDisabled())
        return;

    Wt::WContainerWidget::refresh();
}

/********************************************//**
 * \brief Create server status page and it's widgets.
 *
 * Create widgets for each slot (and additional not sloted widgets) and fills labels with text.
 * This should be done only once per player.
 *
 ***********************************************/

void ServerStatusPage::CreateStatusPage()
{
    Misc::Console(DEBUG_CODE, "Entering %s\n", __FUNCTION__);

    addWidget(new Wt::WText(tr(TXT_INFO_STATUS)));
    for (int i = 0; i < 4; ++i)
        addWidget(new Wt::WBreak());

    int realmsCount = sConfig.GetConfig(CONFIG_REALMS_COUNT);

    for (int i = 0; i < realmsCount; ++i)
    {
        realms[i] = new Wt::WTable();

        texts[i] = new Wt::WText*[SERVER_STATUS_TEXT_SLOT_COUNT];

        for (int j = SERVER_STATUS_TEXT_REALM; j < SERVER_STATUS_TEXT_SLOT_COUNT; ++j)
            texts[i][j] = new Wt::WText();

        #define ADD_WIDGET_TO_REALM(a, b, c) \
               realms[a]->elementAt(b, 0)->addWidget(new Wt::WText(Wt::WString::tr(c)))

        ADD_WIDGET_TO_REALM(i, SERVER_STATUS_TEXT_REALM, TXT_STATUS_REALM);
        ADD_WIDGET_TO_REALM(i, SERVER_STATUS_TEXT_STATE, TXT_STATUS_STATE);
        ADD_WIDGET_TO_REALM(i, SERVER_STATUS_TEXT_ONLINE, TXT_STATUS_ONLINE);
        ADD_WIDGET_TO_REALM(i, SERVER_STATUS_TEXT_MAXONLINE, TXT_STATUS_MAXONLINE);
        ADD_WIDGET_TO_REALM(i, SERVER_STATUS_TEXT_FACTIONS, TXT_STATUS_FACTIONS);
        ADD_WIDGET_TO_REALM(i, SERVER_STATUS_TEXT_UPTIME, TXT_STATUS_UPTIME);
        ADD_WIDGET_TO_REALM(i, SERVER_STATUS_TEXT_REVISION, TXT_STATUS_REV);
        ADD_WIDGET_TO_REALM(i, SERVER_STATUS_TEXT_DIFF, TXT_STATUS_DIFF);
        ADD_WIDGET_TO_REALM(i, SERVER_STATUS_TEXT_AVGDIFF, TXT_STATUS_AVGDIFF);
        ADD_WIDGET_TO_REALM(i, SERVER_STATUS_TEXT_INFO, sConfig.GetRealmInformations(i).additionalInfo);

        // -1 couse we don't want to set value for SERVER_STATUS_TEXT_INFO
        for (int j = SERVER_STATUS_TEXT_REALM; j < SERVER_STATUS_TEXT_SLOT_COUNT-1; ++j)
            realms[i]->elementAt(j, 1)->addWidget(texts[i][j]);

        realms[i]->elementAt(SERVER_STATUS_TEXT_INFO, 0)->setColumnSpan(2);

        addWidget(realms[i]);
        addWidget(new WBreak());
        addWidget(new WBreak());

        clients[i] = new Wt::Http::Client(this);
        clients[i]->setTimeout(15);
        clients[i]->done().connect(boost::bind(&ServerStatusPage::UpdateStatus, this, _1, _2, i));
    }

    RunUpdateStatus();
    timer->start();

    Misc::Console(DEBUG_CODE, "%s End\n", __FUNCTION__);
}

/********************************************//**
 * \brief Updates server status
 *
 * This function parse and updates server status in content.
 *
 ***********************************************/

void ServerStatusPage::RunUpdateStatus()
{
    Misc::Console(DEBUG_CODE, "Entering %s\n", __FUNCTION__);

    if (isHidden() || isDisabled())
        return;

    int realmsCount = sConfig.GetConfig(CONFIG_REALMS_COUNT);

    for (int i = 0; i < realmsCount; ++i)
        clients[i]->get(sConfig.GetRealmInformations(i).statusUrl);

    Misc::Console(DEBUG_CODE, "%s End\n", __FUNCTION__);
}

void ServerStatusPage::UpdateStatus(boost::system::error_code err, const Wt::Http::Message& response, int realmId)
{
    Misc::Console(DEBUG_CODE, "Entering %s\n", __FUNCTION__);

    if (realmId < 0 || realmId > sConfig.GetConfig(CONFIG_REALMS_COUNT))
        return;

    std::istringstream iss;

    if (!err && response.status() == 200)
        iss.str(response.body());
    else
        iss.str("0 0 0 0 0 0 0 0 0 0 0 0 0 0 0");

    std::string online, maxOnline, rev, diff, avgDiff, queue, maxQueue, unk;
    int tmpUp, ally = 0, horde = 0, hordePct = 0, allyPct = 0;

    iss >> tmpUp;
    iss >> online;
    iss >> maxOnline;
    iss >> queue;
    iss >> maxQueue;
    iss >> unk;
    iss >> rev;
    iss >> diff;
    iss >> avgDiff;
    iss >> ally;
    iss >> horde;

    if (ally || horde)
    {
        hordePct = horde/float(ally+horde) * 100;
        allyPct = 100 - hordePct;
    }

    int d, h, m, s;
    d = tmpUp/DAY;
    h = (tmpUp - d*DAY)/HOUR;
    m = (tmpUp - d*DAY - h*HOUR)/MINUTE;
    s = tmpUp - d*DAY - h*HOUR - m*MINUTE;

    WString tmpStr = Wt::WString::fromUTF8(sConfig.GetRealmInformations(realmId).name);

    texts[realmId][SERVER_STATUS_TEXT_REALM]->setText(tmpStr);
    texts[realmId][SERVER_STATUS_TEXT_STATE]->setText(Wt::WString::tr(tmpUp ? TXT_GEN_ONLINE : TXT_GEN_OFFLINE));
    texts[realmId][SERVER_STATUS_TEXT_ONLINE]->setText(online);
    texts[realmId][SERVER_STATUS_TEXT_MAXONLINE]->setText(maxOnline);
    texts[realmId][SERVER_STATUS_TEXT_FACTIONS]->setText(Wt::WString::tr(TXT_STATUS_FACTIONS_FMT).arg(horde).arg(hordePct).arg(ally).arg(allyPct));
    texts[realmId][SERVER_STATUS_TEXT_UPTIME]->setText(Wt::WString::tr(TXT_STATUS_UPTIME_FMT).arg(d).arg(h).arg(m).arg(s));
    texts[realmId][SERVER_STATUS_TEXT_REVISION]->setText(rev);
    texts[realmId][SERVER_STATUS_TEXT_DIFF]->setText(diff);
    texts[realmId][SERVER_STATUS_TEXT_AVGDIFF]->setText(avgDiff);

    Misc::Console(DEBUG_CODE, "%s End\n", __FUNCTION__);
}
