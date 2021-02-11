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

#pragma once

#include "error_formatter.h"

namespace sym_arrow { namespace error
{

line_mark::line_mark()
{};

line_mark::line_mark(line_type lt)
{
    m_line_types.push_back(lt);
};
line_mark::line_mark(const line_mark& prev, line_type lt)
    :m_line_types(prev.m_line_types)
{
    m_line_types.push_back(lt);
};

error_formatter::error_formatter()
    :m_line_type(line_type::init)
{};

std::ostream& error_formatter::head()
{
    return m_head;
}

void error_formatter::store()
{
    std::string l   = m_line.str();
    
    if (l.size() == 0)
        return;

    push_line_start();
    push(l);
    push("\n");

    m_line = stream_type();
}

void error_formatter::push_line_start()
{
    std::string str;

    long n = (long)m_line_type.m_line_types.size();

    for(long i = 0; i < n; ++ i)
    {
        auto lt = m_line_type.m_line_types[i];

        switch(lt)
        {
            case line_type::init:
                break;
            case line_type::new_line:
            {
                str += tab();                
                break;
            }
            case line_type::new_info:
            {
                if (i == n-1)
                    str += tab() + "* ";
                else
                    str += tab() + "  ";
                break;
            }
        };
    }

    push(str);
}

void error_formatter::push(const std::string& str)
{
    m_stored.push_back(str);
};

line_mark error_formatter::new_line()
{
    return new_line(line_mark());
};

line_mark error_formatter::new_info()
{
    return new_info(line_mark());
}

line_mark error_formatter::new_line(const line_mark& lm)
{
    store();
    m_line_type = line_mark(lm, line_type::new_line);

    return m_line_type;
};

line_mark error_formatter::new_info(const line_mark& lm)
{
    store();
    m_line_type = line_mark(lm, line_type::new_info);
    return m_line_type;
}

std::ostream& error_formatter::line()
{
    return m_line;
}

std::string error_formatter::tab() const
{
    return std::string(4, ' ');
}
std::string error_formatter::str()
{
    std::ostringstream os;
    os << m_head.str();

    store();

    if (m_stored.size() > 0)
        os << "\n";

    for (const auto ln : m_stored)
        os << ln;

    return os.str();
};

}}
