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

#include "sym_arrow/nodes/value_functions_common.h"

namespace sym_arrow { namespace details
{

value details::special_cases_power_real(const value& v10, const value& v2, bool& computed)
{
    // special cases:
    // special cases are treated differently than according to IEEE 754-2008 standards:
    // power_real(NaN, y)   -> NaN for any y
    // power_real(x, NaN)   -> NaN for any x
    // pow(+-Inf, y)        -> plus zero for y negative, and plus infinity for y positive
    // pow(+-Inf, 0)        -> NaN
    // pow(x, -Inf)         -> plus infinity for 0 < |x| < 1, and plus zero for |x| > 1
    // pow(0, -Inf)         -> NaN
    // pow(x, +Inf)         -> plus zero for 0 < |x| < 1, and plus infinity for |x| > 1
    // pow(0, +Inf)         -> NaN
    // pow(0, y)            -> plus infinity for y < 0
    // pow(0, y)            -> plus zero for y > 0
    // power_real(0, +-0)   -> NaN
    // power_real(x, +-0)   -> 1
    // power_real(+-1, y)   -> 1

    value v1    = abs(v10);

    if (v1.is_finite() == false || v1.is_zero() == true || v1.is_one() == true
         || v2.is_finite() == false || v2.is_zero() == true)
    {
        computed = true;
    }
    else
    {
        computed = false;
    }

    value zero      = value::make_zero();
    value one       = value::make_one();
    value nan       = value::make_nan();
    value inf       = value::make_inf_plus();

    if (computed == false)
        return zero;

    if (v1.is_nan() == true)
        return nan;

    if (v2.is_nan() == true)
        return nan;

    if (v1.is_inf() == true)
    {
        if (v2.is_negative() == true)
            return zero;
        else if (v2.is_positive() == true)
            return inf;
        else
            return nan;
    }

    if (v2.is_inf_minus() == true)
    {
        if (v1 < one)
            return inf;
        else if (v1 > one)
            return zero;
        else
            return nan;
    }

    if (v2.is_inf_plus() == true)
    {
        if (v1 < one)
            return zero;
        else if (v1 > one)
            return inf;
        else
            return nan;
    }

    if (v1.is_zero() == true)
    {
        if (v2 < zero)
            return inf;
        else if (v2 > zero)
            return zero;
        else
            return nan;
    }

    if (v2.is_zero() == true)
        return one;

    if (v1.is_one() == true)
        return one;

    // not possible
    computed = false;
    return zero;
}

value details::special_cases_power_int(const value& v1, int v2, bool& computed)
{
    // integer power v1 ^ v2
    // special cases are treated differently than according to IEEE 754-2008 standards:
    // power_int(NaN, y)    -> NaN for any y
    // power_int(-Inf, y)   -> 0 for y < 0
    // power_int(-Inf, y)   -> -Inf for y > 0 and y odd
    // power_int(-Inf, y)   -> Inf for y > 0 and y even
    // power_int(-Inf, 0)   -> NaN
    // power_int(+Inf, y)   -> 0 for y < 0
    // power_int(+Inf, y)   -> +Inf for y > 0
    // power_int(+Inf, 0)   -> NaN
    // power_int(+-0, y)    -> 0.0 for y > 0
    // power_int(+-0, y)    -> NaN for y < 0
    // power_int(0, 0)      -> NaN
    // power_int(x, 0)      -> 1
    // power_int(1, y)      -> 1

    if (v1.is_finite() == false || v1.is_zero() == true || v2 == 0)
    {
        computed = true;
    }
    else
    {
        computed = false;
    }

    value zero      = value::make_zero();
    value one       = value::make_one();
    value nan       = value::make_nan();
    value inf       = value::make_inf_plus();

    if (computed == false)
        return zero;

    if (v1.is_nan() == true)
        return nan;

    bool v2_odd     = (v2 % 2 != 0);
    bool v2_even    = (v2 % 2 == 0);

    if (v1.is_inf_minus() == true)
    {
        if (v2 < 0)
            return zero;
        else if (v2 > 0 && v2_odd == true)
            return -inf;
        else if (v2 > 0 && v2_even == true)
            return inf;
        else
            return nan;
    }

    if (v1.is_inf_plus() == true)
    {
        if (v2 < 0)
            return zero;
        else if (v2 > 0)
            return inf;
        else
            return nan;
    }

    if (v1.is_zero() == true)
    {
        if (v2 < 0)
            return nan;
        else if (v2 > 0)
            return zero;
        else if (v2 == 0)
            return nan;
    }

    if (v2 == 0)
        return one;

    // not possible
    computed = false;
    return zero;
}

}}
