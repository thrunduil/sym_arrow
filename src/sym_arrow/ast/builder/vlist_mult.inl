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
#include "sym_arrow/ast/builder/vlist_mult.h"
#include "sym_arrow/ast/builder/vlist.inl"
#include "dag/dag.h"

namespace sym_arrow { namespace ast
{

//--------------------------------------------------------------
//                        VLIST_MULT
//--------------------------------------------------------------
template<class Value_t, class Derived>
inline vlist_mult<Value_t,Derived>::vlist_mult(const Value_t& pow)
    :vlist_base(), m_pow(pow)
{};

template<class Value_t, class Derived>
inline vlist_mult<Value_t,Derived>::vlist_mult(vlist_mult&& other)
    :vlist_base(std::move(other)), m_pow(std::move(other.m_pow))
{}

template<class Value_t, class Derived>
vlist_mult<Value_t,Derived>::~vlist_mult()
{
    if (is_null() == true)
        return;

    vlist_stack stack;
    close(stack);
};

template<class Value_t, class Derived>
inline const Value_t& vlist_mult<Value_t,Derived>::get_pow() const
{
    return m_pow;
};

template<class Value_t, class Derived>
void vlist_mult<Value_t,Derived>::make_pow(const Value_t& val)
{
    m_pow  = val * std::move(m_pow);

    size_t size = current_size();

    for (size_t i = 0; i < size; ++i)
        elem(i).get_value_ref() = std::move(elem(i).get_value_ref()) * val;
};

template<class Value_t, class Derived>
inline void vlist_mult<Value_t,Derived>::set_pow(const Value_t& p)
{
    m_pow = p;
};

template<class Value_t, class Derived>
inline void vlist_mult<Value_t,Derived>::init_with_default_values(const Value_t& p)
{
    new(&m_pow) Value_t(p);
};

template<class Value_t, class Derived>
inline bool vlist_mult<Value_t,Derived>::is_simple_mult() const
{
    if (vlist_base::is_simple() && is_value_one(elem(0).get_value()) == true)
        return true;
    else
        return false;
};

//--------------------------------------------------------------
//                        ilist_mult
//--------------------------------------------------------------
inline ilist_mult::ilist_mult(int pow)
    :vlist_mult(pow)
{};

inline ilist_mult::ilist_mult(ilist_mult&& other)
    :vlist_mult(std::move(other))
{};

inline void ilist_mult::make_pow(int p)
{
    vlist_mult::make_pow(p);
};

inline void ilist_mult::init_with_default_values()
{
    vlist_mult::init_with_default_values(1);
};

//--------------------------------------------------------------
//                        rlist_mult
//--------------------------------------------------------------
inline rlist_mult::rlist_mult(const value& pow)
    :vlist_mult(pow)
{};

inline rlist_mult::rlist_mult(rlist_mult&& other)
    :vlist_mult(std::move(other))
{};

inline void rlist_mult::make_pow(const value& p)
{
    vlist_mult::make_pow(p);
};

inline void rlist_mult::init_with_default_values()
{
    vlist_mult::init_with_default_values(value::make_one());
};

inline void rlist_mult::make_pow(int p)
{
    vlist_mult::make_pow(value::make_value(p));
};

};};