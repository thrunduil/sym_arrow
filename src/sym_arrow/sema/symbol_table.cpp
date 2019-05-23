/*
*  Morfa programming language.
*  This file is a part of sym_arrow library.
*
*  Copyright (c) Pawe³ Kowal 2017-2019
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

#include "sym_arrow/sema/symbol_table.h"
#include "sym_arrow/sema/symbol_table_impl.h"
#include "sym_arrow/error/sema_error.h"

namespace sym_arrow
{

sym_table::sym_table()
{
    m_impl  = impl_type::get();
};

sym_table::~sym_table()
{};

sym_table::sym_table(const sym_table& s)
    :m_impl(s.m_impl)
{};

sym_table& sym_table::operator=(const sym_table& s)
{
    m_impl = s.m_impl;
    return *this;
}

sym_table sym_table::get()
{
    return sym_table();
};

void sym_table::define_set(const identifier& sym, const set& s)
{
    return m_impl->define_set(sym, s);
};

void sym_table::define_symbol(const identifier& sym, const std::vector<identifier>& args,
                        const identifier& t, bool is_const)
{
    return m_impl->define_symbol(sym, args, t, is_const);
}

void sym_table::define_type(const identifier& t)
{
    return m_impl->define_type(t);
}

bool sym_table::is_defined_set(const identifier& sym) const
{
    return m_impl->is_defined_set(sym);
}

bool sym_table::is_defined_type(const identifier& sym) const
{
    return m_impl->is_defined_type(sym);
}

};
