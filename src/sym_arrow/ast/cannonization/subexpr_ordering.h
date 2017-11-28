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

#include "sym_arrow/nodes/expr.h"
#include "sym_arrow/ast/builder/build_item.h"

#include <map>

namespace sym_arrow { namespace ast
{

enum class cmp_result
{
    better,
    equal,
    worse
};

enum class mult_subexpr_type
{
    empty,
    ipow,
    ipow_horner,
    rpow,   
    exp
};

class mult_subexpr
{
    private:        
        mult_subexpr_type   m_type;
        expr_handle         m_base;
        size_t              m_pos_in_array;
        mutable size_t      m_group_size;

        size_t              m_deep;
        size_t              m_length;
        size_t              m_length_total;
        int                 m_pow;

    public:
        mult_subexpr();

        // return true if this is uninitialized object
        bool                is_empty() const;

        // return base of this subexpression
        expr_handle         get_base() const;

        // return type of this subexpression
        mult_subexpr_type   get_type() const;

        // return integer power; meaningful only for ipow subexpressions
        int                 get_int_power() const;

        // return number of terms, that contain this subexpression
        size_t              factor_length() const;

        // return number of possible deep reductions
        size_t              get_number_deep() const;

        // get position in subexpr array of the first element with given
        // base
        size_t              get_pos_in_array() const;

        // number factorization merged together
        size_t              get_group_size() const;

    public:
        // make horner simplification on ex base with total_length subexpr
        // with given length, and pow being the minimum value of powers in
        // the sequence; n_deep_simpl is the number of  subexpressions that
        // can be deeply simplified
        static mult_subexpr make_ipow_horner(size_t pos_arr, expr_handle ex, 
                                size_t total_length, int min_pow, size_t n_deep_simpl);

        // collect ipow subexpression with the same base ex, the same power pow,
        // with total_length subexpr with the same base (positive and negative
        // bases are treated separately), and length_equal subexpr with the same
        // power pow; n_deep_simpl is the number of subexpressions that can be deeply
        // simplified
        static mult_subexpr make_ipow_equal(size_t pos_arr, expr_handle ex, 
                                size_t total_length, size_t length_equal, int pow, 
                                size_t n_deep_simpl);

        // collect exp subexpression with tha same base ex of length total_length,
        // n_deep_simpl is the number of subexpressions that can be deeply simplified
        static mult_subexpr make_exp(size_t pos_arr, expr_handle ex, size_t total_length, 
                                size_t n_deep);

        // collect rpow subexpression with tha same base ex of length total_length,
        // n_deep_simpl is the number of subexpressions that can be deeply simplified
        static mult_subexpr make_rpow_equal(size_t pos_arr, expr_handle ex, 
                                size_t total_length, size_t n_deep);
        
    public:
        // return true if this item is better than other
        cmp_result          compare(const mult_subexpr& other) const;

        // this factorization should be merged with other
        void                increase_group_size() const;

    private:
        // this and other have the same type
        cmp_result          compare_ipow(const mult_subexpr& other) const;
        cmp_result          compare_ipow_horner(const mult_subexpr& other) const;
        cmp_result          compare_rpow(const mult_subexpr& other) const;
        cmp_result          compare_exp(const mult_subexpr& other) const;
};

};};
