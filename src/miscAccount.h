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

#ifndef MISCACCOUNT_H_INCLUDED
#define MISCACCOUNT_H_INCLUDED

#include "defines.h"

namespace Misc
{
    namespace Account
    {
        void AddActivity(uint32 accountId, const char * ip, const char * activity, const char * activityArgs);
        void AddActivity(uint32 accountId, const std::string & ip, const char * activity, const std::string & activityArgs);
        void AddActivity(const char * username, const char * ip, const char * activity, const char * activityArgs, bool escape = false);
        void AddActivity(const std::string & username, const std::string & ip, const char * activity, const std::string & activityArgs, bool escape = false);

        std::string GeneratePassword();
    }
}

#endif // MISCACCOUNT_H_INCLUDED
