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

#include "error.h"

ErrorPage::ErrorPage(SessionInfo * sess, WContainerWidget * parent)
: WContainerWidget(parent)
{
    session = sess;
    ShowError();
}

void ErrorPage::refresh()
{
    ShowError();

    WContainerWidget::refresh();
}

void ErrorPage::SetErrorMsg(int error)
{
    switch (error)
    {
        case ERROR_DB_CONNECT:
            switch (session->language)
            {
                case LANG_EN:
                    errormsg = "Can't connect to database !";
                    break;
                case LANG_PL:
                default:
                    errormsg = "Błąd połączenia z bazą danych !";
                    break;
            }
            break;
        case ERROR_LOGIN:
            switch (session->language)
            {
                case LANG_EN:
                    errormsg = "Invalid login or password";
                    break;
                case LANG_PL:
                default:
                    errormsg = "Błędny login lub hasło";
                    break;
            }
            break;
        default:
            errormsg = "";
            break;
    }

    ShowError();
}

void ErrorPage::SetAdditionalErrorMsg(const char * str)
{
    if (!str)
        return;

    additionalMsg = str;
}

void ErrorPage::SetAdditionalErrorMsg(std::string str)
{
    SetAdditionalErrorMsg(str.c_str());
}

void ErrorPage::ShowError()
{
    if (count())
    {
        for (int i = count() - 1; i >= 0; --i)
        {
            WWidget * tmp = widget(i);
            removeWidget(tmp);
            delete tmp;
            tmp = NULL;
        }
    }

    addWidget(new WText(utf8(errormsg)));
    addWidget(new WBreak());
    addWidget(new WText(utf8(additionalMsg)));
}
