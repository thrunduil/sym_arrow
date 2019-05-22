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

#include "sym_arrow/config.h"
#include "dag/dag.h"
#include "sym_arrow/fwd_decls.h"

#pragma warning(push)
#pragma warning(disable:4251) //needs to have dll-interface to be used by clients

namespace sym_arrow
{

// representation of a set
class SYM_ARROW_EXPORT set
{
    private:
        using ptr_type      = ast::set_ptr;

    private:
        ptr_type            m_expr;

    public:
        // create uninitialized set
        set();

        // create symbol with given elements; 
        explicit set(const std::vector<identifier>& elems);

        // destructor
        ~set();

        // copy and move constructors
        set(const set& other);
        set(set&& other);

        // assignment and move assignment
        set& operator=(const set& other);
        set& operator=(set&& other);

    public:
        // return true if this symbol is not initialized
        bool                is_null() const;

        // return number of elements
        size_t              size() const;

        // get i-th element
        identifier          arg(size_t i) const;

        // return true if x is a member of this set
        bool                is_member(const expr& x) const;

    public:
        // create a set from internal representation; internal use only
        explicit set(const ptr_type& ex);

        // access to internal pointers; internal use only
        const ptr_type&     get_ptr() const;
};

};

#pragma warning(pop)
