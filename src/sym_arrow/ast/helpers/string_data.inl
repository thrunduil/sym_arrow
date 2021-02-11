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

#include "string_data.h"

namespace sym_arrow { namespace ast { namespace details
{

inline string_data::string_data()
    :m_name(nullptr)
{};

inline bool string_data::is_null() const
{
    return m_name == nullptr;
}

inline const char* string_data::get_string() const
{
    return m_name;
}

inline size_t string_data::get_size() const
{
    if (m_name == nullptr)
        return 0;

    const char* ptr_root    = m_name - 2 * sizeof(size_t);
    return reinterpret_cast<const size_t*>(ptr_root)[0];
}

inline size_t string_data::get_hash() const
{
    if (m_name == nullptr)
        return 0;

    const char* ptr_root    = m_name - 2 * sizeof(size_t);
    return reinterpret_cast<const size_t*>(ptr_root)[1];
}

};};}
