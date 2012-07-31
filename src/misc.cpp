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

#include "misc.h"

#include <stdarg.h>

#include <jwsmtp/jwsmtp.h>
#include <Wt/WApplication>

void Misc::SendMail(const Wt::WString& from, const Wt::WString& to, const Wt::WString& sub, const Wt::WString& msg)
{
    jwsmtp::mailer mail;

    mail.addrecipient(to.toUTF8().c_str());
    mail.setsender(from.toUTF8().c_str());
    mail.setsubject(sub.toUTF8().c_str());
    mail.setmessage(msg.toUTF8().c_str());

    mail.setserver(MAIL_HOST);
    mail.username(MAIL_USER);
    mail.password(MAIL_PASS);

    mail.send();
}

void Misc::SendMailTr(const char * from, const char * to, const char * sub, const char * msg)
{
    Wt::WString fromStr, toStr, subStr, msgStr;
    fromStr = from;
    toStr = to;
    subStr = Wt::WString::tr(sub);
    msgStr = Wt::WString::tr(msg);

    SendMail(fromStr, toStr, subStr, msgStr);
}

int Misc::Irand(int min, int max)
{
    return rand()%(max - min) + min;
}

void Misc::Console(DebugFlags flag, char const* text, ...)
{
    if (DebugFlags(DEBUG_OPTIONS) & flag)
    {
        va_list args;
        va_start(args, text);
        vprintf(text, args);
        va_end(args);
    }
}

void Misc::Log(LogFlags flag, char const* text, ...)
{
    if (LogFlags(LOG_OPTIONS) & flag)
    {
        char buffer[strlen(text) + 250];

        va_list args;
        va_start(args, text);
        vsprintf(buffer, text, args);
        va_end(args);

        wApp->log("notice") << buffer;
    }
}

std::string Misc::GetFormattedString(const char * format, ...)
{
    char buffer[strlen(format) + 1000];

    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);

    return std::string(buffer);
}
