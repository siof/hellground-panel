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

#ifndef TEMPLATE_WIDGET_H_INCLUDED
#define TEMPLATE_WIDGET_H_INCLUDED

#include <Wt/WContainerWidget>

#include "defines.h"

class TemplateWidget : public Wt::WContainerWidget
{
public:
    TemplateWidget(Wt::WTemplate * templ, Wt::WContainerWidget * parent = NULL);
    ~TemplateWidget() {}

private:
    std::vector<TemplateInfo> templates;
    Wt::WComboBox * templateCombo;
    Wt::WTemplate * templt;

    void ChangeTemplate();
};

#endif // TEMPLATE_WIDGET_H_INCLUDED
