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

/********************************************//**
 * \addtogroup Pages
 * \{
 *
 * \addtogroup Server status page
 * \{
 *
 * \file serverStatus.cpp
 * This file contains code needed to parse and show server status.
 *
 ***********************************************/

#include "serverStatus.h"
#include <curl/curl.h>
#include <iostream>
#include <sstream>

/********************************************//**
 * \brief Creates new ServerStatusPage object.
 *
 * \param sess      Contains current session informations.
 * \param parent    Parent container type object in which this container should be placed.
 *
 ***********************************************/

ServerStatusPage::ServerStatusPage(SessionInfo * sess, WContainerWidget * parent)
: WContainerWidget(parent), session(sess), needCreation(true)
{
    setContentAlignment(AlignCenter|AlignTop);
    timer = new WTimer();
    timer->setInterval(60000);
    timer->timeout().connect(this, &ServerStatusPage::UpdateStatus);
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

    if (needCreation)
        CreateStatusPage();
    else
    {
        UpdateLabels();
        UpdateStatus();
    }

    WContainerWidget::refresh();
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
    clear();
    needCreation = false;

    WWidget * tmpWidget = NULL;

    statusSlots[SERVER_STATUS_TEXT_MAIN].SetAll(session, TXT_LBL_STATUS_MAIN, NULL, 4);

    tmpWidget = new WText("");
    statusSlots[SERVER_STATUS_TEXT_REALM].SetAll(session, TXT_LBL_STATUS_REALM, tmpWidget, 1);

    tmpWidget = new WText("");
    statusSlots[SERVER_STATUS_TEXT_STATE].SetAll(session, TXT_LBL_STATUS_STATE, tmpWidget, 1);

    tmpWidget = new WText("");
    statusSlots[SERVER_STATUS_TEXT_ONLINE].SetAll(session, TXT_LBL_STATUS_ONLINE, tmpWidget, 1);

    tmpWidget = new WText("");
    statusSlots[SERVER_STATUS_TEXT_MAXONLINE].SetAll(session, TXT_LBL_STATUS_MAXONLINE, tmpWidget, 1);

    tmpWidget = new WText("");
    statusSlots[SERVER_STATUS_TEXT_FACTIONS].SetAll(NULL, tmpWidget, 1, 0);

    tmpWidget = new WText("");
    statusSlots[SERVER_STATUS_TEXT_UPTIME].SetAll(session, TXT_LBL_STATUS_UPTIME, tmpWidget, 1);

    tmpWidget = new WText("");
    statusSlots[SERVER_STATUS_TEXT_REVISION].SetAll(session, TXT_LBL_STATUS_REV, tmpWidget, 1);

    tmpWidget = new WText("");
    statusSlots[SERVER_STATUS_TEXT_DIFF].SetAll(session, TXT_LBL_STATUS_DIFF, tmpWidget, 1);

    tmpWidget = new WText("");
    statusSlots[SERVER_STATUS_TEXT_AVGDIFF].SetAll(session, TXT_LBL_STATUS_AVGDIFF, tmpWidget, 1);

    tmpWidget = new WText("");
    statusSlots[SERVER_STATUS_TEXT_INFO].SetAll(session, TXT_LBL_STATUS_INFO, tmpWidget, 1);

    int tmpCount;

    // add widgets to page
    for (int i = 0; i < SERVER_STATUS_TEXT_SLOT_COUNT; ++i)
    {
        tmpWidget = statusSlots[i].GetLabel();
        if (tmpWidget)
            addWidget(tmpWidget);

        tmpWidget = statusSlots[i].GetWidget();
        if (tmpWidget)
            addWidget(tmpWidget);

        tmpCount = statusSlots[i].GetBreakCount();

        #ifdef DEBUG
        printf("\nCreateStatusPage(): i: %i, tmpCount: %i\n", i, tmpCount);
        #endif
        for (int j = 0; j < tmpCount; ++j)
            addWidget(new WBreak());
    }

    UpdateStatus();
    timer->start();
}

/********************************************//**
 * \brief Updates server status labels
 *
 * This function updates server status labels depends on language.
 *
 ***********************************************/

void ServerStatusPage::UpdateLabels()
{
    for (int i = 0; i < SERVER_STATUS_TEXT_SLOT_COUNT; ++i)
        statusSlots[i].UpdateLabel(session);
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

    if(curl)
    {
        char * buffer = new char[256];
        curl_easy_setopt(curl, CURLOPT_URL, STATUS_SOURCE);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writebuffer);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);    // max 10 seconds for status download

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
        uint32 tmpUp, ally = 0, horde = 0;

        iss >> tmpUp;
        iss >> online;
        iss >> maxOnline;
        iss >> queue;
        iss >> maxQueue;
        iss >> unk;
        iss >> rev;
        iss >> diff;
        iss >> avgDiff;
//        iss >> horde; // core support needed
//        iss >> ally;  // core support needed

        uint32 d, h, m, s;
        d = tmpUp/(3600*24);
        h = (tmpUp - d*3600*24)/3600;
        m = (tmpUp - d*3600*24 - h*3600)/60;
        s = tmpUp - d*3600*24 - h*3600 - m*60;

        buffer = new char[100];

        sprintf(buffer, session->GetText(TXT_UPTIME_FMT).toUTF8().c_str(), d, h, m, s);
        std::string uptime = buffer;

        delete [] buffer;

        buffer = new char[100];

        sprintf(buffer, session->GetText(TXT_LBL_STATUS_FACTIONS_FMT).toUTF8().c_str(), horde, ally);

        ((WText*)statusSlots[SERVER_STATUS_TEXT_REALM].GetWidget())->setText(REALM_NAME);
        ((WText*)statusSlots[SERVER_STATUS_TEXT_STATE].GetWidget())->setText(session->GetText(tmpUp ? TXT_ONLINE : TXT_OFFLINE));
        ((WText*)statusSlots[SERVER_STATUS_TEXT_ONLINE].GetWidget())->setText(online);
        ((WText*)statusSlots[SERVER_STATUS_TEXT_MAXONLINE].GetWidget())->setText(maxOnline);
        ((WText*)statusSlots[SERVER_STATUS_TEXT_FACTIONS].GetWidget())->setText(buffer);
        ((WText*)statusSlots[SERVER_STATUS_TEXT_UPTIME].GetWidget())->setText(uptime);
        ((WText*)statusSlots[SERVER_STATUS_TEXT_REVISION].GetWidget())->setText(rev);
        ((WText*)statusSlots[SERVER_STATUS_TEXT_DIFF].GetWidget())->setText(diff);
        ((WText*)statusSlots[SERVER_STATUS_TEXT_AVGDIFF].GetWidget())->setText(avgDiff);
//        ((WText*)statusSlots[SERVER_STATUS_TEXT_INFO].GetWidget())->setText();

        delete [] buffer;
        curl_easy_cleanup(curl);
    }
}
