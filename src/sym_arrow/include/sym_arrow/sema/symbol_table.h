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

#include "sym_arrow/nodes/expr.h"
#include "sym_arrow/nodes/type.h"
#include "sym_arrow/nodes/set.h"

#pragma warning(push)
#pragma warning(disable:4251)    //needs to have dll-interface

namespace sym_arrow
{

// management of symbol definitions
// this is a singleton class
class SYM_ARROW_EXPORT sym_table
{
    private:
        using impl_type     = details::sym_table_impl;

    private:
        impl_type*          m_impl;

    private:
        sym_table();

    public:
        ~sym_table();

        sym_table(const sym_table&);
        sym_table& operator=(const sym_table&);

        // get global symbol table
        static sym_table    get();

        // definition of a symbol sym as a set
        void                define_set(const identifier& sym, const set& s);

        // definition of a new type t
        void                define_type(const identifier& t);

        // define possibly indexed symbol sym<a1, ..., ak> of type t
        // sets a1, ..., ak must be already defined using define_set
        // function, if type name t is not initialized, then sym has
        // default type
        void                define_symbol(const identifier& sym, const std::vector<identifier>& args,
                                const type& t);

        // define function sym[n1 : t1, ..., nk : tk> of type t
        // with k arguments of types ti; arguments names ni plays only
        // debugging purpose
        void                define_function(const identifier& sym, const std::vector<formal_arg>& args,
                                const type& t);

    public:
        // return true is symbol sym defines a set
        bool                is_defined_set(const identifier& sym) const;

        // return true is symbol sym defines a type
        bool                is_defined_type(const identifier& sym) const;
};

};

#pragma warning(pop)