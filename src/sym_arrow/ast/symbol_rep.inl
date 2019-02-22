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

#include "symbol_rep.inl"

namespace sym_arrow { namespace ast
{

inline const char* symbol_rep::get_name() const
{
    return m_name.get_string();
};

inline size_t symbol_rep::get_name_size() const
{
    return m_name.get_size();
};

inline size_t symbol_rep::get_symbol_code() const
{ 
    return m_code; 
};

inline void symbol_rep::release(stack_type& stack)
{
    (void)stack;
};

inline size_t symbol_rep::hash_value() const
{
    return m_name.get_hash();
};

inline size_t indexed_symbol_rep::get_symbol_code() const
{
    return m_name->get_symbol_code();
}

};}
