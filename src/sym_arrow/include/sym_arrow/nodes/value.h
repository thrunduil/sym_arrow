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

#include "sym_arrow/config.h"

#include <iosfwd>

namespace sli
{
    class gsli_double;
};

namespace sym_arrow
{

#pragma warning(push)
#pragma warning(disable: 4251)  // needs to have dll-interface to be used by clients

// floating point value represented as sign * exp(v)
class SYM_ARROW_EXPORT value
{
    private:
        using float_type        = double;

    public:
        using impl_type         = sli::gsli_double;

    private:        
        float_type              m_data;

    public:     
        // initialization with 0.0
        value();

        // convert from double to value
        value(double v);

        // construtor from internal representation;
        // internal use only
        explicit value(const impl_type& val);

        // calculate hash of value object
        static size_t           eval_hash(const value& val);

        // calculate hash of real number; val is first converted to value
        static size_t           eval_hash(double val);

        // return true if val is equal to this value
        bool                    equal(const value& val) const;

        // return true if val is equal to this value; val is first converted
        // to value object
        bool                    equal(double val) const;

        // return hash of this object
        size_t                  hash_value() const;

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

        // return internal representation
        const impl_type&        get_rep() const;

        // convert this value to double
        double                  get_value()    const;

        // convert this value to double and return absolute value
        // of this number
        double                  get_value_abs() const;

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

// multiplication v1 * v2; it is assumed, that 0 * v = 0 for any v
// including inf and nan values
SYM_ARROW_EXPORT value   operator*(const value& v1, const value& v2);
SYM_ARROW_EXPORT value   operator*(const value& v1, const double& v2);
SYM_ARROW_EXPORT value   operator*(const double& v1, const value& v2);

// division v1 / v2
SYM_ARROW_EXPORT value   operator/(const value& v1, const value& v2);
SYM_ARROW_EXPORT value   operator/(const value& v1, const double& v2);
SYM_ARROW_EXPORT value   operator/(const double& v1, const value& v2);

// unary minus
SYM_ARROW_EXPORT value   operator-(const value& v1);

// integer power v1 ^ v2
SYM_ARROW_EXPORT value   power_int(const value& v1, int v2);

// real power |v1| ^ v2
SYM_ARROW_EXPORT value   power_real(const value& v1, const value& v2);

// inverse v1 ^ (-1)
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

#include "sym_arrow/details/value.inl"
