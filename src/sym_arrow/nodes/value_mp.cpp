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

#include "sym_arrow/ast/ast_tags.h"
#include "sym_arrow/nodes/value_functions_common.h"
#include "sym_arrow/ast/value_context_data.h"

#include <iostream>

namespace sym_arrow { namespace details
{
    class value_mp : public sym_dag::dag_item<details::value_mp, ast::value_tag, false>
    {
        public:
            using impl_type     = matcl::mp_float;
            using base_type     = sym_dag::dag_item<details::value_mp, ast::value_tag, false>;

        public:
            impl_type           m_data; 
            mutable size_t      m_hash;

        public:
            value_mp(double v)
                : base_type(this), m_data(v), m_hash(0)
            {};

            value_mp(const impl_type& v)
                : base_type(this), m_data(v), m_hash(0)
            {};

            size_t get_hash() const
            {
                if (m_hash != 0)
                    return m_hash;

                m_hash = eval_hash(m_data);
                return m_hash;
            };
            
            static size_t eval_hash(const impl_type& val)
            {
                size_t seed = matcl::hash_value(val);
                return seed;
            }
    };

    void details::initialize_values(ast::details::value_context_data& context)
    {
        context.val_zero_ptr    = new details::value_mp(0.0);
        context.val_one_ptr     = new details::value_mp(1.0);
        context.val_mone_ptr    = new details::value_mp(-1.0);
        context.val_nan_ptr     = new details::value_mp(std::numeric_limits<double>::quiet_NaN());
    };

    void details::close_values(ast::details::value_context_data& context)
    {
        delete context.val_zero_ptr;
        delete context.val_one_ptr;
        delete context.val_mone_ptr;
        delete context.val_nan_ptr;
    };

    void details::initialize_values()
    {
        // force initialization
        sym_dag::dag_context<ast::value_tag>::get().get_context_data();
    }

    static inline
    ast::details::value_context_data&
    get_context()
    {
        return sym_dag::dag_context<ast::value_tag>::get().get_context_data();
    };

}}

namespace sym_arrow
{

value::value()
    : value(details::get_context().val_zero_ptr)
{};

value::value(double v)
    :value(mp_float(v))
{}

value::value(const mp_float& v)
    : m_ptr(item_type::make(v))
{};

value::value(handle_type h)
    :m_ptr(ptr_type::from_this(h))
{};

value::value(const value& other)
    :m_ptr(other.m_ptr)
{};

value::value(value&& other)
    :m_ptr(std::move(other.m_ptr))
{};

value::~value()
{}

value& value::operator=(const value& other)
{
    m_ptr = other.m_ptr;
    return *this;
}

value& value::operator=(value&& other)
{
    m_ptr = std::move(other.m_ptr);
    return *this;
}

value value::make_value(double val)
{
    return value(mp_float(val));
};

value value::make_value(const matcl::mp_float& val)
{
    return value(val);
};

value value::make_from_string(const std::string& s)
{
    return value(mp_float(s));
}

double value::get_double() const
{ 
    return m_ptr.get()->m_data.cast_float();
};

const mp_float& value::get_mp_float() const
{ 
    return m_ptr.get()->m_data;
};

value value::make_nan()
{
    return value(details::get_context().val_nan_ptr);
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
    return value(details::get_context().val_one_ptr);
};

value value::make_minus_one()
{
    return value(details::get_context().val_mone_ptr);
};

value value::make_zero()
{
    return value(details::get_context().val_zero_ptr);
};

bool value::is_nan() const
{ 
    return matcl::is_nan(this->get_mp_float());
};

bool value::is_inf() const
{
    return matcl::is_inf(this->get_mp_float());
};

bool value::is_inf_plus() const
{
    return matcl::is_inf(this->get_mp_float()) && this->get_mp_float() > 0.0;
};

bool value::is_inf_minus() const
{
    return matcl::is_inf(this->get_mp_float()) && this->get_mp_float() < 0.0;
};

bool value::is_finite() const
{
    return matcl::is_finite(this->get_mp_float());
};

bool value::is_int() const
{
    return matcl::is_int(this->get_mp_float());
};

int value::convert_int() const
{
    return this->get_mp_float().cast_int(matcl::round_mode::nearby);
}

size_t value::eval_hash(const value& val)
{
    return val.m_ptr->get_hash();
};

bool value::is_e() const
{ 
    return this->get_mp_float() == matcl::constants::mp_e(this->get_mp_float().get_precision());
}

//---------------------------------------------------------------------
//                  functions defined for value
//---------------------------------------------------------------------
value sym_arrow::operator+(const value& v1, const value& v2)
{
    return value(v1.get_mp_float() + v2.get_mp_float());
};

value sym_arrow::operator+(const value& v1, const double& v2)
{
    if (v2 == 0.0)
        return v1;
    else
        return value(v1.get_mp_float() + v2);
};

value sym_arrow::operator+(const double& v1, const value& v2)
{
    if (v1 == 0.0)
        return v2;
    else
        return value(v1 + v2.get_mp_float());
};

value sym_arrow::operator*(const value& v1, const value& v2)
{ 
    return value(v1.get_mp_float() * v2.get_mp_float());
};

value sym_arrow::operator*(const value& v1, const double& v2)
{ 
    if (v2 == 1.0)
        return v1;
    else
        return value(v1.get_mp_float() * v2);
};

value sym_arrow::operator*(const double& v1, const value& v2)
{ 
    if (v1 == 1.0)
        return v2;
    else
        return value(v1 * v2.get_mp_float());
};

value sym_arrow::operator/(const value& v1, const value& v2)
{ 
    if (v2.is_zero())
        return value::make_nan();

    return value(v1.get_mp_float() / v2.get_mp_float());
};

value sym_arrow::operator/(const value& v1, const double& v2)
{ 
    if (v2 == 0.0)
        return value::make_nan();

    if (v2 == 1.0)
        return v1;
    else
        return value(v1.get_mp_float() / v2);
};

value sym_arrow::operator/(const double& v1, const value& v2)
{ 
    if (v2.is_zero())
        return value::make_nan();

    return value(v1 / v2.get_mp_float());
};

value sym_arrow::operator-(const value& v1, const value& v2)
{ 
    return value(v1.get_mp_float() - v2.get_mp_float());
};

value sym_arrow::operator-(const value& v1, const double& v2)
{ 
    if (v2 == 0.0)
        return v1;
    else
        return value(v1.get_mp_float() - v2);
};

value sym_arrow::operator-(const double& v1, const value& v2)
{ 
    return value(v1 - v2.get_mp_float());
};

value sym_arrow::operator-(const value& v1)
{ 
    return value(-v1.get_mp_float());
};

bool sym_arrow::operator==(const value& v1,const value& v2)
{ 
    return eeq_nan(v1.get_mp_float(), v2.get_mp_float());
};

bool sym_arrow::operator!=(const value& v1,const value& v2)
{
    return neq_nan(v1.get_mp_float(), v2.get_mp_float());
};

bool sym_arrow::operator>(const value& v1,const value& v2)
{
    if (v1.is_nan() == true && v2.is_nan() == false)
        return true;

    return v1.get_mp_float() > v2.get_mp_float();
};

bool sym_arrow::operator<(const value& v1,const value& v2)
{ 
    if (v1.is_nan() == false && v2.is_nan() == true)
        return true;

    return v1.get_mp_float() < v2.get_mp_float();
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
    bool computed;
    value res   = details::special_cases_power_int(v1, v2, computed);

    if (computed == true)
        return res;

    return (v2 == 1) ? v1 : value(pow(v1.get_mp_float(), v2));
};

value sym_arrow::power_real(const value& v1, const value& v2)
{
    bool computed;
    value res   = details::special_cases_power_real(v1, v2, computed);

    if (computed == true)
        return res;

    return value(pow(abs(v1.get_mp_float()), v2.get_mp_float()));
};

value sym_arrow::inv(const value& v1)
{ 
    if (v1.is_zero())
        return value::make_nan();

    return value(inv(v1.get_mp_float()));
};

value sym_arrow::exp(const value& v1)
{ 
    return value(exp(v1.get_mp_float()));
};

value sym_arrow::log(const value& v1)
{ 
    return value(log(abs(v1.get_mp_float())));
};

value sym_arrow::abs(const value& v1)
{ 
    return value(abs(v1.get_mp_float()));
};

void sym_arrow::disp(const value& v, bool add_nl)
{
    disp(std::cout, v, add_nl);
};

void sym_arrow::disp(std::ostream& os, const value& v, bool add_nl)
{
    int prec    = get_disp_precision();

    if (prec < 0)
        os << v.get_mp_float().to_string();
    else if (prec == 0)
        os << v.get_mp_float();
    else
        os << v.get_mp_float().to_string(matcl::precision(prec));

    if (add_nl)
        os << "\n";
}

double sym_arrow::float_distance(const value& v1, const value& v2)
{
    return matcl::ulp_distance(v1.get_mp_float(), v2.get_mp_float()).cast_float();
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