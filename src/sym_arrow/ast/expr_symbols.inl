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

#include "sym_arrow/ast/expr_symbols.h"
#include "sym_arrow/details/dag_traits.h"
#include "sym_arrow/func/symbol_functions.h"

namespace sym_arrow { namespace ast
{

template<class Derived>
expr_symbols<Derived>::expr_symbols(const Derived* tag)
    :base_type(tag)
{};

template<class Derived>
const dbs_lib::dbs& expr_symbols<Derived>::get_symbol_set() const
{
    return m_symbols;
};

template<class Derived>
size_t expr_symbols<Derived>::number_symbols() const
{
    return m_symbols.size();
}

template<class Derived>
void expr_symbols<Derived>::add_symbol(size_t c)
{
    m_symbols = std::move(m_symbols).set(c);
};

template<class Derived>
void expr_symbols<Derived>::add_symbols(const dbs& h)
{
    m_symbols   = (std::move(m_symbols) | h);
};

template<class Derived>
bool expr_symbols<Derived>::has_symbol(size_t code) const
{
    return m_symbols.test(code);
}

template<class Derived>
bool expr_symbols<Derived>::has_any_symbol(const dbs& h) const
{
    return m_symbols.test_any(h);
}

template<class Derived>
bool expr_symbols<Derived>::has_all_symbols(const dbs& h) const
{
    return m_symbols.test_all(h);
}

template<class Derived>
void expr_symbols<Derived>::add_symbols(expr_handle h)
{
    details::add_symbols(h, m_symbols);
};

};};
