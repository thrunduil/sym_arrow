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

#include "sym_arrow/nodes/mult_expr.h"
#include "sym_arrow/ast/ast.h"

namespace sym_arrow
{

mult_expr::mult_expr()
{};

mult_expr::mult_expr(const ast::mult_rep* h)
    :m_expr(h)
{};

mult_expr::mult_expr(const mult_expr& other)
    :m_expr(other.m_expr)
{};

mult_expr::mult_expr(mult_expr&& other)
    :m_expr(std::move(other.m_expr))
{};

mult_expr& mult_expr::operator=(const mult_expr& other)
{
    m_expr = other.m_expr;
    return *this;
}

mult_expr& mult_expr::operator=(mult_expr&& other)
{
    m_expr = std::move(other.m_expr);
    return *this;
}

mult_expr::~mult_expr()
{};

bool mult_expr::is_null() const
{
    return m_expr.is_null();
};

const expr& mult_expr::get_expr() const
{
    return m_expr;
};

size_t mult_expr::isize() const
{
    const ast::mult_rep* h = m_expr.get_ptr()->static_cast_to<ast::mult_rep>();
    return h->isize();
};

size_t mult_expr::rsize() const
{
    const ast::mult_rep* h = m_expr.get_ptr()->static_cast_to<ast::mult_rep>();
    return h->rsize();
};

bool mult_expr::has_exp() const
{
    const ast::mult_rep* h = m_expr.get_ptr()->static_cast_to<ast::mult_rep>();
    return h->has_exp();
};

expr mult_expr::Exp() const
{
    const ast::mult_rep* h = m_expr.get_ptr()->static_cast_to<ast::mult_rep>();

    if (h->has_exp() == false)
        return expr();

    return expr(h->Exp());
};

int mult_expr::IV(size_t i) const
{
    const ast::mult_rep* h = m_expr.get_ptr()->static_cast_to<ast::mult_rep>();
    return h->IV(i);
};

expr mult_expr::IE(size_t i) const
{
    const ast::mult_rep* h = m_expr.get_ptr()->static_cast_to<ast::mult_rep>();

    return expr(h->IE(i));
}

expr mult_expr::RE(size_t i) const
{
    const ast::mult_rep* h = m_expr.get_ptr()->static_cast_to<ast::mult_rep>();

    return expr(h->RE(i));
}

const value& mult_expr::RV(size_t i) const
{
    const ast::mult_rep* h = m_expr.get_ptr()->static_cast_to<ast::mult_rep>();
    return h->RV(i);
}

};
