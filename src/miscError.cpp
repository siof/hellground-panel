/*
*    HG Players Panel - web panel for HellGround server Players
*    Copyright (C) 2011-2012 HellGround Team : Siof, lukaasm,
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

#include "miscError.h"

#include <Wt/WFlags>
#include <Wt/WMessageBox>
#include <Wt/WString>

Wt::StandardButton Misc::Error::ShowErrorBox(Wt::WString & name, Wt::WString & content, Wt::WFlags<Wt::StandardButton> buttons, const Wt::WAnimation &anim)
{
    return Wt::WMessageBox::show(name, content, buttons, anim);
}

void Misc::Error::ShowErrorBoxTr(const char * name, const char * txt)
{
    Wt::WString tmpStrN, tmpStrT;
    tmpStrN = Wt::WString::tr(name);
    tmpStrT = Wt::WString::tr(txt);

    ShowErrorBox(tmpStrN, tmpStrT, Wt::Ok);
}
