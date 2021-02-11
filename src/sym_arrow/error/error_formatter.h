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

#include "sym_arrow/config.h"

#include <sstream>
#include <list>
#include <vector>

namespace sym_arrow { namespace error
{

enum class line_type
{
    init,
    new_line,
    new_info
};

class line_mark
{
    public:
        std::vector<line_type>  m_line_types;

    public:
        line_mark();
        line_mark(line_type lt);
        line_mark(const line_mark& prev, line_type lt);
};

class error_formatter
{
    private:
        using stream_type       = std::ostringstream;
        using stream_type_list  = std::list<std::string>;

    private:
        stream_type         m_head;
        stream_type         m_line;
        stream_type_list    m_stored;
        line_mark           m_line_type;

    public:
        error_formatter();

        line_mark           new_line();
        line_mark           new_info();
        line_mark           new_line(const line_mark& lm);
        line_mark           new_info(const line_mark& lm);

        std::ostream&       head();    
        std::ostream&       line();

        std::string         str();

    private:
        void                store();
        void                push_line_start();
        void                push(const std::string& str);
        std::string         tab() const;
};

}}
