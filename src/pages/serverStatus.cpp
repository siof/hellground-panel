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
    timer = new Wt::WTimer();
    timer->setInterval(60000);
    timer->timeout().connect(this, &ServerStatusPage::RunUpdateStatus);

    CreateStatusPage();
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
    addWidget(new Wt::WText(tr(TXT_INFO_STATUS)));
    for (int i = 0; i < 4; ++i)
        addWidget(new Wt::WBreak());

    for (int i = 0; i < REALMS_COUNT; ++i)
    {
        realms[i] = new Wt::WTable();

        realms[i]->elementAt(SERVER_STATUS_TEXT_REALM, 0)->addWidget(new Wt::WText(tr(TXT_STATUS_REALM)));
        realms[i]->elementAt(SERVER_STATUS_TEXT_STATE, 0)->addWidget(new Wt::WText(tr(TXT_STATUS_STATE)));
        realms[i]->elementAt(SERVER_STATUS_TEXT_ONLINE, 0)->addWidget(new Wt::WText(tr(TXT_STATUS_ONLINE)));
        realms[i]->elementAt(SERVER_STATUS_TEXT_MAXONLINE, 0)->addWidget(new Wt::WText(tr(TXT_STATUS_MAXONLINE)));
        realms[i]->elementAt(SERVER_STATUS_TEXT_FACTIONS, 0)->addWidget(new Wt::WText(tr(TXT_STATUS_FACTIONS)));
        realms[i]->elementAt(SERVER_STATUS_TEXT_UPTIME, 0)->addWidget(new Wt::WText(tr(TXT_STATUS_UPTIME)));
        realms[i]->elementAt(SERVER_STATUS_TEXT_REVISION, 0)->addWidget(new Wt::WText(tr(TXT_STATUS_REV)));
        realms[i]->elementAt(SERVER_STATUS_TEXT_DIFF, 0)->addWidget(new Wt::WText(tr(TXT_STATUS_DIFF)));
        realms[i]->elementAt(SERVER_STATUS_TEXT_AVGDIFF, 0)->addWidget(new Wt::WText(tr(TXT_STATUS_AVGDIFF)));
        realms[i]->elementAt(SERVER_STATUS_TEXT_INFO, 0)->addWidget(new Wt::WText(tr(realmsInfo[i][REALM_INFO_ADDITIONAL])));

        realms[i]->elementAt(SERVER_STATUS_TEXT_REALM, 1)->addWidget(new Wt::WText(""));
        realms[i]->elementAt(SERVER_STATUS_TEXT_STATE, 1)->addWidget(new Wt::WText(""));
        realms[i]->elementAt(SERVER_STATUS_TEXT_ONLINE, 1)->addWidget(new Wt::WText(""));
        realms[i]->elementAt(SERVER_STATUS_TEXT_MAXONLINE, 1)->addWidget(new Wt::WText(""));
        realms[i]->elementAt(SERVER_STATUS_TEXT_FACTIONS, 1)->addWidget(new Wt::WText(""));
        realms[i]->elementAt(SERVER_STATUS_TEXT_UPTIME, 1)->addWidget(new Wt::WText(""));
        realms[i]->elementAt(SERVER_STATUS_TEXT_REVISION, 1)->addWidget(new Wt::WText(""));
        realms[i]->elementAt(SERVER_STATUS_TEXT_DIFF, 1)->addWidget(new Wt::WText(""));
        realms[i]->elementAt(SERVER_STATUS_TEXT_AVGDIFF, 1)->addWidget(new Wt::WText(""));
//        realms[i]->elementAt(SERVER_STATUS_TEXT_INFO, 1)->addWidget(new WText(""));
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
}

/********************************************//**
 * \brief Updates server status
 *
 * This function parse and updates server status in content.
 *
 ***********************************************/

void ServerStatusPage::RunUpdateStatus()
{
    if (isHidden() || isDisabled())
        return;

    for (int i = 0; i < REALMS_COUNT; ++i)
        clients[i]->get(realmsInfo[i][REALM_INFO_STATUS_URL]);
}

void ServerStatusPage::UpdateStatus(boost::system::error_code err, const Wt::Http::Message& response, int realmId)
{
    if (realmId < 0 || realmId > REALMS_COUNT)
        return;

    std::istringstream iss;

    if (!err && response.status() == 200)
        iss.str(response.body());
    else
        iss.str("0 0 0 0 0 0 0 0 0 0 0 0 0 0 0");

    std::string online, maxOnline, rev, diff, avgDiff, queue, maxQueue, unk;
    int tmpUp, ally = 0, horde = 0;

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
        horde = horde/float(ally+horde) * 100;
        ally = 100 - horde;
    }

    int d, h, m, s;
    d = tmpUp/DAY;
    h = (tmpUp - d*DAY)/HOUR;
    m = (tmpUp - d*DAY - h*HOUR)/MINUTE;
    s = tmpUp - d*DAY - h*HOUR - m*MINUTE;

    ((Wt::WText*)realms[realmId]->elementAt(SERVER_STATUS_TEXT_REALM, 1)->widget(0))->setText(realmsInfo[realmId][REALM_INFO_NAME]);
    ((Wt::WText*)realms[realmId]->elementAt(SERVER_STATUS_TEXT_STATE, 1)->widget(0))->setText(tr(tmpUp ? TXT_GEN_ONLINE : TXT_GEN_OFFLINE));
    ((Wt::WText*)realms[realmId]->elementAt(SERVER_STATUS_TEXT_ONLINE, 1)->widget(0))->setText(online);
    ((Wt::WText*)realms[realmId]->elementAt(SERVER_STATUS_TEXT_MAXONLINE, 1)->widget(0))->setText(maxOnline);
    ((Wt::WText*)realms[realmId]->elementAt(SERVER_STATUS_TEXT_FACTIONS, 1)->widget(0))->setText(tr(TXT_STATUS_FACTIONS_FMT).arg(horde).arg(ally));
    ((Wt::WText*)realms[realmId]->elementAt(SERVER_STATUS_TEXT_UPTIME, 1)->widget(0))->setText(tr(TXT_STATUS_UPTIME_FMT).arg(d).arg(h).arg(m).arg(s));
    ((Wt::WText*)realms[realmId]->elementAt(SERVER_STATUS_TEXT_REVISION, 1)->widget(0))->setText(rev);
    ((Wt::WText*)realms[realmId]->elementAt(SERVER_STATUS_TEXT_DIFF, 1)->widget(0))->setText(diff);
    ((Wt::WText*)realms[realmId]->elementAt(SERVER_STATUS_TEXT_AVGDIFF, 1)->widget(0))->setText(avgDiff);
//    ((Wt::WText*)realms[i]->elementAt(SERVER_STATUS_TEXT_INFO, 1)->widget(0))->setText();
}
