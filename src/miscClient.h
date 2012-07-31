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

#ifndef MISCCLIENT_H_INCLUDED
#define MISCCLIENT_H_INCLUDED

namespace Wt
{
    class WString;
}

namespace Misc
{
    namespace Client
    {
        /********************************************//**
         * \brief Returns expansion name.
         *
         * \param sess      player session informations (needed to get expansion name from db)
         * \param index     expansion id
         * \return expansion name from db
         *
         * Returns expansion name for given index.
         * Name is stored in db and loaded on session creation so there is only need to read this information depends on index.
         *
         ***********************************************/

        Wt::WString GetExpansionName(int index);

        /********************************************//**
         * \brief Return client locale name.
         *
         * \param index     locale index
         * \return locale name
         *
         * Returns players client locale depends on locale index.
         *
         ***********************************************/

        Wt::WString GetLocale(int index);
    }
}

#endif // MISCCLIENT_H_INCLUDED
