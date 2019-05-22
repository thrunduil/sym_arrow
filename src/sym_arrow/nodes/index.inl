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

#include "sym_arrow/nodes/index.h"

namespace sym_arrow 
{

inline index::index()
{}

inline index::index(const ptr_type& ex)
    :m_expr(ex)
{};

inline index::index(const ast::index_rep* h)
    : m_expr(ptr_type::from_this(h))
{};

inline index::index(const index& other)
    :m_expr(other.m_expr)
{};

inline index::index(index&& other)
    :m_expr(std::move(other.m_expr))
{};

inline index& index::operator=(const index& other)
{
    m_expr      = other.m_expr;
    return *this;
};

inline index& index::operator=(index&& other)
{
    m_expr      = std::move(other.m_expr);
    return *this;
};

inline bool index::is_null() const
{ 
    return m_expr == ptr_type(); 
};

inline const index::ptr_type& index::get_ptr() const
{ 
    return m_expr; 
};

//---------------------------------------------------------------------
//                  functions
//---------------------------------------------------------------------
inline bool sym_arrow::operator<(const index& s1, const index& s2)
{ 
    return s1.get_ptr() < s2.get_ptr(); 
};

inline bool sym_arrow::operator>(const index& s1, const index& s2)
{ 
    return s1.get_ptr() > s2.get_ptr(); 
};

inline bool sym_arrow::operator<=(const index& s1, const index& s2)
{ 
    return s1.get_ptr() <= s2.get_ptr(); 
};

inline bool sym_arrow::operator>=(const index& s1, const index& s2)
{ 
    return s1.get_ptr() >= s2.get_ptr(); 
};

inline bool sym_arrow::operator==(const index& s1, const index& s2)
{ 
    return s1.get_ptr() == s2.get_ptr(); 
};

inline bool sym_arrow::operator!=(const index& s1, const index& s2)
{ 
    return s1.get_ptr() != s2.get_ptr(); 
};

};