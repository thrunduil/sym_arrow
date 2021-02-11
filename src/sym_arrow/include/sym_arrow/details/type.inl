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

#include "sym_arrow/nodes/type.h"

namespace sym_arrow
{

//-------------------------------------------------------------------
//                          type
//-------------------------------------------------------------------

inline type::type()
{};

inline type::type(const identifier& name, bool is_const)
    :m_type_id(name), m_is_const(is_const)
{}

inline type::~type()
{};

inline type::type(const type& other)
    :m_type_id(other.m_type_id), m_is_const(other.m_is_const)
{};

inline type::type(type&& other)
    :m_type_id(std::move(other.m_type_id)), m_is_const(other.m_is_const)
{};

inline type& type::operator=(const type& other)
{
    m_type_id   = other.m_type_id;
    m_is_const  = other.m_is_const;
    return *this;
}

inline type& type::operator=(type&& other)
{
    m_type_id   = std::move(other.m_type_id);
    m_is_const  = other.m_is_const;
    return *this;
}

inline bool type::is_const() const
{
     return m_is_const;
}

inline bool type::is_null() const
{
     return m_type_id.is_null();
}

inline const identifier& type::type_name() const
{
    return m_type_id;
}

//-------------------------------------------------------------------
//                          formal_arg
//-------------------------------------------------------------------

inline formal_arg::formal_arg()
{}

inline formal_arg::formal_arg(const identifier& arg_name, const type& ty)
    :m_arg_name(arg_name), m_type(ty)
{}

inline formal_arg::~formal_arg()
{};

inline formal_arg::formal_arg(const formal_arg& other)
    : m_arg_name(other.m_arg_name), m_type(other.m_type)
{};

inline formal_arg::formal_arg(formal_arg&& other)
    : m_arg_name(std::move(other.m_arg_name)), m_type(std::move(other.m_type))
{}

inline formal_arg& formal_arg::operator=(const formal_arg& other)
{
    m_arg_name  = other.m_arg_name;
    m_type      = other.m_type;

    return *this;
};

inline formal_arg& formal_arg::operator=(formal_arg&& other)
{
    m_arg_name  = std::move(other.m_arg_name);
    m_type      = std::move(other.m_type);

    return *this;
};

inline bool formal_arg::is_null() const
{
    return m_arg_name.is_null();
}

inline const identifier& formal_arg::arg_name() const
{
    return m_arg_name;
};

inline const type& formal_arg::type_name() const
{
    return m_type;
};

//-------------------------------------------------------------------
//                          functions
//-------------------------------------------------------------------

inline bool sym_arrow::operator==(const type& t1, const type& t2)
{
    return (t1.type_name() == t2.type_name()) && (t1.is_const() == t2.is_const());
}

inline bool sym_arrow::operator!=(const type& t1, const type& t2)
{
    return (t1.type_name() != t2.type_name()) || (t1.is_const() != t2.is_const());
}

};
