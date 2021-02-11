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

#include "sym_arrow/config.h"
#include "sym_arrow/fwd_decls.h"
#include "sym_arrow/nodes/value.h"
#include "dag/dag.h"
#include "sym_arrow/ast/ast_tags.h"

namespace sym_arrow { namespace ast { namespace details
{

template<class Value>
struct value_expr
{
    Value       m_value;
    expr_ptr    m_expr;

    value_expr(const Value& v);
    value_expr(const Value& v, expr_handle h);

    expr_handle     get_expr_handle() const { return m_expr.get(); };
    const Value&    get_value() const       { return m_value; };
};

template<class Value>
inline value_expr<Value>::value_expr(const Value& v)
    :m_value(v)
{}

template<class Value>
inline value_expr<Value>::value_expr(const Value& v, expr_handle h)
    :m_value(v), m_expr(expr_ptr::from_this(h)) 
{};

};};};
