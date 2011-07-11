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

#include "accInfo.h"
#include "../database.h"
#include "../menu.h"

AccountInfoSlotItem::~AccountInfoSlotItem()
{
    if (label)
        delete label;

    if (widget)
        delete widget;

    label = NULL;
    widget = NULL;
}

void AccountInfoSlotItem::SetLabel(WText * lbl)
{
    if (label)
        delete label;

    label = lbl;
}

void AccountInfoSlotItem::SetWidget(WWidget * wid)
{
    if (widget)
        delete widget;

    widget = wid;
}

void AccountInfoSlotItem::SetAll(WText * txt, WWidget * wid, int br, uint32 txtId)
{
    SetLabel(txt);
    SetWidget(wid);
    SetBreakCount(br);
    SetTextId(txtId);
}

void AccountInfoSlotItem::UpdateLabel(SessionInfo * sess)
{
    if (!label || !sess)
        return;

    label->setText(sess->GetText(textId));
}


AccountInfoPage::AccountInfoPage(SessionInfo * sess, WContainerWidget * parent) :
    WContainerWidget(parent)
{
    session = sess;
    setContentAlignment(AlignCenter|AlignTop);
    needInfoCreation = true;
}

AccountInfoPage::~AccountInfoPage()
{
    session = NULL;
}

void AccountInfoPage::refresh()
{
    if (isHidden() || isDisabled())
        return;

    if (needInfoCreation)
        CreateAccountInfo();
    else
    {
        UpdateTextWidgets();
        UpdateAccountInfo();
    }

    WContainerWidget::refresh();
}

void AccountInfoPage::UpdateTextWidgets()
{
    for (int i = 0; i < ACCINFO_SLOT_COUNT; ++i)
        accInfoSlots[i].UpdateLabel(session);
}

void AccountInfoPage::UpdateAccountInfo()
{
    Database * realmDb = new Database(SERVER_DB_DATA, SQL_REALMDB);
    Database * charDb = new Database(SERVER_DB_DATA, SQL_CHARDB);
    DatabaseRow * tmpRow;
                    //          0     1         2         3        4        5       6
    realmDb->SetPQuery("SELECT id, last_ip, last_login, online, expansion, locale, locked FROM account WHERE id = '%u'", session->accid);

    // there should be only one record in db
    if (realmDb->ExecuteQuery() > 0)
    {
        tmpRow = realmDb->GetRow();

        WWidget * tmpWidget = NULL;

        tmpWidget = accInfoSlots[ACCINFO_SLOT_TYPE].GetWidget();
        ((WText*)tmpWidget)->setText(GetExpansionName(session, tmpRow->fields[4].GetInt()));

        tmpWidget = accInfoSlots[ACCINFO_SLOT_LAST_LOGIN_DATE].GetWidget();
        ((WText*)tmpWidget)->setText(tmpRow->fields[2].GetWString());

        tmpWidget = accInfoSlots[ACCINFO_SLOT_LAST_LOGGED_IP].GetWidget();
        ((WText*)tmpWidget)->setText(tmpRow->fields[1].GetWString());

        tmpWidget = accInfoSlots[ACCINFO_SLOT_IP_LOCK].GetWidget();
        ((WText*)tmpWidget)->setText(tmpRow->fields[6].GetBool() ? session->GetText(TXT_LBL_ACC_IP_LOCK_ON) : session->GetText(TXT_LBL_ACC_IP_LOCK_OFF));

        tmpWidget = accInfoSlots[ACCINFO_SLOT_ONLINE].GetWidget();
        ((WText*)tmpWidget)->setText(tmpRow->fields[3].GetBool() ? session->GetText(TXT_IS_ONLINE) : session->GetText(TXT_IS_OFFLINE));

        tmpWidget = accInfoSlots[ACCINFO_SLOT_CLIENT_VERSION].GetWidget();
        ((WText*)tmpWidget)->setText(GetLocale(tmpRow->fields[5].GetInt()));

/*
        tmpWidget = accInfoSlots[ACCINFO_SLOT_VOTE_POINTS].GetWidget();
        ((WText*)tmpWidget)->setText();

        tmpWidget = accInfoSlots[ACCINFO_SLOT_MULTIACC].GetWidget();
        ((WText*)tmpWidget)->setText();
*/

        tmpWidget = accInfoSlots[ACCINFO_SLOT_ACC_BAN].GetWidget();
        realmDb->SetPQuery("SELECT banreason FROM account_banned WHERE active = 1 AND id = '%u'", session->accid);
        if (realmDb->ExecuteQuery())
            ((WText*)tmpWidget)->setText(session->GetText(TXT_LBL_BAN_YES) + ": " + realmDb->GetRow()->fields[0].GetWString());
        else
            ((WText*)tmpWidget)->setText(session->GetText(TXT_LBL_BAN_NO));

        tmpWidget = accInfoSlots[ACCINFO_SLOT_LAST_IP_BAN].GetWidget();
        realmDb->SetPQuery("SELECT banreason FROM ip_banned WHERE ip = '%s'", session->lastIp.toUTF8().c_str());
        if (realmDb->ExecuteQuery())
            ((WText*)tmpWidget)->setText(session->GetText(TXT_LBL_BAN_YES));
        else
            ((WText*)tmpWidget)->setText(session->GetText(TXT_LBL_BAN_NO));

        tmpWidget = accInfoSlots[ACCINFO_SLOT_CURR_IP_BAN].GetWidget();
        realmDb->SetPQuery("SELECT banreason FROM ip_banned WHERE ip = '%s'", session->sessionIp.toUTF8().c_str());
        if (realmDb->ExecuteQuery())
            ((WText*)tmpWidget)->setText(session->GetText(TXT_LBL_BAN_YES));
        else
            ((WText*)tmpWidget)->setText(session->GetText(TXT_LBL_BAN_NO));
    }

    delete charDb;
    charDb = NULL;
    delete realmDb;
    realmDb = NULL;
}

void AccountInfoPage::CreateAccountInfo()
{
    #ifdef DEBUG
    printf("\nCall void AccountInfoPage::CreateAccountInfo()\n");
    #endif
    clear();

    Database * realmDb = new Database(SERVER_DB_DATA, SQL_REALMDB);
    Database * charDb = new Database(SERVER_DB_DATA, SQL_CHARDB);
    DatabaseRow * tmpRow;
                    //          0     1         2         3        4        5       6
    realmDb->SetPQuery("SELECT id, last_ip, last_login, online, expansion, locale, locked FROM account WHERE id = '%u'", session->accid);

    // there should be only one record in db
    if (realmDb->ExecuteQuery() > 0)
    {
        #ifdef DEBUG
        printf("\nCreateAccountInfo(): Account founded\n");
        #endif
        needInfoCreation = false;
        tmpRow = realmDb->GetRow();

        WText * tmpTxt = NULL;
        WWidget * tmpWidget = NULL;

        tmpTxt = new WText(session->GetText(TXT_LBL_ACC_INFO));
        accInfoSlots[ACCINFO_SLOT_INFO].SetAll(tmpTxt, NULL, 3, TXT_LBL_ACC_INFO);

        tmpTxt = new WText(session->GetText(TXT_LBL_ACC_EXPANSION));
        tmpWidget = new WText(GetExpansionName(session, tmpRow->fields[4].GetInt()));
        accInfoSlots[ACCINFO_SLOT_TYPE].SetAll(tmpTxt, tmpWidget, 2, TXT_LBL_ACC_EXPANSION);

        tmpTxt = new WText(session->GetText(TXT_CURRENT_IP));
        tmpWidget = new WText(session->sessionIp);
        accInfoSlots[ACCINFO_SLOT_CURRENT_IP].SetAll(tmpTxt, tmpWidget, 2, TXT_CURRENT_IP);

        tmpTxt = new WText(session->GetText(TXT_LBL_ACC_CREATE_DATE));
        tmpWidget = new WText(session->joinDate);
        accInfoSlots[ACCINFO_SLOT_CREATE_DATE].SetAll(tmpTxt, tmpWidget, 1, TXT_LBL_ACC_CREATE_DATE);

        tmpTxt = new WText(session->GetText(TXT_LBL_ACC_LAST_LOGIN));
        tmpWidget = new WText(tmpRow->fields[2].GetWString());
        accInfoSlots[ACCINFO_SLOT_LAST_LOGIN_DATE].SetAll(tmpTxt, tmpWidget, 1, TXT_LBL_ACC_LAST_LOGIN);

        tmpTxt = new WText(session->GetText(TXT_LBL_ACC_LAST_IP));
        tmpWidget = new WText(tmpRow->fields[1].GetWString());
        accInfoSlots[ACCINFO_SLOT_LAST_LOGGED_IP].SetAll(tmpTxt, tmpWidget, 1, TXT_LBL_ACC_LAST_IP);

        tmpTxt = new WText(session->GetText(TXT_LBL_ACC_IP_LOCK));
        tmpWidget = new WText(tmpRow->fields[6].GetBool() ? session->GetText(TXT_LBL_ACC_IP_LOCK_ON) : session->GetText(TXT_LBL_ACC_IP_LOCK_OFF));
        accInfoSlots[ACCINFO_SLOT_IP_LOCK].SetAll(tmpTxt, tmpWidget, 1, TXT_LBL_ACC_IP_LOCK);

        tmpTxt = new WText(session->GetText(TXT_LBL_ACC_ONLINE));
        tmpWidget = new WText(tmpRow->fields[3].GetBool() ? session->GetText(TXT_IS_ONLINE) : session->GetText(TXT_IS_OFFLINE));
        accInfoSlots[ACCINFO_SLOT_ONLINE].SetAll(tmpTxt, tmpWidget, 1, TXT_LBL_ACC_ONLINE);

        tmpTxt = new WText(session->GetText(TXT_LBL_ACC_CLIENT_VERSION));
        tmpWidget = new WText(GetLocale(tmpRow->fields[5].GetInt()));
        accInfoSlots[ACCINFO_SLOT_CLIENT_VERSION].SetAll(tmpTxt, tmpWidget, 1, TXT_LBL_ACC_CLIENT_VERSION);

        //accVotePoints;

        //accMultiAcc;


        tmpTxt = new WText(session->GetText(TXT_LBL_ACC_BAN));

        realmDb->SetPQuery("SELECT banreason FROM account_banned WHERE active = 1 AND id = '%u'", session->accid);
        if (realmDb->ExecuteQuery())
            tmpWidget = new WText(session->GetText(TXT_LBL_BAN_YES) + ": " + realmDb->GetRow()->fields[0].GetWString());
        else
            tmpWidget = new WText(session->GetText(TXT_LBL_BAN_NO));

        accInfoSlots[ACCINFO_SLOT_ACC_BAN].SetAll(tmpTxt, tmpWidget, 1, TXT_LBL_ACC_BAN);


        tmpTxt = new WText(session->GetText(TXT_LBL_ACC_IP_BAN));

        realmDb->SetPQuery("SELECT banreason FROM ip_banned WHERE ip = '%s'", session->lastIp.toUTF8().c_str());
        if (realmDb->ExecuteQuery())
            tmpWidget = new WText(session->GetText(TXT_LBL_BAN_YES));
        else
            tmpWidget = new WText(session->GetText(TXT_LBL_BAN_NO));

        accInfoSlots[ACCINFO_SLOT_LAST_IP_BAN].SetAll(tmpTxt, tmpWidget, 1, TXT_LBL_ACC_IP_BAN);


        tmpTxt = new WText(session->GetText(TXT_CURRENT_IP_BAN));

        realmDb->SetPQuery("SELECT banreason FROM ip_banned WHERE ip = '%s'", session->sessionIp.toUTF8().c_str());
        if (realmDb->ExecuteQuery())
            tmpWidget = new WText(session->GetText(TXT_LBL_BAN_YES));
        else
            tmpWidget = new WText(session->GetText(TXT_LBL_BAN_NO));

        accInfoSlots[ACCINFO_SLOT_CURR_IP_BAN].SetAll(tmpTxt, tmpWidget, 1, TXT_CURRENT_IP_BAN);

//        accCharacters;
//        charName;
//        charClass;


        int tmpCount;

        #ifdef DEBUG
        printf("\nCreateAccountInfo(): ACC_INFO_SLOT_COUNT: %i\n", ACCINFO_SLOT_COUNT);
        #endif

        // add widgets to page
        for (int i = 0; i < ACCINFO_SLOT_COUNT; ++i)
        {
            tmpWidget = accInfoSlots[i].GetLabel();
            if (tmpWidget)
                addWidget(tmpWidget);

            tmpWidget = accInfoSlots[i].GetWidget();
            if (tmpWidget)
                addWidget(tmpWidget);

            tmpCount = accInfoSlots[i].GetBreakCount();

            #ifdef DEBUG
            printf("\nCreateAccountInfo(): i: %i, tmpCount: %i\n", i, tmpCount);
            #endif
            for (int j = 0; j < tmpCount; ++j)
                addWidget(new WBreak());
        }
    }
    else
    {
        // this should be replaced with something better ...
        HGMenu * tmpMenu = (HGMenu*)(parent()->parent());

        std::string dbError = realmDb->GetError();

        delete charDb;
        charDb = NULL;
        delete realmDb;
        realmDb = NULL;

        if (tmpMenu)
            tmpMenu->ShowError(ERROR_SLOT_DB, dbError);

        return;
    }

    delete charDb;
    charDb = NULL;
    delete realmDb;
    realmDb = NULL;
}
