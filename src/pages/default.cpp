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

#include "default.h"

DefaultPage::DefaultPage(SessionInfo * sess, WContainerWidget * parent)
: WContainerWidget(parent)
{
    session = sess;
    ShowText();
}

void DefaultPage::refresh()
{
    if (isHidden() || isDisabled())
        return;

    ShowText();

    WContainerWidget::refresh();
}

void DefaultPage::ShowText()
{
    clear();

    addWidget(new WText(session->GetText(TXT_SERVER_INFO), this));
}
