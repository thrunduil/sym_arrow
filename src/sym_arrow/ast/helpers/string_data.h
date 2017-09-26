/* 
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

#include "sym_arrow/fwd_decls.h"

namespace sym_arrow { namespace ast { namespace details
{

// helper class used by symbol_rep
struct string_data
{
    // pointer to first element of null-terminated char array; 
    // additional data are stored at negative positions
    char*               m_name;

    // m_name is initialized to nullptr
    string_data();

    // construtor;
    string_data(const char* ptr, size_t size, size_t hash);

    // release memory
    void                destroy();

    // return beginning of the char array
    const char*         get_string() const;

    // number of elements of the char array (without trailing 0)
    size_t              get_size() const;

    // return stored hash value
    size_t              get_hash() const;

    // return true if this object is not initialized
    bool                is_null() const;
};

}}}

#include "sym_arrow/ast/helpers/string_data.inl"
