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

#pragma once

#include "sym_arrow/ast/builder/build_item.h"

namespace sym_arrow { namespace ast
{

//-------------------------------------------------------------------
//                      ITEM
//-------------------------------------------------------------------
template<class Value_type>
inline build_item<Value_type>::build_item(const Value_type& scal, expr_handle expr)
    : m_value(scal), m_expr(expr, sym_dag::copy_t()), m_is_special(false)
{};

template<class Value_type>
inline build_item<Value_type>::build_item(const value_type& scal, const expr& expr)
    : m_value(scal), m_expr(expr.get_ptr()), m_is_special(false)
{};

template<class Value_type>
inline build_item<Value_type>::build_item(const value_type& scal, expr&& expr)
    : m_value(scal), m_expr(std::move(expr.get_ptr())), m_is_special(false)
{};

template<class Value_type>
inline build_item<Value_type>::build_item(special_t, const Value_type& scal, 
                        expr_handle special_expr)
    : m_value(scal), m_expr(special_expr, sym_dag::copy_t()), m_is_special(true)
{}

template<class Value_type>
inline build_item<Value_type>::build_item(const build_item& other)
    : m_value(other.m_value), m_expr(other.m_expr), m_is_special(other.m_is_special)
{}

template<class Value_type>
inline build_item<Value_type>::build_item(build_item&& other)
    : m_value(std::move(other.m_value)), m_expr(std::move(other.m_expr))
    , m_is_special(other.m_is_special)
{}

template<class Value_type>
inline build_item<Value_type>&
build_item<Value_type>::operator=(const build_item& other)
{
    m_value         = other.m_value;
    m_expr          = other.m_expr;
    m_is_special    = other.m_is_special;
    return *this;
}

template<class Value_type>
inline build_item<Value_type>& 
build_item<Value_type>::operator=(build_item&& other)
{
    m_value         = std::move(other.m_value);
    m_expr          = std::move(other.m_expr);
    m_is_special    = other.m_is_special;
    return *this;
}


template<class Value_type>
inline const Value_type& build_item<Value_type>::get_value() const
{
    return m_value;
};

template<class Value_type>
inline Value_type& build_item<Value_type>::get_value_ref()
{
    return m_value;
};

template<class Value_type>
void build_item<Value_type>::release(vlist_stack& stack)
{
    expr_handle ex  = m_expr.release();

    stack.push_back(ex);
};

template<class Value_type>
inline expr_handle build_item<Value_type>::get_expr_handle() const
{
    return m_expr.get();
};

template<class Value_type>
inline bool build_item<Value_type>::is_special() const
{
    return m_is_special;
};

template<class Value_type>
inline bool build_item<Value_type>::compare(const build_item& b) const
{
    return this->m_expr < b.m_expr;
};

//-------------------------------------------------------------------
//                      ITEM_HANDLE
//-------------------------------------------------------------------
template<class Value_type>
inline build_item_handle<Value_type>::build_item_handle(const Value_type& v)
    :m_is_special(false), m_expr(nullptr), m_value(v)
{};

template<class Value_type>
inline build_item_handle<Value_type>::build_item_handle(const Value_type& p, 
                                           expr_handle e)    
    :m_value(p), m_expr(e), m_is_special(false)
{};

template<class Value_type>
inline bool build_item_handle<Value_type>::is_special() const
{
    return m_is_special;
};

template<class Value_type>
inline bool build_item_handle<Value_type>::compare(const build_item_handle& b) const
{
    return this->m_expr < b.m_expr;
}

};};
