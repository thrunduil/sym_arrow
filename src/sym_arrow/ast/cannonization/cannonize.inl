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

#include "sym_arrow/ast/cannonization/cannonize.h"
#include "sym_arrow/ast/ast_rep_details.h"

namespace sym_arrow { namespace ast
{

template<class Item>
struct get_value;

template<>
struct get_value<value>
{
    static const value& eval(const value& v)
    {
        return v;
    };
};

template<>
struct get_value<details::value_expr<value>>
{
    static const value& eval(const details::value_expr<value>& v)
    {
        return v.m_value;
    };
};

template<>
struct get_value<build_item_handle<value>>
{
    static const value& eval(const build_item_handle<value>& v)
    {
        return v.get_value();
    };
};

template<>
struct get_value<build_item<value>>
{
    static const value& eval(const build_item<value>& v)
    {
        return v.get_value();
    };
};
template<class Item>
value cannonize::get_normalize_scaling(const value& V0, size_t n, const Item* V)
{
    value max   = abs(V0);

    for (size_t i = 0; i < n; ++i)
    {
        value loc   = abs(get_value<Item>::eval(V[i]));
    
        if (loc > max)
            max = loc;
    };
    
    if (max.is_finite() == false)
        return value::make_one();

    if (V0.is_zero() == false)
    {
        if (V0.is_positive() == true)
            return max;
        else
            return -max;
    }
    else
    {
        if (get_value<Item>::eval(V[0]).is_positive() == true)
            return max;
        else
            return -max;
    };
};

};};
