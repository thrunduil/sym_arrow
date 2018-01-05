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

#include "sym_arrow/ast/cannonization/item_collector.h"
#include "sym_arrow/ast/mult_rep.h"
#include "sym_arrow/exception.h"
#include "sym_arrow/functions/expr_functions.h"

#include <map>

namespace sym_arrow { namespace ast
{

//----------------------------------------------------------------
//                  item_collector_size
//----------------------------------------------------------------
inline item_collector_size::item_collector_size()
    : m_in(0), m_rn(0)
{};

inline item_collector_size::item_collector_size(size_t isize, size_t rsize)
    : m_in(isize), m_rn(rsize)
{};

inline void item_collector_size::report_special()
{};

template<>
inline void item_collector_size::report_atom<true>()
{
    ++m_in;
};

template<>
inline void item_collector_size::report_atom<false>()
{
    ++m_rn;
};

template<>
inline void item_collector_size::add<true>(const item_collector_size& other)
{
    m_in            = m_in + other.m_in;
    m_rn            = m_rn + other.m_rn;
};

template<>
inline void item_collector_size::add<false>(const item_collector_size& other)
{
    m_rn            = m_rn + other.m_in + other.m_rn;
};

template<>
inline void item_collector_size::report_mult<true>(const mult_rep* am)
{
    m_in            += am->isize();
    m_rn            += am->rsize();
};

template<>
inline void item_collector_size::report_mult<false>(const mult_rep* am)
{
    m_rn            += am->isize();
    m_rn            += am->rsize();
};

//----------------------------------------------------------------
//                  item_collector_add
//----------------------------------------------------------------
inline size_t item_collector_add::size() const
{
    return m_size;
}

inline void item_collector_add::add_add(const value& add)
{
    m_add = m_add + add;
};

inline bool item_collector_add::has_log() const
{
    return m_log.is_null() == false;
};

inline const value& item_collector_add::get_add() const
{
    return m_add;
};

inline const expr& item_collector_add::get_log() const
{
    return m_log;
};

inline void item_collector_add::set_log(const expr& ex)
{
    m_log = ex;
}

inline void item_collector_add::set_add(const value& add)
{
    m_add = add;
}

inline void item_collector_add::add(const value& scal, expr_handle tmp_ex)
{
    //TODO
    new(m_ih_tmp) item_handle(m_temp_vals->make_handle(scal), tmp_ex);
    ++m_ih_tmp;
};

inline void item_collector_add::set_size(size_t n)
{
    m_size      = n;
    m_ih_tmp    = m_ih + n;
};

inline item_collector_add::item_handle* item_collector_add::get_ih()
{
    return m_ih;
};

inline void item_collector_add::test() const
{
    #ifdef SYM_ARROW_DEBUG_EXPR
        assertion(m_ih_tmp == m_ih + m_size, "error in collect_items");
    #endif
};

//----------------------------------------------------------------
//                  item_collector_mult
//----------------------------------------------------------------
inline void item_collector_mult::test() const
{
    assertion(m_rih_tmp == m_rih + m_size.rsize(), "error in collect_items");
    assertion(m_iih_tmp == m_iih + m_size.isize(), "error in collect_items");
};

inline void item_collector_mult::add_exp(const expr& v)
{
    if (m_exp.is_null())
        m_exp   = v;
    else
        m_exp   = std::move(m_exp) + v;
};

inline void item_collector_mult::add_exp(expr&& v)
{
    if (m_exp.is_null())
        m_exp   = std::move(v);
    else
        m_exp   = std::move(m_exp) + std::move(v);
};

inline void item_collector_mult::add(int pow, expr_handle ex)
{
    new(m_iih_tmp) iitem_handle(pow, ex);
    ++m_iih_tmp;
};

inline void item_collector_mult::add(const value& pow, expr_handle ex)
{
    if (pow.is_zero() == true)
    {
        --m_size.rsize();
        return;
    };

    //TODO
    new(m_rih_tmp) ritem_handle(m_temp_vals->make_handle(pow), ex);
    ++m_rih_tmp;
};

inline void item_collector_mult::make_power(const value& v, int p)
{
    value v2 = power_int(v, p);
    add_scal(v2);
    return;
};

inline void item_collector_mult::make_power(const value& v, const value& pow)
{
    if (v.is_one() || v.is_minus_one())
        return;

    value ev        = power_real(v, pow);
    m_scal          = m_scal*ev;
    return;
};

inline bool item_collector_mult::has_exp() const
{
    return m_exp.is_null() == false;
};

};};