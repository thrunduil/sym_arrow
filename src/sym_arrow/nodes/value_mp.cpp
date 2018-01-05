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

#include "sym_arrow/config.h"

#if SYM_ARROW_VALUE_TYPE == SYM_ARROW_VALUE_MP

#include "sym_arrow/nodes/value_mp.h"
#include <boost/functional/hash.hpp>
#include "sym_arrow/functions/expr_functions.h"

#include <iostream>

namespace sym_arrow
{

value::value()
    :m_data(0.0)
{};

value::value(double v)
    :value(impl_type(v))
{}

value::value(const mp_float& v)
    :m_data(v)
{};

value value::make_value(double val)
{
    return value(impl_type(val));
};

value value::make_value(const matcl::mp_float& val)
{
    return value(impl_type(val));
};

value value::make_from_string(const std::string& s)
{
    return value(impl_type(s));
}

const value::impl_type& value::get_rep() const
{
    return m_data;
}

double value::get_double() const
{ 
    return m_data.cast_float();
};

const mp_float& value::get_mp_float() const
{ 
    return m_data;
};

value value::make_nan()
{
    return value(matcl::constants::mp_nan());
};

value value::make_inf_plus()
{
    return value(matcl::constants::mp_inf());
};

value value::make_inf_minus()
{
    return value(-matcl::constants::mp_inf());
};

value value::make_one()
{
    return value(matcl::mp_float(1.0));
};

value value::make_minus_one()
{
    return value(matcl::mp_float(-1.0));
};

value value::make_zero()
{
    return value(matcl::mp_float(0.0));
};

bool value::is_nan() const
{ 
    return matcl::is_nan(m_data);
};

bool value::is_inf() const
{
    return matcl::is_inf(m_data);
};

bool value::is_inf_plus() const
{
    return matcl::is_inf(m_data) && m_data > 0.0;
};

bool value::is_inf_minus() const
{
    return matcl::is_inf(m_data) && m_data < 0.0;
};

bool value::is_finite() const
{
    return matcl::is_finite(m_data);
};

bool value::is_int() const
{
    return matcl::is_int(m_data);
};

int value::convert_int() const
{
    return m_data.cast_int(matcl::round_mode::nearby);
}

size_t value::eval_hash(const value& val)
{
    //TODO
    double vd           = val.get_double();
    const size_t* tmp   = reinterpret_cast<const size_t*>(&vd);

    size_t seed = 0;

    for(size_t i = 0; i < sizeof(double) / sizeof(size_t); ++i)
        boost::hash_combine(seed,tmp[i]);

    return seed;
};

bool value::is_e() const
{ 
    return m_data == matcl::constants::mp_e(m_data.get_precision());
}

//---------------------------------------------------------------------
//                  functions defined for value
//---------------------------------------------------------------------
value sym_arrow::operator+(const value& v1, const value& v2)
{
    return value(v1.get_rep() + v2.get_rep());
};

value sym_arrow::operator+(const value& v1, const double& v2)
{
    return value(v1.get_rep() + v2);
};

value sym_arrow::operator+(const double& v1, const value& v2)
{
    return value(v1 + v2.get_rep());
};

value sym_arrow::operator*(const value& v1, const value& v2)
{ 
    return value(v1.get_rep() * v2.get_rep());
};

value sym_arrow::operator*(const value& v1, const double& v2)
{ 
    return value(v1.get_rep() * v2);
};

value sym_arrow::operator*(const double& v1, const value& v2)
{ 
    return value(v1 * v2.get_rep());
};

value sym_arrow::operator/(const value& v1, const value& v2)
{ 
    return value(v1.get_rep() / v2.get_rep());
};

value sym_arrow::operator/(const value& v1, const double& v2)
{ 
    return value(v1.get_rep() / v2);
};

value sym_arrow::operator/(const double& v1, const value& v2)
{ 
    return value(v1 / v2.get_rep());
};

value sym_arrow::operator-(const value& v1, const value& v2)
{ 
    return value(v1.get_rep() - v2.get_rep());
};

value sym_arrow::operator-(const value& v1, const double& v2)
{ 
    return value(v1.get_rep() - v2);
};

value sym_arrow::operator-(const double& v1, const value& v2)
{ 
    return value(v1 - v2.get_rep());
};

value sym_arrow::operator-(const value& v1)
{ 
    return value(-v1.get_rep());
};

bool sym_arrow::operator==(const value& v1,const value& v2)
{ 
    return eeq_nan(v1.get_rep(), v2.get_rep());
};

bool sym_arrow::operator!=(const value& v1,const value& v2)
{
    return neq_nan(v1.get_rep(), v2.get_rep());
};

bool sym_arrow::operator>(const value& v1,const value& v2)
{
    if (v1.is_nan() == true && v2.is_nan() == false)
        return true;

    return v1.get_rep() > v2.get_rep();
};

bool sym_arrow::operator<(const value& v1,const value& v2)
{ 
    if (v1.is_nan() == false && v2.is_nan() == true)
        return true;

    return v1.get_rep() < v2.get_rep();
};    

bool sym_arrow::operator<=(const value& v1,const value& v2)
{ 
    return (v1 > v2) == false;
};    

bool sym_arrow::operator>=(const value& v1,const value& v2)
{ 
    return (v1 < v2) == false;
};    

value sym_arrow::power_int(const value& v1, int v2)
{ 
    return value(pow(v1.get_rep(), v2));
};

value sym_arrow::power_real(const value& v1, const value& v2)
{
    return value(pow(abs(v1.get_rep()), v2.get_rep()));
};

value sym_arrow::inv(const value& v1)
{ 
    return value(inv(v1.get_rep()));
};

value sym_arrow::exp(const value& v1)
{ 
    return value(exp(v1.get_rep()));
};

value sym_arrow::log(const value& v1)
{ 
    return value(log(abs(v1.get_rep())));
};

value sym_arrow::abs(const value& v1)
{ 
    return value(abs(v1.get_rep()));
};

void sym_arrow::disp(const value& v, bool add_nl)
{
    disp(std::cout, v, add_nl);
};

void sym_arrow::disp(std::ostream& os, const value& v, bool add_nl)
{
    int prec    = get_disp_precision();

    if (prec < 0)
        os << v.get_rep().to_string();
    else if (prec == 0)
        os << v.get_rep();
    else
        os << v.get_rep().to_string(matcl::precision(prec));

    if (add_nl)
        os << "\n";
}

double sym_arrow::float_distance(const value& v1, const value& v2)
{
    return matcl::ulp_distance(v1.get_rep(), v2.get_rep()).cast_float();
}

int sym_arrow::get_default_precision()
{
    return (int)matcl::mp_float::get_default_precision().get();
}

void sym_arrow::set_default_precision(int prec)
{
    matcl::mp_float::set_default_precision(matcl::precision(prec));
}

};

#endif