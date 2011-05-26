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

#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED

#include "../defines.h"

class ErrorPage : public WContainerWidget
{
public:
    ErrorPage(SessionInfo * sess, WContainerWidget * parent = 0);
    ~ErrorPage() {}

    void refresh();     // overload WWidget::refresh() for automatic content change ;) this should be done for all pages
    void SetErrorMsg(int error);
    void SetAdditionalErrorMsg(const char * str);
    void SetAdditionalErrorMsg(std::string str);
private:
    void ShowError();

    SessionInfo * session;
    std::string errormsg;
    std::string additionalMsg;
};

#endif // ERROR_H_INCLUDED
