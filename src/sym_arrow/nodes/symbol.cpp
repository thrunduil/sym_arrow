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

#include "sym_arrow/nodes/symbol.inl"
#include "sym_arrow/ast/symbol_rep.h"
#include "sym_arrow/ast/ast.h"
#include "sym_arrow/functions/expr_functions.h"

namespace sym_arrow 
{

symbol::~symbol()
{};

symbol::symbol(const char* name, size_t num_char)
    :m_expr(ast::symbol_rep::make(ast::named_symbol_info(name, num_char)))
{
    if (num_char == 0 || name[0] == '$')
        throw std::runtime_error("invalid symbol name");
};

const char* symbol::get_name() const
{
    return get_ptr()->get_name();
}

size_t symbol::get_symbol_code() const
{
    return get_ptr()->get_symbol_code();
};

expr symbol::operator()() const
{
    return function(*this);
}

expr symbol::operator()(const expr& x) const
{
    return function(*this, x);
}

expr symbol::operator()(const expr& x, const expr& y) const
{
    return function(*this, x, y);
}

expr symbol::operator()(std::initializer_list<expr> ex) const
{
    return function(*this, ex.begin(), ex.size());
}

};