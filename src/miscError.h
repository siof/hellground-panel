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

#ifndef MISCERROR_H_INCLUDED
#define MISCERROR_H_INCLUDED

#include <Wt/WAnimation>

namespace Misc
{
    namespace Error
    {
        /********************************************//**
         * \brief Shows WMessageBox with given content
         *
         * \param name      text to be shown as box name
         * \param content   text to be shown as box content
         * \param buttons   buttons to be shown
         * \param anim      animation
         * \return id of clicked button
         *
         ***********************************************/

        Wt::StandardButton ShowErrorBox(Wt::WString & name, Wt::WString & content, Wt::WFlags<Wt::StandardButton> buttons, const Wt::WAnimation &anim = Wt::WAnimation());

        /********************************************//**
         * \brief Shows WMessageBox with given name/text localization id's by using ShowErrorBox(...) function
         *
         * \param name  localization id for message box name
         * \param txt   localization id for message box content
         *
         ***********************************************/

        void ShowErrorBoxTr(const char * name, const char * txt);
    }
}

#endif // MISCERROR_H_INCLUDED
