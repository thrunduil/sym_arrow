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
#include "sym_arrow/functions/constructor_functions.h"
#include "sym_arrow/utils/stack_array.h"
#include "sym_arrow/sema/symbol_table_impl.h"
#include "sym_arrow/sema/typing.h"
#include "sym_arrow/error/sema_error.h"

namespace sym_arrow 
{

namespace sd = sym_arrow::details;
namespace se = sym_arrow::error;

//-------------------------------------------------------------------
//                  identifier
//-------------------------------------------------------------------
identifier::~identifier()
{};

identifier::identifier(const char* name, size_t num_char)
    :m_expr()
{
    ast::identifier_ptr n = ast::identifier_rep::make(ast::identifier_info(name, num_char));
    m_expr = n;

    if (num_char == 0 || name[0] == '$')
        throw std::runtime_error("invalid symbol name");
};

const char* identifier::get_name() const
{
    return get_ptr()->get_name();
}

size_t identifier::get_base_symbol_code() const
{
    return get_ptr()->get_base_symbol_code();
};

expr identifier::operator()() const
{
    return function(*this);
}

expr identifier::operator()(const expr& x1) const
{
    return sym_arrow::function(*this, x1);
}

expr identifier::operator()(const expr& x1, const expr& x2) const
{
    return sym_arrow::function(*this, x1, x2);
}

expr identifier::operator()(std::initializer_list<expr> ex) const
{
    return sym_arrow::function(*this, ex);
}

expr identifier::operator()(const std::vector<expr>& ex) const
{
    return sym_arrow::function(*this, ex);
}

symbol identifier::index(const identifier& t) const
{
    return sym_arrow::make_symbol(*this, t, false);
}

symbol identifier::index(const expr& x1, const identifier& t) const
{
    return sym_arrow::make_symbol(*this, x1, t, false);
}

symbol identifier::index(const expr& x1, const expr& x2, const identifier& t) const
{
    return sym_arrow::make_symbol(*this, x1, x2, t, false);
}

symbol identifier::index(std::initializer_list<expr> ex, const identifier& t) const
{
    return sym_arrow::make_symbol(*this, ex, t, false);
}

symbol identifier::index(const std::vector<expr>& ex, const identifier& t) const
{
    return sym_arrow::make_symbol(*this, ex, t, false);
}

//-------------------------------------------------------------------
//                  symbol
//-------------------------------------------------------------------

symbol::~symbol()
{};

symbol::symbol(const char* name, size_t num_char)
    :m_expr()
{
    ast::identifier_ptr n = ast::identifier_rep::make(ast::identifier_info(name, num_char));
    m_expr = make_symbol(identifier(n), identifier(), false).get_ptr();
};

identifier symbol::get_name() const
{
    return identifier(get_ptr()->get_name());
}

identifier symbol::get_type() const
{
    return identifier(get_ptr()->get_type());
}

size_t symbol::size() const
{
    return get_ptr()->size();
}

bool symbol::is_const() const
{
    return get_ptr()->is_const();
}

expr symbol::arg(size_t i) const
{
    return expr(get_ptr()->arg(i));
}

size_t symbol::get_symbol_code() const
{
    return get_ptr()->get_symbol_code();
};

size_t symbol::get_base_symbol_code() const
{
    return get_ptr()->get_base_symbol_code();
};

};