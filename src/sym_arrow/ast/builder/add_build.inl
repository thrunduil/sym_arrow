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

#include "sym_arrow/ast/builder/add_build.h"
#include "sym_arrow/ast/builder/vlist.inl"
#include "sym_arrow/ast/builder/build_item.inl"

namespace sym_arrow { namespace ast
{

template<class item_type>
add_build::add_build(const add_build_info<item_type>& bi)
    : base_type(this), m_vlist(bi.get_const(), value::make_one())
{
    for (size_t i = 0; i < bi.size(); ++i)
        insert_elem(bi.get_mult(i),bi.get_expr(i));

    if (bi.has_log())
        insert_log_elem(bi.get_expr_log());
};

template<class item_type>
add_build::add_build(const add_build_info2<item_type>& bi)
    : base_type(this), m_vlist(bi.get_const(), value::make_one())
{
    for (size_t i = 0; i < bi.size(); ++i)
        insert_elem(bi.get_mult(i), bi.get_expr(i));

    if (bi.has_log())
        insert_log_elem(bi.get_expr_log());
};

inline bool add_build::is_simple_add() const
{
    return m_vlist.is_simple_add();
};

inline const build_item<value>& add_build::unique_elem() const
{
    return m_vlist.elem(0);
};

inline const add_build::vlist_type& add_build::get_vlist() const
{
    return m_vlist;
};

inline const expr& add_build::get_cannonized() const
{
    return m_cannonized;
}

inline void add_build::set_cannonized(const expr& ex) const
{
    m_cannonized    = ex;
};

inline void add_build::set_modified()
{
    m_cannonized = expr();
}

inline void add_build::make_scal(const value& val)
{
    m_vlist.make_scal(val);
};

inline void add_build::make_add(const value& val)
{
    m_vlist.make_add(val);    
};

};};
