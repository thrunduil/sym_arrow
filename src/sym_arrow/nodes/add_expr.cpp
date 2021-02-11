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

#include "sym_arrow/nodes/add_expr.h"
#include "sym_arrow/ast/ast.h"

namespace sym_arrow
{

add_expr::add_expr()
{};

add_expr::add_expr(const ast::add_rep* h)
    :m_expr(h)
{};

add_expr::add_expr(const add_expr& other)
    :m_expr(other.m_expr)
{};

add_expr::add_expr(add_expr&& other)
    :m_expr(std::move(other.m_expr))
{};

add_expr& add_expr::operator=(const add_expr& other)
{
    m_expr = other.m_expr;
    return *this;
}

add_expr& add_expr::operator=(add_expr&& other)
{
    m_expr = std::move(other.m_expr);
    return *this;
};

add_expr::~add_expr()
{};

bool add_expr::is_null() const
{
    return m_expr.is_null();
}

const expr& add_expr::get_expr() const
{
    return m_expr;
}

size_t add_expr::size() const
{
    const ast::add_rep* h = m_expr.get_ptr()->static_cast_to<ast::add_rep>();
    return h->size();
};

bool add_expr::has_log() const
{
    const ast::add_rep* h = m_expr.get_ptr()->static_cast_to<ast::add_rep>();
    return h->has_log();
};

const value& add_expr::V0() const
{
    const ast::add_rep* h = m_expr.get_ptr()->static_cast_to<ast::add_rep>();
    return h->V0();
};

const value& add_expr::V(size_t i) const
{
    const ast::add_rep* h = m_expr.get_ptr()->static_cast_to<ast::add_rep>();
    return h->V(i);
};

expr add_expr::Log() const
{
    const ast::add_rep* h = m_expr.get_ptr()->static_cast_to<ast::add_rep>();

    if (h->has_log() == false)
        return expr();

    return expr(h->Log());
};

expr add_expr::E(size_t i) const
{
    const ast::add_rep* h = m_expr.get_ptr()->static_cast_to<ast::add_rep>();

    return expr(h->E(i));
}

};
