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

#ifndef MISCHASH_H_INCLUDED
#define MISCHASH_H_INCLUDED

#include <string>

#include "defines.h"

namespace Misc
{
    namespace Hash
    {
        enum Flags
        {
            HASH_FLAG_UPPER     = 1,    /**< text should be uppered before hashing */
            HASH_FLAG_BIN       = 2,    /**< return bin instead of hex string */
        };

        /********************************************//**
         * \brief Returns sha1 hash from given text
         *
         * \param txt   text format to be hashed
         * \param flags additional flags
         * \return hashed hex/bin string
         *
         ***********************************************/

        std::string GetSHA1(const char * txt, uint32 flags);

        /********************************************//**
         * \brief Returns sha1 hash from given text as WString
         *
         * \param txt   text format to be hashed
         * \param flags additional flags
         * \return hashed hex/bin WString
         *
         ***********************************************/

        Wt::WString WGetSHA1(const char * txt, uint32 flags);

        /********************************************//**
         * \brief Returns sha1 from given formatted text by using GetSHA1 function
         *
         * \param txt   text format to be hashed
         * \param flags additional flags
         * \return hashed hex/bin string
         *
         ***********************************************/

        std::string PGetSHA1(const char * txt, uint32 flags, ...);

        /********************************************//**
         * \brief Returns sha1 from gicen formatted text by using WGetSHA1 function
         *
         * \param txt   text format to be hashed
         * \param flags additional flags
         * \return hashed hex/bin WString
         *
         ***********************************************/

        Wt::WString PWGetSHA1(const char * txt, uint32 flags, ...);
    }
}


#endif // MISCHASH_H_INCLUDED
