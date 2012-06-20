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

#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED

#include "../defines.h"

class ErrorPageSlot
{
public:
    ErrorPageSlot() : text(NULL), textId(""), str(WString::fromUTF8("")), textIdBased(false) {}
    ~ErrorPageSlot();

    void SetText(WText * txt, const char * id);
    void SetText(WText * txt, WString &strn);
    void SetText(WString &strn);
    void SetText(SessionInfo * sess, const char * id);

    WText * CreateText(SessionInfo * sess);
    void DeleteText();
    void UpdateText(SessionInfo * sess);

    WText * GetText() { return text; }

    void Clear()
    {
        DeleteText();
        textId = "";
        str = "";
        textIdBased = false;
    }
private:
    WText * text;
    std::string textId;
    WString str;
    bool textIdBased;
};

class ErrorPage : public WContainerWidget
{
public:
    ErrorPage(SessionInfo * sess, WContainerWidget * parent = 0);
    ~ErrorPage() {}

    void refresh();     // overload WWidget::refresh() for automatic content change ;) this should be done for all pages
    void SetErrorMsg(ErrorSlots slot, const char * txtId);
    void SetErrorMsg(ErrorSlots slot, WString &str);
    void ClearSlots();
private:
    void CreateErrors();
    void UpdateErrors();

    SessionInfo * session;
    ErrorPageSlot errors[ERROR_SLOT_COUNT];
    bool created;
};

#endif // ERROR_H_INCLUDED
