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

#include "sym_arrow/ast/cannonization/simplifier.h"
#include "sym_arrow/utils/sort.inl"

namespace sym_arrow { namespace ast
{

template<class Item, class Value_handle_builder>
void simplify_expr<Item, Value_handle_builder>::sort(Item* ptr, size_t size)
{
    struct expr_comp
    {
        bool operator()(const Item& a, const Item& b) const
        {
            return a.compare(b);
        };
    };

    sym_arrow::utils::sort_q(ptr, size, expr_comp());
};

template<class Item, class Value_handle_builder>
void simplify_expr<Item, Value_handle_builder>::make(Item* ptr, size_t& size, 
                                            Value_handle_builder& handle_builder)
{
    if (size < 2)
        return;

    sort(ptr, size);

    bool any_simpl = false;
    simplify(ptr, size, any_simpl, handle_builder);

    if (any_simpl == true)
    {
        size_t pos = 0;

        for (size_t i = 0; i < size; ++i)
        {
            if (ptr[i].get_expr_handle() != nullptr)
            {
                ptr[pos] = std::move(ptr[i]);
                ++pos;
            };
        };

        size = pos;
    };
};

template<class Item, class Value_handle_builder>
void simplify_expr<Item, Value_handle_builder>::simplify(Item* x, size_t size, bool& any_simpl,
                                                    Value_handle_builder& handle_builder)
{
    using value_type    = decltype(x[0].get_value());
    using  expr_type    = decltype(x[0].get_expr_handle());    

    for (size_t i = 1; i < size; ++i)
    {
        expr_handle x_last(x[i-1].get_expr_handle());
        expr_handle x_current(x[i].get_expr_handle());

        if (x_current == x_last)
        {
            value_type tmp_s = std::move(x[i-1].get_value()) + std::move(x[i].get_value());

            if (is_value_zero(tmp_s) == true)
                x[i].get_expr_ref()     = expr_type();
            else
                x[i].get_value_ref()    = handle_builder.make_handle(tmp_s);
          
            x[i-1].get_expr_ref()       = expr_type();
            any_simpl                   = true;
        };
    };
};

};};