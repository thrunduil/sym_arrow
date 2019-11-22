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

// representation of a scope
class SYM_ARROW_EXPORT scope
{
    private:
        using value_type    = ast::scope_rep;
        using ptr_type      = ast::scope_ptr;

    private:
        ptr_type            m_expr;

    public:
        // create global scope
        scope();

        // create new (unnamed) scope within parent scope
        // different than any existing scope
        static scope    make_scope(const scope& parent);

        // create new scope within parent scope with given name or
        // return existing one
        static scope    make_scope(const scope& parent, const std::string& name);

        // return true if this object represents global scope
        bool            is_global() const;

        // create fresh identifier in this scope
        identifier      get_unique_identifier() const;     

        // return parent scope of this scope; if this scope is
        // global, then this scope is returned
        scope           get_parent_scope() const;

    public:
        // create a set from internal representation; internal use only
        explicit scope(const ptr_type& ex);

        // access to internal pointers; internal use only
        const ptr_type&     get_ptr() const;
};

};

#pragma warning(pop)