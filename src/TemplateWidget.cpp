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

#include "TemplateWidget.h"

#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WComboBox>
#include <Wt/WPushButton>
#include <Wt/WTemplate>
#include <Wt/WText>

#include "database.h"
#include "misc.h"

TemplateWidget::TemplateWidget(Wt::WTemplate * templ, Wt::WContainerWidget * parent)
: Wt::WContainerWidget(parent)
{
    templt = templ;
    templateCombo = new Wt::WComboBox();

    templates = Misc::GetTemplatesFromDB();

    Wt::WPushButton * tmpButton = new Wt::WPushButton();
    tmpButton->setText(Wt::WString::tr(TXT_BTN_CHANGE_TEMPLATE));
    tmpButton->clicked().connect(this, &TemplateWidget::ChangeTemplate);

    addWidget(new Wt::WText(Wt::WString::tr(TXT_TEMPLATE_CHOOSE)));
    addWidget(new Wt::WBreak());
    addWidget(templateCombo);
    addWidget(new Wt::WBreak());
    addWidget(tmpButton);
}

void TemplateWidget::ChangeTemplate()
{
    if (!templateCombo->count() || templateCombo->currentIndex() < 0 || templateCombo->currentIndex() > templates.size())
        return;

    TemplateInfo tmpltInfo;

    int index = templateCombo->currentIndex();

    std::vector<TemplateInfo>::const_iterator itr = templates.begin();
    std::advance(itr, index);

    // it's probably impossible but ...
    if (itr == templates.end())
        return;

    tmpltInfo = *itr;

    // it's probably impossible but ...
    if (tmpltInfo.name != templateCombo->currentText())
        return;

    if (tmpltInfo.currentTemplate.empty() || tmpltInfo.stylePath.empty())
        return;

    wApp->useStyleSheet(tmpltInfo.GetFullStylePath());
    wApp->setCookie("tmplt", tmpltInfo.name, WEEK);
    templt->setTemplateText(tmpltInfo.currentTemplate);
    templt->refresh();
}
