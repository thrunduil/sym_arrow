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

#include "sym_arrow/nodes/scalar.h"

namespace sym_arrow
{

//---------------------------------------------------------------------
//                  scalar
//---------------------------------------------------------------------
inline scalar::scalar()
{};

inline scalar::scalar(const ptr_type& rep)
    :m_ptr(rep)
{};

inline scalar::scalar(const scalar& other)
    :m_ptr(other.m_ptr)
{};

inline scalar::scalar(scalar&& other)
    :m_ptr(std::move(other.m_ptr))
{};

inline scalar& scalar::operator=(const scalar& other)
{
    m_ptr = other.m_ptr;
    return *this;
};

inline scalar& scalar::operator=(scalar&& other)
{
    m_ptr = std::move(other.m_ptr);
    return *this;
};

//---------------------------------------------------------------------
//                functions defined for scalar
//---------------------------------------------------------------------
inline bool sym_arrow::operator<(const scalar& s1, const scalar& s2)
{ 
    return s1.get_ptr() < s2.get_ptr(); 
};

inline bool sym_arrow::operator>(const scalar& s1, const scalar& s2)
{ 
    return s1.get_ptr() > s2.get_ptr(); 
};

inline bool sym_arrow::operator<=(const scalar& s1, const scalar& s2)
{ 
    return s1.get_ptr() <= s2.get_ptr(); 
};

inline bool sym_arrow::operator>=(const scalar& s1, const scalar& s2)
{ 
    return s1.get_ptr() >= s2.get_ptr(); 
};

inline bool sym_arrow::operator==(const scalar& s1, const scalar& s2)
{ 
    return s1.get_ptr() == s2.get_ptr(); 
};

inline bool sym_arrow::operator!=(const scalar& s1, const scalar& s2)
{ 
    return s1.get_ptr() != s2.get_ptr(); 
};

};