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

#include "sym_arrow/nodes/function_expr.h"
#include "sym_arrow/ast/ast.h"

namespace sym_arrow
{

function_expr::function_expr()
{};

function_expr::function_expr(const ast::function_rep* h)
    :m_expr(h)
{};

function_expr::function_expr(const function_expr& other)
    :m_expr(other.m_expr)
{};

function_expr::function_expr(function_expr&& other)
    :m_expr(std::move(other.m_expr))
{};

function_expr& function_expr::operator=(const function_expr& other)
{
    m_expr = other.m_expr;
    return *this;
}

function_expr& function_expr::operator=(function_expr&& other)
{
    m_expr = std::move(other.m_expr);
    return *this;
}

function_expr::~function_expr()
{};

bool function_expr::is_null() const
{
    return m_expr.is_null();
};

const expr& function_expr::get_expr() const
{
    return m_expr;
};

size_t function_expr::size() const
{
    const ast::function_rep* h = m_expr.get_ptr()->static_cast_to<ast::function_rep>();
    return h->size();
};

expr function_expr::arg(size_t i) const
{
    const ast::function_rep* h = m_expr.get_ptr()->static_cast_to<ast::function_rep>();
    return expr(h->arg(i));
};

symbol function_expr::name() const
{
    const ast::function_rep* h = m_expr.get_ptr()->static_cast_to<ast::function_rep>();
    return symbol(ast::symbol_ptr::from_this(h->name()));
};

};
