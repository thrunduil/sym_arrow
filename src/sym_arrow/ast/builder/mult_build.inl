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

#include "sym_arrow/ast/builder/mult_build.h"
#include "sym_arrow/ast/builder/build_item.inl"
#include "sym_arrow/ast/builder/vlist_mult.inl"

namespace sym_arrow { namespace ast
{

template<class Item>
mult_build::mult_build(const mult_build_info_int<Item>& bi)
    : base_type(this), m_ilist(nullptr), m_rlist(nullptr), m_scal(bi.get_const())
{
    make_ilist();

    size_t is = bi.isize();

    for (size_t i = 0; i < is; ++i)
        insert_ielem(bi.get_ipow(i), bi.get_iexpr(i));
};

template<class Ipow_item, class Rpow_item>
mult_build::mult_build(const mult_build_info<Ipow_item, Rpow_item>& bi)
    : base_type(this), m_ilist(nullptr), m_rlist(nullptr), m_scal(value::make_one())
{
    size_t is = bi.isize();
    size_t rs = bi.rsize();

    if (is > 0)
    {
        make_ilist();    

        for (size_t i = 0; i < is; ++i)
            insert_ielem(bi.get_ipow(i), bi.get_iexpr(i));
    };

    bool has_exp    = bi.has_exp();

    if (rs > 0 || has_exp == true)
    {
        make_rlist();

        for (size_t i = 0; i < rs; ++i)
            insert_relem(bi.get_rpow(i), bi.get_rexpr(i));
    }

    if (has_exp == true)
        insert_elem_exp(bi.get_exp_handle());
};

inline const value& mult_build::get_scal() const
{
    return m_scal;
};

inline bool mult_build::is_simple_mult() const
{ 
    return m_ilist && m_ilist->is_simple_mult() && m_rlist == nullptr;
};

inline const build_item<int>& mult_build::unique_ielem() const
{
    return m_ilist->elem(0);
};

inline const mult_build::ilist_type* mult_build::get_ilist() const
{
    return m_ilist;
};

inline const mult_build::rlist_type* mult_build::get_rlist() const
{
    return m_rlist;
};

inline const expr& mult_build::get_cannonized() const
{
    return m_cannonized;
}

inline void mult_build::set_cannonized(const expr& ex) const
{
    m_cannonized = ex;
};

inline void mult_build::set_modified()
{
    m_cannonized = expr();
}

inline void mult_build::make_scal(const value& s)
{
    m_scal = s*m_scal;
};

};};
