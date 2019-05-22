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

// representation of an index
class SYM_ARROW_EXPORT index
{
    private:
        using ptr_type      = ast::index_ptr;

    private:
        ptr_type            m_expr;

    public:
        // create uninitialized index
        index();

        // create an index with name given by symbol sym with
        // elements from a set defined by symbol set
        index(const identifier& sym, const identifier& set);

        // destructor
        ~index();

        // copy and move constructors
        index(const index& other);
        index(index&& other);

        // assignment and move assignment
        index& operator=(const index& other);
        index& operator=(index&& other);

    public:
        // return true if this symbol is not initialized
        bool                is_null() const;

        // return name of this index
        symbol              get_name() const;

        // get name of index set
        identifier          set_name() const;

    public:
        // create a symbol from internal representation; internal use only
        explicit index(const ptr_type& ex);
        explicit index(const ast::index_rep* h);

        // access to internal pointers; internal use only
        const ptr_type&     get_ptr() const;
};

// comparison function based on addresses of pointers;
// these functions may return different results than 
// corresponding functions defined for class expr
bool SYM_ARROW_EXPORT    operator<(const index& s1, const index& s2);
bool SYM_ARROW_EXPORT    operator>(const index& s1, const index& s2);
bool SYM_ARROW_EXPORT    operator<=(const index& s1, const index& s2);
bool SYM_ARROW_EXPORT    operator>=(const index& s1, const index& s2);
bool SYM_ARROW_EXPORT    operator==(const index& s1, const index& s2);
bool SYM_ARROW_EXPORT    operator!=(const index& s1, const index& s2);

};

#pragma warning(pop)
