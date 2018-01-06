/* 
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

bool error_value::is_finite() const
{
    return m_value.is_finite() && std::isfinite(m_error);
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
    value v     = v1.get_value() / v2.get_value();

    if (v.is_zero() == true)
        return error_value(v, 0.0);
    
    double err  = v1.get_error() + v2.get_error();

    if (v1.get_value() != v2.get_value())
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

error_value testing::power_int(const error_value& v1, int v2)
{
    value v     = power_int(v1.get_value(), v2);
    double err  = std::abs(v2) * v1.get_error();

    if (v2 != 0 && v2 != 1 && v1.get_value().is_zero() == false 
            && v1.get_value().is_one() == false)
    {
        value base  = abs(log(abs(v1.get_value())) * double(v2));
        err         = err + base.get_double() * 1.0 + 0.5;
    }

    return error_value(v, err);
};

error_value testing::power_real(const error_value& v1, const error_value& v2)
{
    //TODO: special cases
    value v     = power_real(v1.get_value(), v2.get_value());
    value err   = abs(v2.get_value()) * v1.get_error()
                + v2.get_value() * abs(log(abs(v1.get_value()))) * v2.get_error();

    if (v2.get_value().is_zero() == false && v2.get_value().is_one() == false
            && v1.get_value().is_zero() == false && v1.get_value().is_one() == false)
    {
        value base  = abs(log(abs(v1.get_value())) * v2.get_value());
        err         = err + base * 1.0 + 0.5;
    }

    return error_value(v, err.get_double());
};

};};
