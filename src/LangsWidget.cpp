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

#include "LangsWidget.h"

#include <Wt/WApplication>
#include <Wt/WPushButton>

LangsWidget::LangsWidget(Wt::WContainerWidget * parent)
: Wt::WContainerWidget(parent)
{
    AddLangButton(LANG_PL, "pl");
    AddLangButton(LANG_EN, "en");
}

void LangsWidget::AddLangButton(Lang lang, const char * txt)
{
    Wt::WPushButton * tmpBtn;
    tmpBtn = new Wt::WPushButton(this);
    tmpBtn->setText(txt);
    tmpBtn->setStyleClass(txt);
    tmpBtn->clicked().connect(boost::bind(&LangsWidget::ChangeLanguage, this, lang));
}

void LangsWidget::ChangeLanguage(Lang lang)
{
    std::string loc;

    switch (lang)
    {
        case LANG_PL:
            loc = "pl";
            break;
        case LANG_EN:
            loc = "en";
            break;
        case LANG_JA:
            loc = "ja";
            break;
        case LANG_RU:
            loc = "ru";
            break;
        case LANG_CZ:
            loc = "cz";
            break;
        case LANG_IT:
            loc = "it";
            break;
        case LANG_DE:
            loc = "de";
            break;
        case LANG_FR:
            loc = "fr";
            break;
        case LANG_ES:
            loc = "es";
            break;
        case LANG_CS:
            loc = "cs";
            break;
        case LANG_PT:
            loc = "pt";
            break;
        case LANG_ZH:
            loc = "zh";
            break;
        default:
            loc = "en";
            break;
    }

    wApp->setLocale(loc);
}
