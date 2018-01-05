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

#include "sym_arrow/nodes/value_double.h"
#include <limits>

namespace sym_arrow
{

//---------------------------------------------------------------------
//                  value
//---------------------------------------------------------------------
inline value::value()
    :m_data(0.0)
{};

inline value::value(double v)
    :m_data(v)
{}

inline value value::make_value(double val)
{
    return value(val);
};

inline size_t value::hash_value() const
{ 
    return eval_hash(*this); 
};

inline bool value::equal(const value& val) const
{ 
    return operator==(*this,val); 
};

inline value& value::operator*=(const value& v)
{ 
    return *this = operator*(*this, v); 
};

inline value& value::operator+=(const value& v)
{ 
    return *this = operator+(*this, v); 
};

inline bool value::is_zero() const
{ 
    return m_data == 0.0;
}

inline bool value::is_one() const
{ 
    return m_data == 1.0;
}

inline bool value::is_minus_one() const
{ 
    return m_data == -1.0;
}

inline bool value::is_positive() const
{ 
    return m_data > 0.0;
}

inline bool value::is_negative() const
{ 
    return m_data < 0.0;
}

inline double value::get_double() const
{ 
    return m_data;
};

inline value value::make_one()
{
    return value(1.0);
};

inline value value::make_minus_one()
{
    return value(-1.0);
};

inline value value::make_zero()
{
    return value(0.0);
};

//---------------------------------------------------------------------
//                  value functions
//---------------------------------------------------------------------

inline value sym_arrow::operator+(const value& v1, const value& v2)
{
    return value(v1.get_double() + v2.get_double());
};

inline value sym_arrow::operator+(const value& v1, const double& v2)
{
    return value(v1.get_double() + v2);
};

inline value sym_arrow::operator+(const double& v1, const value& v2)
{
    return value(v1 + v2.get_double());
};

inline value sym_arrow::operator-(const value& v1, const value& v2)
{ 
    return value(v1.get_double() - v2.get_double());
};

inline value sym_arrow::operator-(const value& v1, const double& v2)
{ 
    return value(v1.get_double() - v2);
};

inline value sym_arrow::operator-(const double& v1, const value& v2)
{ 
    return value(v1 - v2.get_double());
};

inline value sym_arrow::operator-(const value& v1)
{ 
    return value(-v1.get_double());
};

inline value sym_arrow::operator*(const value& v1, const value& v2)
{ 
    return value(v1.get_double() * v2.get_double());
};

inline value sym_arrow::operator*(const value& v1, const double& v2)
{ 
    return value(v1.get_double() * v2);
};

inline value sym_arrow::operator*(const double& v1, const value& v2)
{ 
    return value(v1 * v2.get_double());
};

inline value sym_arrow::operator/(const value& v1, const value& v2)
{ 
    return value(v1.get_double() / v2.get_double());
};

inline value sym_arrow::operator/(const value& v1, const double& v2)
{ 
    return value(v1.get_double() / v2);
};

inline value sym_arrow::operator/(const double& v1, const value& v2)
{ 
    return value(v1 / v2.get_double());
};

inline bool sym_arrow::operator==(const value& v1,const value& v2)
{ 
    if (v1.is_nan() == true && v2.is_nan() == true)
        return true;

    return v1.get_double() == v2.get_double();
};

inline bool sym_arrow::operator!=(const value& v1,const value& v2)
{
    return (v1 == v2) == false;
};

inline bool sym_arrow::operator>(const value& v1,const value& v2)
{
    if (v1.is_nan() == true && v2.is_nan() == false)
        return true;

    return v1.get_double() > v2.get_double();
};

inline bool sym_arrow::operator<(const value& v1,const value& v2)
{ 
    if (v1.is_nan() == false && v2.is_nan() == true)
        return true;

    return v1.get_double() < v2.get_double();
};    

inline bool sym_arrow::operator<=(const value& v1,const value& v2)
{ 
    return (v1 > v2) == false;
};    

inline bool sym_arrow::operator>=(const value& v1,const value& v2)
{ 
    return (v1 < v2) == false;
};    

inline value sym_arrow::power_int(const value& v1, int v2)
{ 
    return value(std::pow(v1.get_double(), v2));
};

inline value sym_arrow::power_real(const value& v1, const value& v2)
{
    return value(std::pow(std::abs(v1.get_double()), v2.get_double()));
};

inline value sym_arrow::inv(const value& v1)
{ 
    return value(1.0 / v1.get_double());
};

inline value sym_arrow::exp(const value& v1)
{ 
    return value(std::exp(v1.get_double()));
};

inline value sym_arrow::log(const value& v1)
{ 
    return value(std::log(std::abs(v1.get_double())));
};

inline value sym_arrow::abs(const value& v1)
{ 
    return value(std::abs(v1.get_double()));
};

};