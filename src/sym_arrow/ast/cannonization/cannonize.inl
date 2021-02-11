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
value cannonize::get_normalize_scaling(const value& V0, size_t n, const Item* V, bool has_log)
{
    (void)n;

    if (has_log == true)
        return value::make_one();

    if (V0.is_zero() == false)
    {
        if (V0.is_negative() == true)
            return value::make_minus_one();
        else
            return value::make_one();
    };

    value loc   = V[0].get_value();

    if (loc.is_negative() == true)
        return value::make_minus_one();
    else
        return value::make_one();
};

template<class Item>
value cannonize::get_normalize_scaling(const value& V0, size_t n, const Item* V, bool has_log, 
                                        const value& v)
{
    (void)n;

    if (has_log == true)
        return value::make_one();

    if (V0.is_zero() == false)
    {
        if (V0.is_negative() == true)
            return value::make_minus_one();
        else
            return value::make_one();
    };

    bool is_v_neg   = v.is_negative();    
    value loc       = V[0].get_value();
    bool is_neg     = loc.is_negative();

    if (is_v_neg != is_neg)
        return value::make_minus_one();
    else
        return value::make_one();
};

};};
