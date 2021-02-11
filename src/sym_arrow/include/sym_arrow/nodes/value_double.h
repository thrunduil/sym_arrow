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

#include "sym_arrow/config.h"
#include "sym_arrow/details/dag_traits.h"

#include <iosfwd>

namespace sym_arrow { namespace details
{

void initialize_values();
void initialize_values(ast::details::value_context_data& );
void close_values(ast::details::value_context_data& );

}}

namespace sym_arrow
{

#pragma warning(push)
#pragma warning(disable: 4251)  // needs to have dll-interface to be used by clients

// floating point value represented using floating point implementation
class SYM_ARROW_EXPORT value
{
    private:
        using float_type        = double;

    public:
        using impl_type         = double;

        // value handle type
        using handle_type       = value;

        // value storing mp_float is not (effectively) a POD type
        static const 
        bool is_pod             = true;

    private:        
        float_type              m_data;

    public:     
        // initialization with 0.0
        value();

        // convert from double to value
        value(double v);

        // calculate hash of value object
        static size_t           eval_hash(const value& val);

        // return true if val is equal to this value
        bool                    equal(const value& val) const;

        // return hash of this object
        size_t                  hash_value() const;

    public:
        // access to internal pointer; internal use only
        handle_type             get_handle() const;

        // convert this value to double
        double                  get_double() const;

    public:        
        // make object of class value from a double scalar
        static value            make_value(double val);
        
        // make NaN
        static value            make_nan();

        // make +Inf
        static value            make_inf_plus();

        // make -Inf
        static value            make_inf_minus();

        // make 0 value; faster than make_value(0)
        static value            make_zero();
        
        // make 1 value; faster than make_value(1)
        static value            make_one();

        // make -1 value; faster than make_value(-1)
        static value            make_minus_one();        

        // return true if this value represents NaN
        bool                    is_nan() const;

        // return true if this value represents +Inf of -Int
        bool                    is_inf() const;

        // return true if this value represents +Inf
        bool                    is_inf_plus() const;

        // return true if this value represents -Inf
        bool                    is_inf_minus() const;

        // return true if this value represents 0
        bool                    is_zero() const;

        // return true if this value represents 1
        bool                    is_one() const;

        // return true if this value represents -1
        bool                    is_minus_one() const;

        // return true if this value represents e (i.e. exp(1))
        bool                    is_e() const;

        // return true if this value represents positive number
        bool                    is_positive() const;

        // return true if this value represents negative number
        bool                    is_negative() const;

        // return true if this value represents a finite number
        bool                    is_finite() const;

        // return true if this value represents an integer
        bool                    is_int() const;

        // convert this value to an integer with rounding to nearest
        int                     convert_int() const;

        // multiply assign
        value&                  operator*=(const value& v);

        // add assign
        value&                  operator+=(const value& v);
};

// return true if v1 == v2 or v1 and v2 are both NaN
SYM_ARROW_EXPORT bool    operator==(const value& v1,const value& v2);

// return true if v1 != v2 and v1 and v2 are not both NaN
SYM_ARROW_EXPORT bool    operator!=(const value& v1,const value& v2);

SYM_ARROW_EXPORT bool    operator>(const value& v1,const value& v2);
SYM_ARROW_EXPORT bool    operator<(const value& v1,const value& v2);
SYM_ARROW_EXPORT bool    operator<=(const value& v1,const value& v2);
SYM_ARROW_EXPORT bool    operator>=(const value& v1,const value& v2);

// addition v1 + v2
SYM_ARROW_EXPORT value   operator+(const value& v1, const value& v2);
SYM_ARROW_EXPORT value   operator+(const value& v1, const double& v2);
SYM_ARROW_EXPORT value   operator+(const double& v1, const value& v2);

// subtraction v1 - v2
SYM_ARROW_EXPORT value   operator-(const value& v1, const value& v2);
SYM_ARROW_EXPORT value   operator-(const value& v1, const double& v2);
SYM_ARROW_EXPORT value   operator-(const double& v1, const value& v2);

// multiplication v1 * v2
SYM_ARROW_EXPORT value   operator*(const value& v1, const value& v2);
SYM_ARROW_EXPORT value   operator*(const value& v1, const double& v2);
SYM_ARROW_EXPORT value   operator*(const double& v1, const value& v2);

// division v1 / v2; x/0 gives NaN for any x
SYM_ARROW_EXPORT value   operator/(const value& v1, const value& v2);
SYM_ARROW_EXPORT value   operator/(const value& v1, const double& v2);
SYM_ARROW_EXPORT value   operator/(const double& v1, const value& v2);

// unary minus
SYM_ARROW_EXPORT value   operator-(const value& v1);

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
SYM_ARROW_EXPORT value   power_int(const value& v1, int v2);

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
SYM_ARROW_EXPORT value   power_real(const value& v1, const value& v2);

// inverse v1 ^ (-1); inv(0) gives NaN
SYM_ARROW_EXPORT value   inv(const value& v1);

// exponential exp(v1)
SYM_ARROW_EXPORT value   exp(const value& v1);

// logarithm log(|v1|)
SYM_ARROW_EXPORT value   log(const value& v1);

// absolute value |v1|
SYM_ARROW_EXPORT value   abs(const value& v1);

// print a value v to a stream os; add new line at the end
// if add_newline = true
SYM_ARROW_EXPORT void    disp(std::ostream& os, const value& v, 
                            bool add_newline = true);

// print a value v to the cout stream; add new line at the end
// if add_newline = true
SYM_ARROW_EXPORT void    disp(const value& v, bool add_newline = true);

SYM_ARROW_EXPORT double  float_distance(const value& v1, const value& v2);

};

#include "sym_arrow/details/value_double.inl"
