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

#include "sym_arrow/nodes/symbol.h"

namespace sym_arrow 
{

inline symbol::symbol()
{}

inline symbol::symbol(const ptr_type& ex)
    :m_expr(ex)
{};

inline symbol::symbol(const ast::symbol_rep* h)
    : m_expr(ptr_type::from_this(h))
{};

inline symbol::symbol(const symbol& other)
    :m_expr(other.m_expr)
{};

inline symbol::symbol(symbol&& other)
    :m_expr(std::move(other.m_expr))
{};

inline symbol& symbol::operator=(const symbol& other)
{
    m_expr      = other.m_expr;
    return *this;
};

inline symbol& symbol::operator=(symbol&& other)
{
    m_expr      = std::move(other.m_expr);
    return *this;
};

inline symbol::symbol(const std::string& name)
    :symbol(name.data(), name.size())
{};

inline symbol::symbol(const char* name)
    :symbol(name, strlen(name))
{};

inline bool symbol::is_null() const
{ 
    return m_expr == ptr_type(); 
};

inline const symbol::ptr_type& symbol::get_ptr() const
{ 
    return m_expr; 
};

//---------------------------------------------------------------------
//                  functions
//---------------------------------------------------------------------
inline bool sym_arrow::operator<(const symbol& s1, const symbol& s2)
{ 
    return s1.get_ptr() < s2.get_ptr(); 
};

inline bool sym_arrow::operator>(const symbol& s1, const symbol& s2)
{ 
    return s1.get_ptr() > s2.get_ptr(); 
};

inline bool sym_arrow::operator<=(const symbol& s1, const symbol& s2)
{ 
    return s1.get_ptr() <= s2.get_ptr(); 
};

inline bool sym_arrow::operator>=(const symbol& s1, const symbol& s2)
{ 
    return s1.get_ptr() >= s2.get_ptr(); 
};

inline bool sym_arrow::operator==(const symbol& s1, const symbol& s2)
{ 
    return s1.get_ptr() == s2.get_ptr(); 
};

inline bool sym_arrow::operator!=(const symbol& s1, const symbol& s2)
{ 
    return s1.get_ptr() != s2.get_ptr(); 
};

};