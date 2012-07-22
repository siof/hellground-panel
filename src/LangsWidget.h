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

#include <Wt/WContainerWidget>

#include "defines.h"

class LangsWidget : public Wt::WContainerWidget
{
public:
    LangsWidget(Wt::WContainerWidget * parent = NULL);
    ~LangsWidget() {}

private:
    void ChangeLanguage(Lang lang);

    void AddLangButton(Lang lang, const char * txt);
};
