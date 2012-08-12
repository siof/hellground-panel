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

    Database db;
    if (db.Connect(PANEL_DB_DATA, SQL_PANELDB))
    {
        if (db.ExecutePQuery("SELECT name, stylePath, tmpltPath FROM Templates") > DB_RESULT_EMPTY)
        {
            std::vector<DatabaseRow *> tmpRows = db.GetRows();
            const DatabaseRow * tmpRow;
            for (std::vector<DatabaseRow*>::const_iterator itr = tmpRows.begin(); itr != tmpRows.end(); ++itr)
            {
                tmpRow = *itr;

                TemplateInfo tmplt;
                tmplt.name = tmpRow->fields[0].GetString();
                tmplt.stylePath = tmpRow->fields[1].GetString();
                tmplt.tmpltPath = tmpRow->fields[2].GetString();

                templates.push_back(tmplt);
                templateCombo->addItem(tmplt.name);
            }
        }
    }

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

    tmpltInfo = Misc::GetTemplateInfoFromDB(templateCombo->currentText());

    if (tmpltInfo.currentTemplate.empty() || tmpltInfo.stylePath.empty())
        return;

    wApp->useStyleSheet(tmpltInfo.GetFullStylePath());
    templt->setTemplateText(tmpltInfo.currentTemplate);
    templt->refresh();

    wApp->setCookie("tmplt", tmpltInfo.name, WEEK);
}
