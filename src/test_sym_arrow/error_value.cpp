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

#include "error_value.h"
#include "sym_arrow/nodes/value.h"

namespace sym_arrow { namespace testing
{

error_value::error_value(const value& v)
    : m_value(v), m_error(0.0)
{};

error_value::error_value(const value& v, const double& err)
    :m_value(v), m_error(std::abs(err))
{};

bool error_value::is_precise(double ulp_limit) const
{
    if (this->is_finite() == false)
        return false;

    if (m_error == 0.0)
        return true;

    if (m_value.is_zero() == true)
        return false;

    return m_error < std::abs(ulp_limit);
};

bool error_value::is_equal(const value& val, double ulp_limit, double& ulp_error) const
{
    bool fin1   = val.is_finite();
    bool fin2   = m_value.is_finite();
    ulp_error   = std::numeric_limits<double>::infinity();

    if (fin1 == false || fin2 == false)
    {
        if (val.is_nan() == true && m_value.is_nan() == true)
            return true;

        return val == m_value;
    };

    if (val == m_value)
    {
        ulp_error = 0.0;
        return true;
    };

    if (std::isfinite(m_error) == false)
        return true;

    if (m_value.is_zero() == true)
        return false;

    double dif      = float_distance(m_value, val);
    double ulp_err  = dif / (1 + m_error); 
    ulp_error       = ulp_err;

    return ulp_err < ulp_limit;
};

const value& error_value::get_value() const
{
    return m_value;
}
double error_value::get_error() const
{
    return m_error;
}

value error_value::get_abs_error() const
{
    return m_error * m_value * std::numeric_limits<double>::epsilon();
}

bool error_value::is_finite() const
{
    return m_value.is_finite() && std::isfinite(m_error);
}

bool error_value::is_value_finite() const
{
    return m_value.is_finite();
}

void testing::disp(const error_value& v, bool add_nl)
{
    return disp(std::cout, v, add_nl);
};

void testing::disp(std::ostream& os, const error_value& v, bool add_nl)
{
    os << "("; disp(os, v.get_value(), false); os << " - " << v.get_error() << ")";
    if (add_nl)
        os << "\n";
};

error_value testing::operator-(const error_value& v1)
{
    return error_value(-v1.get_value(), v1.get_error());
}

error_value testing::abs(const error_value& v1)
{
    return error_value(abs(v1.get_value()), v1.get_error());
}

error_value testing::exp(const error_value& v1)
{
    value v     = exp(v1.get_value());
    double err  = std::abs(v1.get_value().get_double()) * v1.get_error();
    
    if (v1.get_value().is_zero() == false)
        err     = err + 0.5;

    return error_value(v, err);
};

error_value testing::log(const error_value& v1)
{
    value v     = log(v1.get_value());
    value err   = 1.0 / abs(v) * v1.get_error();

    if (v1.get_value().is_one() == false)
        err     = err + 0.5;

    return error_value(v, err.get_double());
};

error_value testing::operator*(const error_value& v1, const error_value& v2)
{
    value v     = v1.get_value() * v2.get_value();

    if (v.is_zero() == true)
        return error_value(v, 0.0);

    double err  = v1.get_error() + v2.get_error() + 0.5;
    return error_value(v, err);
};

error_value testing::operator*(const value& v1, const error_value& v2)
{
    value v     = v1 * v2.get_value();

    if (v.is_zero() == true)
        return error_value(v, 0.0);

    double err  = v2.get_error() + 0.5;
    return error_value(v, err);
};

error_value testing::operator*(const error_value& v1, const value& v2)
{
    value v     = v1.get_value() * v2;

    if (v.is_zero() == true)
        return error_value(v, 0.0);

    double err  = v1.get_error() + 0.5;
    return error_value(v, err);
};

error_value testing::operator/(const error_value& v1, const error_value& v2)
{
    value min2      = v2.get_value() - v2.get_abs_error();
    value max2      = v2.get_value() + v2.get_abs_error();

    value zero      = value::make_zero();
    double inf      = std::numeric_limits<double>::infinity();

    if (min2 <= zero && max2 >= zero)
        return error_value(value::make_nan(), inf);

    value v     = v1.get_value() / v2.get_value();    
    double err  = v1.get_error() + v2.get_error();

    if (v1.get_value() != v2.get_value() && v1.get_value().is_zero() == false)
        err     = err + 0.5;

    return error_value(v, err);
};

error_value testing::operator+(const error_value& v1, const error_value& v2)
{
    value v     = v1.get_value() + v2.get_value();

    if (v.is_zero() == true)
    {
        if (v1.get_error() != 0.0 || v2.get_error() != 0.0)
            return error_value(v, std::numeric_limits<double>::infinity());
        else
            return error_value(v, 0.0);
    };

    value err   = abs(v1.get_value() / v) * v1.get_error() 
                + abs(v2.get_value() / v) * v2.get_error();

    if (v1.get_value().is_zero() == false && v2.get_value().is_zero() == false
        && v.is_zero() == false)
    {
        err     = err + 0.5;
    }

    return error_value(v, err.get_double());
};

error_value testing::operator-(const error_value& v1, const error_value& v2)
{
    value v     = v1.get_value() - v2.get_value();

    if (v.is_zero() == true)
    {
        if (v1.get_error() != 0.0 || v2.get_error() != 0.0)
            return error_value(v, std::numeric_limits<double>::infinity());
        else
            return error_value(v, 0.0);
    };

    value err   = abs(v1.get_value() / v) * v1.get_error() 
                + abs(v2.get_value() / v) * v2.get_error();

    if (v1.get_value().is_zero() == false && v2.get_value().is_zero() == false
        && v.is_zero() == false)
    {
        err     = err + 0.5;
    }

    return error_value(v, err.get_double());
};

error_value testing::operator-(const error_value& v1, const value& v2)
{
    value v     = v1.get_value() - v2;

    if (v.is_zero() == true)
    {
        if (v1.get_error() != 0.0)
            return error_value(v, std::numeric_limits<double>::infinity());
        else
            return error_value(v, 0.0);
    };

    value err  = abs(v1.get_value() / v) * v1.get_error();

    if (v1.get_value().is_zero() == false && v2.is_zero() == false
        && v.is_zero() == false)
    {
        err     = err + 0.5;
    }

    return error_value(v, err.get_double());
};

error_value testing::operator-(const value& v1, const error_value& v2)
{
    value v     = v1 - v2.get_value();

    if (v.is_zero() == true)
    {
        if (v2.get_error() != 0.0)
            return error_value(v, std::numeric_limits<double>::infinity());
        else
            return error_value(v, 0.0);
    };

    value err   = abs(v2.get_value() / v) * v2.get_error();

    if (v1.is_zero() == false && v2.get_value().is_zero() == false
        && v.is_zero() == false)
    {
        err     = err + 0.5;
    }

    return error_value(v, err.get_double());
};

static value max(const value& x, const value& y)
{
    return (x < y) ? y : x;
};

static value min(const value& x, const value& y)
{
    return (x < y) ? x : y;
};

// real power |v1| ^ v2
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
error_value testing::power_real(const error_value& v1, const error_value& v2)
{
    double inf      = std::numeric_limits<double>::infinity();

    value nan       = value::make_nan();
    double d_nan    = std::numeric_limits<double>::quiet_NaN();
    double d_inf    = std::numeric_limits<double>::infinity();

    if (v1.get_value().is_nan() == true)
        return error_value(nan, d_nan);

    if (v2.get_value().is_nan() == true)
        return error_value(nan, d_nan);

    if (std::isnan(v1.get_error()) == true)
        return error_value(nan, d_nan);

    if (std::isnan(v2.get_error()) == true)
        return error_value(nan, d_nan);

    value min10     = v1.get_value() - v1.get_abs_error();
    value max10     = v1.get_value() + v1.get_abs_error();
    value min1      = min(abs(min10), abs(max10));
    value max1      = max(abs(min10), abs(max10));

    value zero      = value::make_zero();
    value one       = value::make_one();

    if (min1 <= zero && max1 >= 0)
        min1        = zero;

    value min2      = v2.get_value() - v2.get_abs_error();
    value max2      = v2.get_value() + v2.get_abs_error();

    bool v2_is_negative     = (max2 < 0.0);
    bool v2_is_positive     = (min2 > 0.0);

    if (v1.get_value().is_inf() == true)
    {
        if (v2_is_negative == true)
            return error_value(zero, 0.0);
        else if (v2_is_positive == true)
            return error_value(inf, d_inf);
        else
            return error_value(nan, d_nan);
    }

    bool v1_is_less_1       = (max2 < 1.0);
    bool v1_is_greater_1    = (min2 > 1.0);

    if (v2.get_value().is_inf_minus() == true)
    {
        if (v1_is_less_1 == true)
            return error_value(inf, d_inf);
        else if (v1_is_greater_1 == true)
            return error_value(zero, 0.0);
        else
            return error_value(nan, d_nan);
    }

    if (v2.get_value().is_inf_plus() == true)
    {
        if (v1_is_less_1 == true)
            return error_value(zero, 0.0);            
        else if (v1_is_greater_1 == true)
            return error_value(inf, d_inf);
        else
            return error_value(nan, d_nan);
    }

    if (v1.is_finite() == false)
        return error_value(value::make_nan(), inf);

    if (v2.is_finite() == false)
        return error_value(value::make_nan(), inf);
    
    value v         = power_real(v1.get_value(), v2.get_value());
    value err       = abs(v2.get_value()) * v1.get_error()
                    + v2.get_value() * abs(log(abs(v1.get_value()))) * v2.get_error();
    
    if (v2.get_value().is_zero() == false && v2.get_value().is_one() == false
            && v1.get_value().is_zero() == false && v1.get_value().is_one() == false)
    {
        value base  = abs(log(abs(v1.get_value())) * v2.get_value());
        err         = err + base * 1.0 + 0.5;
    }
    // |~0| ^ x, x > 0  -> ~ 0    
    // |~0| ^ x, x < 0  -> +Inf
    // |~0| ^ ~0        -> NaN
    if (min1.is_zero() == true)
    {
        if (v1.get_error() == 0.0) //v1 is exactly zero
        {
            // 0 ^ +x = 0
            if (min2 > 0.0)
                return error_value(value::make_zero(), 0.0);

            if (max2 < 0.0)
                return error_value(inf, d_inf);

            // 0 ^ 0 -> NaN, 0 ^ (+-eps) -> NaN
            return error_value(value::make_nan(), inf);
        };

        // eps ^ +x = 0
        if (min2 > zero)
            return error_value(v, err.get_double());

        // 0 ^-x = inf; result is v, but error is infinite
        return error_value(v, std::numeric_limits<double>::infinity());
    };

    // x ^ ~0           -> ~ 1    
    if (min2 <= zero && max2 >= zero)
    {
        if (v2.get_error() == 0.0) //v2 is exactly zero
            return error_value(value::make_one(), 0.0);
        else
            return error_value(v, err.get_double());
    };

    // |~1| ^ x, x > 0  -> ~ 1    
    // |~1| ^ x, x < 0  -> ~ 1
    // |~1| ^ ~0        -> ~ 1
    if (min1 <= one && max1 >= one)
    {
        if (v1.get_error() == 0.0) //v1 is exactly one
            return error_value(value::make_one(), 0.0);
        else
            return error_value(v, err.get_double());
    };
    
    // x   ^ ~1, x > 0  -> x
    // |~0|^ ~1         -> ~ 0
    if (min2 <= one && max2 >= one)
    {
        // default implementation is OK
        return error_value(v, err.get_double());
    };

    return error_value(v, err.get_double());
};

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
error_value testing::power_int(const error_value& v1, int v2)
{
    double d_inf    = std::numeric_limits<double>::infinity();
    double d_nan    = std::numeric_limits<double>::quiet_NaN();

    value zero      = value::make_zero();
    value one       = value::make_one();    
    value nan       = value::make_nan();
    value inf       = value::make_inf_plus();

    if (v1.get_value().is_nan() == true)
        return error_value(nan, d_nan);

    if (std::isnan(v1.get_error()) == true)
        return error_value(nan, d_nan);

    bool v2_even    = (v2 % 2 == 0);
    bool v2_odd     = (v2 % 2 != 0);

    if (v1.get_value().is_inf_minus() == true)
    {
        if (v2 < 0)
            return error_value(zero, 0.0);
        if (v2 > 0 && v2_odd)
            return error_value(-inf, d_inf);
        if (v2 > 0 && v2_even)
            return error_value(inf, d_inf);

        return error_value(nan, d_nan);
    }

    if (v1.get_value().is_inf_plus() == true)
    {
        if (v2 < 0)
            return error_value(zero, 0.0);
        if (v2 > 0)
            return error_value(inf, d_inf);

        return error_value(nan, d_nan);
    }

    if (v1.is_finite() == false)
        return error_value(value::make_nan(), d_inf);

    value min1      = v1.get_value() - v1.get_abs_error();
    value max1      = v1.get_value() + v1.get_abs_error();

    value v     = power_int(v1.get_value(), v2);
    double err  = std::abs(v2) * v1.get_error();

    if (v2 != 0 && v2 != 1 && v1.get_value().is_zero() == false 
            && v1.get_value().is_one() == false)
    {
        value base  = abs(log(abs(v1.get_value())) * double(v2));
        err         = err + base.get_double() * 1.0 + 0.5;
    }

    // power_int(+-0, y)    -> NaN for y < 0 and 0.0 for y > 0
    // power_int(0, 0)      -> NaN
    if (min1 <= zero && max1 >= zero)
    {
        // inf cannot be exact
        if (v2 < 0)
            return error_value(value::make_nan(), d_inf);

        if (v2 == 0)
            return error_value(value::make_nan(), d_inf);

        if (v1.get_error() == 0.0) //v1 is exactly zero
            return error_value(value::make_zero(), 0.0);
        else
            return error_value(v, err);
    }

    // power_int(x, 0)      -> 1
    if (v2 == 0)
        return error_value(value::make_one(), 0.0);

    return error_value(v, err);
};

};};
