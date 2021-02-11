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

#include "sym_arrow/ast/builder/vlist_add.h"

namespace sym_arrow { namespace ast
{

//-------------------------------------------------------------------
//                        VLIST_ADD
//-------------------------------------------------------------------
inline vlist_add::vlist_add()
    :vlist_base(), m_add(value::make_zero()), m_scal(value::make_one())
{};

inline vlist_add::vlist_add(const value& add, const value& scal)
    :vlist_base(),m_add(add), m_scal(scal)
{};

inline vlist_add::vlist_add(vlist_add&& other)
    : vlist_base(std::move(other)), m_add(std::move(other.m_add))
    , m_scal(std::move(other.m_scal))
{}

inline void vlist_add::make_add(const value& add)
{
    m_add        = m_add + add;
};

inline void vlist_add::set_default_values()
{
    m_scal      = value::make_one();
    m_add       = value::make_zero();
};

inline const value& vlist_add::get_add() const
{
    return m_add;
}

inline const value& vlist_add::get_scal() const
{
    return m_scal;
};

inline bool vlist_add::is_simple_add() const
{
    if (vlist_base::is_simple() && (m_add.is_zero() == true))
        return true;
    else
        return false;
};

};};
