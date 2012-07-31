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

#ifndef MISC_H_INCLUDED
#define MISC_H_INCLUDED

#include "defines.h"

namespace Misc
{
    /********************************************//**
     * \brief Simple random function.
     *
     * \param min   minimum value
     * \param max   maximum value
     * \return randomized value beatween min and max
     *
     ***********************************************/

    int Irand(int min, int max);

    /********************************************//**
     * \brief Writes debug info to console
     *
     * \param flag  Debug flags for which debug should be printed to console.
     * \param text  Text format which should be printed to console.
     *
     * This function checks flags defined in config and decides
     * if debug should be printed to standard output or not.
     * If yes then function fills format with additional data and
     * sends to std out.
     *
     ***********************************************/

    void Console(DebugFlags flag, char const* text, ...);

    /********************************************//**
     * \brief Handles logging
     *
     * \param flag  Log flags for which logging should be done.
     * \param text  Text format which should be logged.
     *
     * This function checks flags defined in config and decides
     * if logging should be done or not.
     * If yes then function fills format with additional data and
     * sends it to log.
     *
     ***********************************************/

    void Log(LogFlags flag, char const* text, ...);

    /********************************************//**
     * \brief Resturns format string filled with data.
     *
     * \param format    string format to fill
     * \return format string with filled with data
     *
     ***********************************************/

    std::string GetFormattedString(const char * format, ...);

    /********************************************//**
     * \brief Sends email.
     *
     * \param from  source email address
     * \param to    destination email address
     * \param sub   subject for email
     * \param msg   message to send
     *
     * Function to send email from source email address to destination email address contains given message and subject.
     *
     ***********************************************/

    void SendMail(const Wt::WString& from, const Wt::WString& to, const Wt::WString& sub, const Wt::WString& msg);
}

#endif // MISC_H_INCLUDED
