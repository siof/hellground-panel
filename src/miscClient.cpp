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

#include "miscClient.h"

#include "defines.h"

Wt::WString Misc::Client::GetExpansionName(int index)
{
    switch (index)
    {
        case EXPANSION_TBC:
            return Wt::WString::tr(TXT_EXPANSION_TBC);
        case EXPANSION_WOTLK:
            return Wt::WString::tr(TXT_EXPANSION_WOTLK);
        case EXPANSION_CATA:
            return Wt::WString::tr(TXT_EXPANSION_CATACLYSM);
        case EXPANSION_PRETBC:
        default:
            return Wt::WString::tr(TXT_EXPANSION_CLASSIC);
    }
}

Wt::WString Misc::Client::GetLocale(int index)
{
    switch (index)
    {
        case 0:
            return Wt::WString::tr("locale.enus");
        case 1:
            return Wt::WString::tr("locale.kokr");
        case 2:
            return Wt::WString::tr("locale.frfr");
        case 3:
            return Wt::WString::tr("locale.dede");
        case 4:
            return Wt::WString::tr("locale.zhcn");
        case 5:
            return Wt::WString::tr("locale.zhtw");
        case 6:
            return Wt::WString::tr("locale.eses");
        case 7:
            return Wt::WString::tr("locale.esmx");
        case 8:
            return Wt::WString::tr("locale.ruru");
        default:
            return Wt::WString::tr("unknown");
    }
}
