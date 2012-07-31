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

#include "miscHash.h"

#include <stdarg.h>

#include <boost/algorithm/string/case_conv.hpp>

#include <Wt/WString>
#include <Wt/Utils>

std::string Misc::Hash::GetSHA1(const char * txt, uint32 flags)
{
    std::string tmpStr = txt;

    if (flags & HASH_FLAG_UPPER)
        boost::algorithm::to_upper(tmpStr);

    tmpStr = Wt::Utils::sha1(tmpStr);

    if (!flags & HASH_FLAG_BIN)
        tmpStr = Wt::Utils::hexEncode(tmpStr);

    return tmpStr;
}

Wt::WString Misc::Hash::WGetSHA1(const char * txt, uint32 flags)
{
    return Wt::WString::fromUTF8(GetSHA1(txt, flags));
}

std::string Misc::Hash::PGetSHA1(const char * format, uint32 flags, ...)
{
    char buffer[strlen(format) + 1000];

    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);

    return GetSHA1(buffer, flags);
}

Wt::WString Misc::Hash::PWGetSHA1(const char * format, uint32 flags, ...)
{
    char buffer[strlen(format) + 1000];

    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);

    return WGetSHA1(buffer, flags);
}
