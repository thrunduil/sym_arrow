/*
*  Morfa programming language.
*  This file is a part of sym_arrow library.
*
*  Copyright (c) Pawe³ Kowal 2017
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#pragma once

#include "sym_arrow/config.h"

#include <string>
#include <iostream>

#pragma warning(push)
#pragma warning(disable:4251) //needs to have dll-interface to be used by clients

namespace sym_arrow
{

// exception class called when assert fails; 
// this class is not derived from std::exception intensionally
class SYM_ARROW_EXPORT assert_exception
{
    private:
        std::string     file;
        int             line;
        std::string     message;

    public:
        assert_exception(const std::string& file, int line, const std::string& message);
        virtual std::string what() const;

        // throw assert_exception
        static void     make(const char* txt, const char* description, 
                            const char* file, int line);
};

};

#pragma warning(pop)

// define assertion macro; when assertion fails, then
// assert_exception is thrown
#ifdef SYM_ARROW_DEBUG_EXPR
    #define assertion(cond,description)        \
        ((cond) ? (void)0 : sym_arrow::assert_exception::make(#cond,description, __FILE__, __LINE__))
#else
    #define assertion(cond,description)
#endif