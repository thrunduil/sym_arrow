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

#include "subexpr_ordering.h"
#include "sym_arrow/exception.h"
#include "sym_arrow/ast/ast.h"
#include "sym_arrow/func/symbol_functions.h"

namespace sym_arrow { namespace ast
{

//-------------------------------------------------------------------
//                  mult_subexpr
//-------------------------------------------------------------------
mult_subexpr::mult_subexpr()
    :m_length(0), m_length_total(0), m_pow(0), m_type(mult_subexpr_type::empty)
    , m_deep(0), m_group_size(1)
{};

bool mult_subexpr::is_empty() const
{
    return m_type == mult_subexpr_type::empty;
};

expr_handle mult_subexpr::get_base() const
{
    return m_base;
};

mult_subexpr_type mult_subexpr::get_type() const
{
    return m_type;
}

int mult_subexpr::get_int_power() const
{
    return m_pow;
};

size_t mult_subexpr::factor_length() const
{
    return m_length;
};

size_t mult_subexpr::get_number_deep() const
{
    return m_deep;
};

size_t mult_subexpr::get_pos_in_array() const
{
    return m_pos_in_array;
}

size_t mult_subexpr::get_group_size() const
{
    return m_group_size;
}

void mult_subexpr::increase_group_size() const
{
    ++m_group_size;
}

mult_subexpr mult_subexpr::make_ipow_horner(size_t pos_arr, expr_handle ex, 
               size_t total_length, int max_abs_power, size_t n_deep_simpl)
{
    mult_subexpr ret;
    ret.m_base          = ex;
    ret.m_length        = total_length;
    ret.m_length_total  = total_length;
    ret.m_pow           = max_abs_power;
    ret.m_deep          = n_deep_simpl;
    ret.m_type          = mult_subexpr_type::ipow_horner;
    ret.m_pos_in_array  = pos_arr;

    return ret;
};

mult_subexpr mult_subexpr::make_ipow_equal(size_t pos_arr, expr_handle ex, 
               size_t total_length, size_t length_equal, int pow, size_t n_deep_simpl)
{
    mult_subexpr ret;
    ret.m_base          = ex;
    ret.m_length        = length_equal;
    ret.m_length_total  = total_length;
    ret.m_pow           = pow;
    ret.m_deep          = n_deep_simpl;
    ret.m_type          = mult_subexpr_type::ipow;
    ret.m_pos_in_array  = pos_arr;

    return ret;
};

mult_subexpr mult_subexpr::make_rpow_equal(size_t pos_arr, expr_handle ex, 
                              size_t total_length, size_t n_deep)
{
    mult_subexpr ret;
    ret.m_base          = ex;
    ret.m_length        = total_length;
    ret.m_length_total  = total_length;
    ret.m_deep          = n_deep;
    ret.m_type          = mult_subexpr_type::rpow;
    ret.m_pos_in_array  = pos_arr;

    return ret;
};

mult_subexpr mult_subexpr::make_exp(size_t pos_arr, expr_handle ex, 
                     size_t total_length, size_t n_deep)
{
    mult_subexpr ret;
    ret.m_base          = ex;
    ret.m_length        = total_length;
    ret.m_length_total  = total_length;
    ret.m_deep          = n_deep;
    ret.m_type          = mult_subexpr_type::exp;
    ret.m_pos_in_array  = pos_arr;
    return ret;
}

cmp_result mult_subexpr::compare(const mult_subexpr& other) const
{
    if (this->m_type == other.m_type)
    {
        switch(this->m_type)
        {
            case mult_subexpr_type::empty:
                return cmp_result::worse;
            case mult_subexpr_type::ipow:
                return compare_ipow(other);
            case mult_subexpr_type::ipow_horner:
                return compare_ipow_horner(other);
            case mult_subexpr_type::rpow:
                return compare_rpow(other);
            case mult_subexpr_type::exp:
                return compare_exp(other);
            default:
                return cmp_result::worse;
        }
    };

    // avoid horner
    if (this->m_type != mult_subexpr_type::ipow_horner
        && other.m_type == mult_subexpr_type::ipow_horner)
    {
        return cmp_result::better;
    };

    if (this->m_type == mult_subexpr_type::ipow_horner
        && other.m_type != mult_subexpr_type::ipow_horner)
    {
        return cmp_result::worse;
    };

    // prefer sequnce with higher number of deep simplifications
    if (this->m_deep > other.m_deep)
        return cmp_result::better;

    if (this->m_deep < other.m_deep)
        return cmp_result::worse;

    // prefer longer sequence irrespective of type
    if (this->m_length > other.m_length)
        return cmp_result::better;

    if (this->m_length < other.m_length)
        return cmp_result::worse;

    // prefer exp, then rpow, then ipow
    if (this->m_type == mult_subexpr_type::exp 
        && other.m_type != mult_subexpr_type::exp)
    {
        return cmp_result::better;
    };

    if (this->m_type != mult_subexpr_type::exp 
        && other.m_type == mult_subexpr_type::exp)
    {
        return cmp_result::worse;
    };

    if (this->m_type == mult_subexpr_type::rpow 
        && other.m_type != mult_subexpr_type::rpow)
    {
        return cmp_result::better;
    };

    if (this->m_type != mult_subexpr_type::rpow 
        && other.m_type == mult_subexpr_type::rpow)
    {
        return cmp_result::worse;
    };

    // we should not be here; all cases are already processed
    assertion(0, "invalid compare");
    return cmp_result::equal;
}

cmp_result mult_subexpr::compare_ipow(const mult_subexpr& other) const
{
    // prefer sequence of equal powers higher number of
    // subexpr can be deeply simplified
    if (this->m_deep > other.m_deep)
        return cmp_result::better;

    if (this->m_deep < other.m_deep)
        return cmp_result::worse;

    //prefer longest sequences of equal powers
    if (this->m_length > other.m_length)
        return cmp_result::better;

    if (this->m_length < other.m_length)
        return cmp_result::worse;

    // in case of equal length prefer negative powers first
    if (this->m_pow < 0 && other.m_pow > 0)
        return cmp_result::better;

    if (this->m_pow > 0 && other.m_pow < 0)
        return cmp_result::worse;
    
    // then prefer sequence with higher total length
    if (this->m_length_total > other.m_length_total)
        return cmp_result::better;

    if (this->m_length_total < other.m_length_total)
        return cmp_result::worse;

    if (this->m_pow < 0)
    {
        //then also other.m_pow < 0

        //in case of equal total length prefer sequence with lower power
        if (this->m_pow < other.m_pow)
            return cmp_result::better;

        if (this->m_pow > other.m_pow)
            return cmp_result::worse;
    }
    else
    {
        //in case of positive powers prefer sequence with higher power
        if (this->m_pow > other.m_pow)
            return cmp_result::better;

        if (this->m_pow < other.m_pow)
            return cmp_result::worse;
    };

    return cmp_result::equal;
};

cmp_result mult_subexpr::compare_ipow_horner(const mult_subexpr& other) const
{
    // prefer negative power first
    if (this->m_pow < 0 && other.m_pow > 0)
        return cmp_result::better;

    if (this->m_pow > 0 && other.m_pow < 0)
        return cmp_result::worse;
    
    if (this->m_pow < 0)
    {
        //then prefer sequence with lower power
        if (this->m_pow < other.m_pow)
            return cmp_result::better;

        if (this->m_pow > other.m_pow)
            return cmp_result::worse;
    }
    else
    {
        //then prefer sequence with higher power
        if (this->m_pow > other.m_pow)
            return cmp_result::better;

        if (this->m_pow < other.m_pow)
            return cmp_result::worse;
    };

    //prefer longest sequences of higher total length
    if (this->m_length_total > other.m_length_total)
        return cmp_result::better;

    if (this->m_length_total < other.m_length_total)
        return cmp_result::worse;

    return cmp_result::equal;
};

cmp_result mult_subexpr::compare_exp(const mult_subexpr& other) const
{
    // prefer sequence of equal powers higher number of
    // subexpr can be deeply simplified
    if (this->m_deep > other.m_deep)
        return cmp_result::better;

    if (this->m_deep < other.m_deep)
        return cmp_result::worse;

    //prefer longest sequences
    if (this->m_length > other.m_length)
        return cmp_result::better;

    if (this->m_length < other.m_length)
        return cmp_result::worse;

    return cmp_result::equal;
};

cmp_result mult_subexpr::compare_rpow(const mult_subexpr& other) const
{
    // prefer sequence of equal powers higher number of
    // subexpr can be deeply simplified
    if (this->m_deep > other.m_deep)
        return cmp_result::better;

    if (this->m_deep < other.m_deep)
        return cmp_result::worse;

    //prefer longest sequences of equal powers
    if (this->m_length > other.m_length)
        return cmp_result::better;

    if (this->m_length < other.m_length)
        return cmp_result::worse;

    return cmp_result::equal;
};

};};
