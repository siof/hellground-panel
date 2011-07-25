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

#include "register.h"
#include "../database.h"

RegisterPage::RegisterPage(SessionInfo * sess, WContainerWidget * parent):
    WContainerWidget(parent), session(sess), needCreation(true)
{
    setContentAlignment(AlignCenter|AlignTop);
}

RegisterPage::~RegisterPage()
{

}

void RegisterPage::refresh()
{
    #ifdef DEBUG
    printf("RegisterPage::refresh()");
    #endif

    if (needCreation)
        CreateRegisterPage();
    else
        UpdateTextWidgets();

    WContainerWidget::refresh();
}

void RegisterPage::UpdateTextWidgets()
{
    for (int i = 0; i < REG_TEXT_SLOT_COUNT; ++i)
        textSlots[i].UpdateLabel(session);

    chRules->setText(session->GetText(TXT_LBL_REGISTER_RULES_ACCEPT));
    btnRegister->setText(session->GetText(TXT_BTN_REGISTER));
}

void RegisterPage::CreateRegisterPage()
{
    clear();
    needCreation = false;

    textSlots[REG_TEXT_MAIN].SetLabel(session, TXT_LBL_REGISTER_MAIN);
    addWidget(textSlots[REG_TEXT_MAIN].GetLabel());
    addWidget(new WBreak());
    addWidget(new WBreak());
    addWidget(new WBreak());
    addWidget(new WBreak());

    textSlots[REG_TEXT_INFO].SetLabel("");
    addWidget(textSlots[REG_TEXT_INFO].GetLabel());
    addWidget(new WBreak());
    addWidget(new WBreak());

    textSlots[REG_TEXT_LOGIN].SetLabel(session, TXT_LBL_ACC_LOGIN);
    txtLogin = new WLineEdit();
    addWidget(textSlots[REG_TEXT_LOGIN].GetLabel());
    addWidget(txtLogin);
    addWidget(new WBreak());

    textSlots[REG_TEXT_EMAIL].SetLabel(session, TXT_LBL_ACC_MAIL);
    txtEmail = new WLineEdit();
    addWidget(textSlots[REG_TEXT_EMAIL].GetLabel());
    addWidget(txtEmail);
    addWidget(new WBreak());

    textSlots[REG_TEXT_RULES].SetLabel(session, TXT_LBL_REGISTER_RULES);
    addWidget(textSlots[REG_TEXT_RULES].GetLabel());
    addWidget(new WBreak());

//    textSlots[REG_TEXT_RULES_ACCEPT].SetLabel(session, TXT_LBL_REGISTER_RULES_ACCEPT);
    chRules = new WCheckBox(session->GetText(TXT_LBL_REGISTER_RULES_ACCEPT));
//    addWidget(textSlots[REG_TEXT_RULES_ACCEPT].GetLabel());
    addWidget(chRules);
    addWidget(new WBreak());
    addWidget(new WBreak());

    btnRegister = new WPushButton(session->GetText(TXT_BTN_REGISTER));
    btnRegister->setEnabled(false);
    addWidget(btnRegister);

    txtLogin->focussed().connect(this, &RegisterPage::ClearLogin);
    txtEmail->focussed().connect(this, &RegisterPage::ClearEmail);
    chRules->changed().connect(this, &RegisterPage::CheckChange);
    btnRegister->clicked().connect(this, &RegisterPage::Register);
}

void RegisterPage::ClearLogin()
{
    txtLogin->setText(WString(""));
}

void RegisterPage::ClearEmail()
{
    txtEmail->setText(WString(""));
}

void RegisterPage::CheckChange()
{
    btnRegister->setEnabled(chRules->isChecked());
}

void RegisterPage::Register()
{
    if (!chRules->isChecked())
    {

        return;
    }

    WString login, mail, pass;

    login = txtLogin->text();
    mail = txtEmail->text();

    pass = "";

    int passLen = irand(PASSWORD_LENGTH_MIN, PASSWORD_LENGTH_MAX);

    std::string tmpStr;

    for (int i = 0; i < passLen; ++i)
        tmpStr += (char)(irand(PASSWORD_ANSI_START, PASSWORD_ANSI_END));

    pass = WString::fromUTF8(tmpStr);

    WString from, msg;
    from = MAIL_FROM;

    Database * db = new Database(SERVER_DB_DATA, SQL_REALMDB);

    login = db->EscapeString(login);
    pass = db->EscapeString(pass);

    // check should be moved to other place but here will be usefull for SendMail tests ;)
    #ifdef REGISTRATION_ENABLED
    db->SetPQuery("INSERT INTO account (username, email,sha_pass_hash) VALUES ('%s', '%s', SHA('%s:%s'))", login.toUTF8().c_str(), mail.toUTF8().c_str(), login.toUTF8().c_str(), pass.toUTF8().c_str());
    db->ExecuteQuery();
    #endif

    msg = "test msg :D passs jest taki se:" + pass;

    SendMail(from, mail, msg);

    delete db;
}
