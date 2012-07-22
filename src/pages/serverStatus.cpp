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

#include <curl/curl.h>
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
    setContentAlignment(AlignCenter|AlignTop);

    timer = new Wt::WTimer();
    timer->setInterval(60000);
    timer->timeout().connect(this, &ServerStatusPage::UpdateStatus);

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
    }

    UpdateStatus();
    timer->start();
}

/********************************************//**
 * \brief Updates server status
 *
 * This function parse and updates server status in content.
 *
 ***********************************************/

size_t writebuffer( char *ptr, size_t /*size*/, size_t /*nmemb*/, char * userdata)
{
    strcpy(userdata, ptr);

    return strlen(userdata);
}

void ServerStatusPage::UpdateStatus()
{
    if (isHidden() || isDisabled())
        return;

    CURL *curl;
    CURLcode results;

    curl = curl_easy_init();

    if (curl)
    {
        for (int i = 0; i < REALMS_COUNT; ++i)
        {
            char * buffer = new char[256];
            curl_easy_setopt(curl, CURLOPT_URL, realmsInfo[i][REALM_INFO_STATUS_URL]);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writebuffer);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, MAX_STATUS_WAIT_TIME);

            results = curl_easy_perform(curl);

            if (results != CURLE_OK)
            {
                for (int i = 0; i < 20; ++i)
                {
                    if (i%2)
                        buffer[i] = '0';
                    else
                        buffer[i] = ' ';
                }
            }

            std::istringstream iss(buffer);
            delete [] buffer;
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
//            iss >> horde; // core support needed
//            iss >> ally;  // core support needed

            int d, h, m, s;
            d = tmpUp/DAY;
            h = (tmpUp - d*DAY)/HOUR;
            m = (tmpUp - d*DAY - h*HOUR)/MINUTE;
            s = tmpUp - d*DAY - h*HOUR - m*MINUTE;

            ((Wt::WText*)realms[i]->elementAt(SERVER_STATUS_TEXT_REALM, 1)->widget(0))->setText(realmsInfo[i][REALM_INFO_NAME]);
            ((Wt::WText*)realms[i]->elementAt(SERVER_STATUS_TEXT_STATE, 1)->widget(0))->setText(tr(tmpUp ? TXT_GEN_ONLINE : TXT_GEN_OFFLINE));
            ((Wt::WText*)realms[i]->elementAt(SERVER_STATUS_TEXT_ONLINE, 1)->widget(0))->setText(online);
            ((Wt::WText*)realms[i]->elementAt(SERVER_STATUS_TEXT_MAXONLINE, 1)->widget(0))->setText(maxOnline);
            ((Wt::WText*)realms[i]->elementAt(SERVER_STATUS_TEXT_FACTIONS, 1)->widget(0))->setText(tr(TXT_STATUS_FACTIONS_FMT).arg(horde).arg(ally));
            ((Wt::WText*)realms[i]->elementAt(SERVER_STATUS_TEXT_UPTIME, 1)->widget(0))->setText(tr(TXT_STATUS_UPTIME_FMT).arg(d).arg(h).arg(m).arg(s));
            ((Wt::WText*)realms[i]->elementAt(SERVER_STATUS_TEXT_REVISION, 1)->widget(0))->setText(rev);
            ((Wt::WText*)realms[i]->elementAt(SERVER_STATUS_TEXT_DIFF, 1)->widget(0))->setText(diff);
            ((Wt::WText*)realms[i]->elementAt(SERVER_STATUS_TEXT_AVGDIFF, 1)->widget(0))->setText(avgDiff);
//            ((Wt::WText*)realms[i]->elementAt(SERVER_STATUS_TEXT_INFO, 1)->widget(0))->setText();
        }

        curl_easy_cleanup(curl);
    }
}
