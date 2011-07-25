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

#ifndef REGISTER_H_INCLUDED
#define REGISTER_H_INCLUDED

#include "../defines.h"
#include "../textItems.h"
#include <WCheckBox>

enum RegistrationTextSlots
{
    REG_TEXT_MAIN = 0,
    REG_TEXT_INFO,
    REG_TEXT_LOGIN,
    REG_TEXT_EMAIL,
    REG_TEXT_RULES,
//    REG_TEXT_RULES_ACCEPT,

    REG_TEXT_SLOT_COUNT
};

class RegisterPage : public WContainerWidget
{
public:
    RegisterPage(SessionInfo * sess, WContainerWidget * parent = 0);
    ~RegisterPage();

    void refresh();     // overload WWidget::refresh() for automatic content change ;) this should be done for all pages
private:
    SessionInfo * session;
    bool needCreation;

    WLineEdit * txtLogin;
    WLineEdit * txtEmail;
    WPushButton * btnRegister;
    WCheckBox * chRules;

    TextItems textSlots[REG_TEXT_SLOT_COUNT];

    void UpdateTextWidgets();
    void CreateRegisterPage();

    void ClearLogin();
    void ClearEmail();
    void CheckChange();
    void Register();
};


#endif // REGISTER_H_INCLUDED
