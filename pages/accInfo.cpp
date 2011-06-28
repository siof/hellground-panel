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

AccountInfoPage::AccountInfoPage(SessionInfo * sess, WContainerWidget * parent)
: WContainerWidget(parent)
{
    session = sess;
    setContentAlignment(AlignCenter|AlignTop);
}

void AccountInfoPage::refresh()
{
    if (isHidden() || isDisabled())
        return;

    UpdateTextWidgets();

    ShowAccountInfo();

    WContainerWidget::refresh();
}

void AccountInfoPage::UpdateTextWidgets()
{

}

void AccountInfoPage::ShowAccountInfo()
{
    clear();

    Database * realmDb = new Database(SERVER_DB_DATA, SQL_REALMDB);
    Database * charDb = new Database(SERVER_DB_DATA, SQL_CHARDB);
    DatabaseRow * tmpRow;
                    //          0     1               2                   3        4        5       6
    realmDb->SetPQuery("SELECT id, last_ip, FROM_UNIXTIME(last_login), online, expansion, locale, locked FROM account WHERE id = '%u'", session->accid);

    // there should be only one record in db
    if (realmDb->ExecuteQuery() > 0)
    {
        tmpRow = realmDb->GetRow();
        accInfoText = new WText("<h3>" + session->GetText(TXT_LBL_ACC_INFO) + "</h3>", this);
        addWidget(accInfoText);

        addWidget(new WBreak());
        addWidget(new WBreak());
        addWidget(new WBreak());

        accType = new WText(session->GetText(TXT_LBL_ACC_EXPANSION), this);
        addWidget(accType);
        addWidget(new WText(GetExpansionName(session, tmpRow->fields[4].GetInt())));
        addWidget(new WBreak());
        addWidget(new WBreak());

        currentIP = new WText(session->GetText(TXT_CURRENT_IP), this);
        addWidget(currentIP);
        addWidget(new WText(session->sessionIp));
        addWidget(new WBreak());
        addWidget(new WBreak());

        accCreateDate = new WText(session->GetText(TXT_LBL_ACC_CREATE_DATE), this);
        addWidget(accCreateDate);
        addWidget(new WText(session->joinDate));
        addWidget(new WBreak());

        accLastLoginDate = new WText(session->GetText(TXT_LBL_ACC_LAST_LOGIN), this);
        addWidget(accLastLoginDate);
        addWidget(new WText(tmpRow->fields[2].GetWString()));
        addWidget(new WBreak());

        accLastLoggedIp = new WText(session->GetText(TXT_LBL_ACC_LAST_IP), this);
        addWidget(accLastLoggedIp);
        addWidget(new WText(tmpRow->fields[1].GetWString()));
        addWidget(new WBreak());

        accIPLock = new WText(session->GetText(TXT_LBL_ACC_IP_LOCK), this);
        addWidget(accIPLock);
        addWidget(new WText(tmpRow->fields[6].GetBool() ? session->GetText(TXT_LBL_ACC_IP_LOCK_ON) : session->GetText(TXT_LBL_ACC_IP_LOCK_OFF)));
        addWidget(new WBreak());

        accOnline = new WText(session->GetText(TXT_LBL_ACC_ONLINE), this);
        addWidget(accOnline);
        addWidget(new WText(tmpRow->fields[3].GetBool() ? session->GetText(TXT_IS_ONLINE) : session->GetText(TXT_IS_OFFLINE)));
        addWidget(new WBreak());

        accClientVersion = new WText(session->GetText(TXT_LBL_ACC_CLIENT_VERSION), this);
        addWidget(accClientVersion);
        addWidget(new WText(GetLocale(tmpRow->fields[5].GetInt())));
        addWidget(new WBreak());

        //accVotePoints;

        //accMultiAcc;

        accBan = new WText(session->GetText(TXT_LBL_ACC_BAN), this);
        addWidget(accBan);

        realmDb->SetPQuery("SELECT banreason FROM account_banned WHERE active = 1 AND id = '%u'", session->accid);

        if (realmDb->ExecuteQuery())
            addWidget(new WText(session->GetText(TXT_LBL_BAN_YES) + ": " + realmDb->GetRow()->fields[0].GetWString()));
        else
            addWidget(new WText(session->GetText(TXT_LBL_BAN_NO)));

        addWidget(new WBreak());

        accIPBan = new WText(session->GetText(TXT_LBL_ACC_IP_BAN), this);
        addWidget(accIPBan);

        realmDb->SetPQuery("SELECT banreason FROM ip_banned WHERE ip = '%s'", session->lastIp.toUTF8().c_str());

        if (realmDb->ExecuteQuery())
            addWidget(new WText(session->GetText(TXT_LBL_BAN_YES)));
        else
            addWidget(new WText(session->GetText(TXT_LBL_BAN_NO)));

        addWidget(new WBreak());

        accSessionIPBan = new WText(session->GetText(TXT_CURRENT_IP_BAN));
        addWidget(accSessionIPBan);

        realmDb->SetPQuery("SELECT banreason FROM ip_banned WHERE ip = '%s'", session->sessionIp.toUTF8().c_str());

        if (realmDb->ExecuteQuery())
            addWidget(new WText(session->GetText(TXT_LBL_BAN_YES)));
        else
            addWidget(new WText(session->GetText(TXT_LBL_BAN_NO)));

        addWidget(new WBreak());

//        accCharacters;
//        charName;
//        charClass;
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
            tmpMenu->ShowError(ERROR_EMPTY_QUERY, dbError, true);

        return;
    }

    delete charDb;
    charDb = NULL;
    delete realmDb;
    realmDb = NULL;
}
