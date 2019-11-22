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
#include "sym_arrow/nodes/type.h"

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
    //TODO
    ast::identifier_ptr n = ast::identifier_rep::make(ast::identifier_info(name, 
                                        num_char, nullptr));
    m_expr = n;
};

const char* identifier::get_name() const
{
    return get_ptr()->get_name();
}

size_t identifier::get_identifier_code() const
{
    return get_ptr()->get_identifier_code();
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

symbol identifier::index() const
{
    return sym_arrow::make_symbol(*this);
}

symbol identifier::index(const expr& x1) const
{
    return sym_arrow::make_symbol(*this, x1);
}

symbol identifier::index(const expr& x1, const expr& x2) const
{
    return sym_arrow::make_symbol(*this, x1, x2);
}

symbol identifier::index(std::initializer_list<expr> ex) const
{
    return sym_arrow::make_symbol(*this, ex);
}

symbol identifier::index(const std::vector<expr>& ex) const
{
    return sym_arrow::make_symbol(*this, ex);
}

//-------------------------------------------------------------------
//                  symbol
//-------------------------------------------------------------------

symbol::~symbol()
{};

symbol::symbol(const char* name, size_t num_char)
    :m_expr()
{
    //TODO
    ast::identifier_ptr n = ast::identifier_rep::make(ast::identifier_info
                                        (name, num_char, nullptr));

    m_expr = make_symbol(identifier(n)).get_ptr();
};

identifier symbol::get_name() const
{
    return identifier(get_ptr()->get_name());
}

const type& symbol::get_type() const
{
    return get_ptr()->get_type();
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

size_t symbol::get_identifier_code() const
{
    return get_ptr()->get_identifier_code();
};

};