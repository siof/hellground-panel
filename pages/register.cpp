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

    textSlots[REG_TEXT_MAIN].SetLabel(session, TXT_LBL_REGISTER_MAIN);
    addWidget(textSlots[REG_TEXT_MAIN].GetLabel());
    addWidget(new WBreak());
    addWidget(new WBreak());
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
    addWidget(btnRegister);

    txtLogin->focussed().connect(this, &RegisterPage::ClearLogin);
    txtEmail->focussed().connect(this, &RegisterPage::ClearEmail);
    chRules->changed().connect(this, &RegisterPage::CheckChange);
    btnRegister->clicked().connect(this, &RegisterPage::Register);
    btnRegister->disable();
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
        return;

    // ADD REGISTRATION CODE
}
