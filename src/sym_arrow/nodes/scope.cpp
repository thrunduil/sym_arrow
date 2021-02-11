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

#include "sym_arrow/nodes/scope.h"
#include "sym_arrow/ast/ast.h"

namespace sym_arrow
{

scope::scope()
    :m_expr(value_type::global_scope())
{};

scope::scope(const ptr_type& ex)
    : m_expr(ex)
{};

const scope::ptr_type& scope::get_ptr() const
{
    return m_expr;
};

scope scope::make_scope(const scope& parent)
{
    return scope(value_type::make_scope(parent.get_ptr().get()));
};

scope scope::make_scope(const scope& parent, const std::string& name)
{
    return scope(value_type::make_scope(parent.get_ptr().get(), name));
};

bool scope::is_global() const
{
    return m_expr->is_global();
};

identifier scope::get_unique_identifier() const
{
    return m_expr->get_unique_identifier();
}

scope scope::get_parent_scope() const
{
    return scope(m_expr->get_parent_scope());
}

};
