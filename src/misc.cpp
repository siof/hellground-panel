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
#include <fstream>

#include <jwsmtp/jwsmtp.h>
#include <Wt/WApplication>

#include "config.h"
#include "database.h"

void Misc::SendMail(const Wt::WString& from, const Wt::WString& to, const Wt::WString& sub, const Wt::WString& msg)
{
    jwsmtp::mailer mail;

    mail.addrecipient(to.toUTF8().c_str());
    mail.setsender(from.toUTF8().c_str());
    mail.setsubject(sub.toUTF8().c_str());
    mail.setmessage(msg.toUTF8().c_str());

    mail.setserver(sConfig.GetConfig(CONFIG_MAIL_HOST));
    mail.username(sConfig.GetConfig(CONFIG_MAIL_USER));
    mail.password(sConfig.GetConfig(CONFIG_MAIL_PASSWORD));

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
    if (sConfig.GetConfig(CONFIG_OPTIONS_DEBUG) & flag)
    {
        va_list args;
        va_start(args, text);
        vprintf(text, args);
        va_end(args);
    }
}

void Misc::Log(LogFlags flag, char const* text, ...)
{
    if (sConfig.GetConfig(CONFIG_OPTIONS_LOG) & flag)
    {
        char buffer[strlen(text) + 1000];

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
    memset(buffer, '\0', strlen(format) + 1000);

    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);

    return std::string(buffer);
}

std::string Misc::GetTemplate(const std::string & fullPath)
{
    std::string templateStr;
    std::fstream tmpltFile;

    tmpltFile.open(fullPath.c_str(), std::fstream::in);
    if (tmpltFile.is_open() && tmpltFile.good())
    {
        char tmpStr[1000];
        while (!tmpltFile.eof())
        {
            tmpltFile.getline(tmpStr, 1000, '\n');
            templateStr += tmpStr;
        }
    }
    tmpltFile.close();

    return templateStr;
}

std::string Misc::GetTemplate(const std::string & tmpltPath, const std::string & name)
{
    return GetTemplate(tmpltPath + "/" + name + ".tmplt");
}

TemplateInfo Misc::GetTemplateInfoFromDB(const std::string & name)
{
    TemplateInfo templateInfo;
    Database db;

    if (db.Connect(DB_PANEL_DATA))
    {
        std::string tmpName = db.EscapeString(name);
        if (db.ExecutePQuery("SELECT name, stylePath, tmpltPath FROM Templates WHERE name = '%s'", tmpName.c_str()) > DB_RESULT_EMPTY)
        {
            templateInfo.name = db.GetRow()->fields[0].GetString();
            templateInfo.stylePath = db.GetRow()->fields[1].GetString();
            templateInfo.tmpltPath = db.GetRow()->fields[2].GetString();

            templateInfo.currentTemplate = GetTemplate(templateInfo.GetFullTemplatePath());
        }
    }

    if (templateInfo.currentTemplate.empty() || templateInfo.stylePath.empty())
    {
        templateInfo.name = sConfig.GetConfig(CONFIG_DEFAULT_TEMPLATE_NAME);
        templateInfo.stylePath = sConfig.GetConfig(CONFIG_DEFAULT_TEMPLATE_STYLE_PATH);
        templateInfo.tmpltPath = sConfig.GetConfig(CONFIG_DEFAULT_TEMPLATE_TMPLT_PATH);
    }

    return templateInfo;
}

TemplateInfo Misc::GetTemplateInfoFromDB(const Wt::WString & name)
{
    return GetTemplateInfoFromDB(name.toUTF8());
}

std::vector<TemplateInfo> Misc::GetTemplatesFromDB()
{
    std::vector<TemplateInfo> templates;
    Database db;
    if (db.Connect(DB_PANEL_DATA))
    {
        if (db.ExecutePQuery("SELECT name, stylePath, tmpltPath FROM Templates") > DB_RESULT_EMPTY)
        {
            std::list<DatabaseRow*> tmpRows = db.GetRows();
            const DatabaseRow * tmpRow;
            for (std::list<DatabaseRow*>::const_iterator itr = tmpRows.begin(); itr != tmpRows.end(); ++itr)
            {
                tmpRow = *itr;

                TemplateInfo tmplt;
                tmplt.name = tmpRow->fields[0].GetString();
                tmplt.stylePath = tmpRow->fields[1].GetString();
                tmplt.tmpltPath = tmpRow->fields[2].GetString();
                tmplt.currentTemplate = GetTemplate(tmplt.tmpltPath, tmplt.name);

                templates.push_back(tmplt);
            }
        }
    }

    return templates;
}
