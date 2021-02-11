/*
*  This file is a part of sym_arrow library.
*
*  Copyright (c) Pawe³ Kowal 2017 - 2021
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

#include "sym_arrow/exception.h"
#include <sstream>

namespace sym_arrow
{

assert_exception::assert_exception(const std::string& file_, int line_, const std::string& message_)
    :file(file_),line(line_), message(message_)
{
    std::ostringstream os;
    os << line;

    message = message + " in file: " + file_ + " at line " + os.str();
};

std::string assert_exception::what() const
{
    return message;
};

void assert_exception::make(const char* txt, const char* description, 
                            const char* file, int line)
{
    std::string message = "assertion failed: ";
    message                += txt;

    if (description)
    {
        message            += ": ";
        message            += description;
    };
    
    assert_exception ex(file,line,message);
    std::cerr << ex.what() << "\n";
    throw assert_exception(file,line,message);
}

};