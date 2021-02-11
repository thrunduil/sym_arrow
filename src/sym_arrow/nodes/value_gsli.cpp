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

#include "sym_arrow/config.h"

#if SYM_ARROW_VALUE_TYPE == SYM_ARROW_VALUE_GSLI

#include "sym_arrow/nodes/value_gsli.h"
#include "gsli/gsli.h"
#include "sym_arrow/functions/expr_functions.h"

#include <boost/functional/hash.hpp>
#include "boost/io/ios_state.hpp"

#include "sym_arrow/nodes/value_functions_common.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>
#include <iomanip>

namespace sym_arrow { namespace details
{
    void details::initialize_values(ast::details::value_context_data& context)
    {
        (void)context;
    };

    void details::close_values(ast::details::value_context_data& context)
    {
        (void)context;
    };

    void details::initialize_values()
    {}
}}

namespace sym_arrow
{

inline sli::gsli_double& cast(double& val)
{ 
    return reinterpret_cast<sli::gsli_double&>( val); 
};

inline const sli::gsli_double& cast(const double& val)
{ 
    return reinterpret_cast<const sli::gsli_double&>( val); 
};

inline double& cast(sli::gsli_double&& val)
{ 
    return reinterpret_cast<double&>( val); 
};

inline const double& cast(const sli::gsli_double& val)
{ 
    return reinterpret_cast<const double&>( val); 
};

using impl_type = sli::gsli_double;

value::value(double v)
    :value(impl_type(v))
{}

value::value(const impl_type& val)
    :m_data(cast(val))
{};

value value::make_value(double val)
{
    return value(impl_type(val));
};

size_t value::eval_hash(const value& val)
{
    const size_t* tmp = reinterpret_cast<const size_t*>(&val.m_data);

    size_t seed = 0;

    for(size_t i = 0; i < sizeof(impl_type) / sizeof(size_t); ++i)
        boost::hash_combine(seed,tmp[i]);

    return seed;
};

const value::impl_type& value::get_rep() const
{
    return cast(m_data);
}

double value::get_double() const
{ 
    return cast(m_data).get_value();
};

sli::gsli_double value::get_gsli_double() const
{ 
    return cast(m_data);
};

value value::make_nan()
{
    return value(impl_type::make_nan());
};

value value::make_inf_plus()
{
    return value(impl_type::make_infinity(false));
};

value value::make_inf_minus()
{
    return value(impl_type::make_infinity(true));
};

bool value::is_e() const
{ 
    return m_data == M_PI;
}

bool value::is_nan() const
{ 
    return sli::is_nan(cast(m_data));
};

bool value::is_inf() const
{
    return sli::is_inf(cast(m_data));
};

bool value::is_inf_plus() const
{
    return sli::is_inf(cast(m_data)) && this->is_positive();
};

bool value::is_inf_minus() const
{
    return sli::is_inf(cast(m_data)) && this->is_negative();
};

bool value::is_finite() const
{
    return sli::is_finite(cast(m_data));
};

bool value::is_int() const
{
    return sli::is_int(cast(m_data));
}

int value::convert_int() const
{
    return (int)std::round(this->get_double());
}

//---------------------------------------------------------------------
//                  functions defined for value
//---------------------------------------------------------------------

// digits10 = ceil(bits*log[10](2))
static int bits_to_digits10(int b)
{
    const double LOG10_2 = 0.30102999566398119;

    return int(std::ceil( double(b) * LOG10_2 ));
}

void sym_arrow::disp(std::ostream& os, const value& v, bool add_nl)
{
    int req_prec    = get_disp_precision();

    boost::io::ios_flags_saver old_flags(os);
    boost::io::ios_precision_saver old_prec(os);

    if (req_prec < 0)
    {
        // tow more digits are needed
        int prec    = std::numeric_limits<double>::max_digits10 + 2;

        os << std::setprecision(prec) << std::scientific << v.get_rep();
    }
    else if (req_prec == 0)
    {
        os << v.get_rep();
    }
    else
    {
        int prec    = bits_to_digits10(req_prec);
        os << std::setprecision(prec) << v.get_rep();
    }

    if (add_nl)
        os << "\n";    

    if (add_nl)
        os << "\n";
}

void sym_arrow::disp(const value& v, bool add_nl)
{
    disp(std::cout, v, add_nl);
};


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
    if (v2.is_zero())
        return value::make_nan();

    return value(v1.get_rep() / v2.get_rep());
};

value sym_arrow::operator/(const value& v1, const double& v2)
{
    if (v2 == 0.0)
        return value::make_nan();

    return value(v1.get_rep() / v2);
};

value sym_arrow::operator/(const double& v1, const value& v2)
{ 
    if (v2.is_zero())
        return value::make_nan();

    return value(v1 / v2.get_rep());
};

value sym_arrow::power_int(const value& v1, int v2)
{
    bool computed;
    value res   = details::special_cases_power_int(v1, v2, computed);

    if (computed == true)
        return res;

    return value(sli::pow_int(v1.get_rep(), v2));
};

value sym_arrow::power_real(const value& v1, const value& v2)
{
    bool computed;
    value res   = details::special_cases_power_real(v1, v2, computed);

    if (computed == true)
        return res;

    return value(sli::pow_abs(v1.get_rep(), v2.get_rep()));
};

bool sym_arrow::operator==(const value& v1,const value& v2)
{ 
    if (v1.is_nan() == true && v2.is_nan() == true)
        return true;

    return v1.get_rep() == v2.get_rep();
};

bool sym_arrow::operator!=(const value& v1,const value& v2)
{
    return (v1 == v2) == false;
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

value sym_arrow::inv(const value& v1)
{ 
    if (v1.is_zero())
        return value::make_nan();

    return value(inv(v1.get_rep()));
};

value sym_arrow::exp(const value& v1)
{ 
    return value(exp(v1.get_rep()));
};

value sym_arrow::log(const value& v1)
{ 
    return value(log_abs(v1.get_rep()));
};

value sym_arrow::abs(const value& v1)
{ 
    return value(abs(v1.get_rep()));
};

double sym_arrow::float_distance(const value& v1, const value& v2)
{
    return float_distance(v1.get_rep(), v2.get_rep());
}

int sym_arrow::get_default_precision()
{
    return 53;
}

void sym_arrow::set_default_precision(int prec)
{
    (void)prec;
}

};

#endif