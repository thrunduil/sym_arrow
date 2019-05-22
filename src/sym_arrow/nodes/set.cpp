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

#include "sym_arrow/nodes/set.h"
#include "sym_arrow/ast/ast.h"
#include "sym_arrow/error/sema_error.h"

namespace sym_arrow
{

namespace sa = sym_arrow::ast;

set::set()
{};

set::set(const ptr_type& ex)
    :m_expr(ex)
{};

set::set(const std::vector<identifier>& elems)
{
    std::vector<identifier> el2 = elems;
    std::sort(el2.begin(), el2.end());

    size_t n    = el2.size();

    // 1. elements in elems vector must be different
    if (n > 1)
    {
        sa::identifier_handle l     = elems[0].get_ptr().get();
        bool show_err               = true;

        for (size_t i = 1; i < n; ++i)
        {
            sa::identifier_handle c = elems[i].get_ptr().get();

            if (c == l && show_err == true)
            {
                error::sema_error().set_elem_multiply_defined(elems[i]);
                show_err    = false;
            }
            else
            {
                l           = c;
                show_err    = true;
            };
        };
    };

    m_expr = ast::set_rep::make(ast::set_rep_info(el2.size(), el2.data()));
}

set::set(const set& other)
    :m_expr(other.m_expr)
{};

set::set(set&& other)
    :m_expr(std::move(other.m_expr))
{};

set& set::operator=(const set& other)
{
    m_expr = other.m_expr;
    return *this;
}

set& set::operator=(set&& other)
{
    m_expr = std::move(other.m_expr);
    return *this;
}

set::~set()
{};

bool set::is_null() const
{
    return m_expr == ptr_type(); 
};

const set::ptr_type& set::get_ptr() const
{
    return m_expr;
};

size_t set::size() const
{
    return m_expr->size();
};

identifier set::arg(size_t i) const
{
    return identifier(m_expr->arg(i));
};

bool set::is_member(const expr& x) const
{
    return m_expr->is_member(x);
}

};
