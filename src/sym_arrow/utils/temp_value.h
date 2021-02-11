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
#include "sym_arrow/fwd_decls.h"
#include "sym_arrow/nodes/value.h"

namespace sym_arrow { namespace details
{

template<class Val, bool Is_pod>
class temp_value
{};

template<>
class temp_value<value, false>
{
    public:
        using handle_type   = typename value::handle_type;
        using value_stack   = std::vector<value>;

    private:
        value_stack*    m_stack;

    public:
        temp_value()
            :m_stack(nullptr)
        {};

        ~temp_value()
        {
            if (m_stack)
                release_stack(m_stack);
        }

        handle_type make_handle(const value& v)
        {
            if (m_stack == nullptr)
                m_stack = get_stack();

            m_stack->push_back(v);
            return v.get_handle();
        }

    private:
        temp_value(const temp_value&) = delete;
        temp_value& operator=(const temp_value&) = delete;

    private:
        value_stack*    get_stack();
        void            release_stack(value_stack* st);
};

template<class Val>
class temp_value<Val, true>
{
    public:
        static Val make_handle(Val v)
        {
            return v;
        };
};

}};