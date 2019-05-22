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
#include "sym_arrow/nodes/scalar.h"
#include "sym_arrow/nodes/symbol.h"
#include "sym_arrow/nodes/index.h"
#include "sym_arrow/details/dag_traits.h"

namespace sym_arrow
{

#pragma warning(push)
#pragma warning(disable:4251)    //needs to have dll-interface

static const bool do_cse_default = true;

// root class representing a symbolic expression
class SYM_ARROW_EXPORT expr
{
    private:
        using ptr_type      = ast::expr_ptr;

    private:
        mutable ptr_type    m_ptr;

    public:
        // default constructor creating uninitialized object
        expr();
        
        // copy and move constructors
        expr(const expr& other);
        expr(expr&& other);
        
        // assignment and move assignment
        expr& operator=(const expr& other);
        expr& operator=(expr&& other);

        // destructor
        ~expr();

    public:
        // create an expression representing a value
        explicit expr(const value& p);

        // create an expression representing a value
        explicit expr(double v);

        // conversion from a scalar
        expr(const scalar& val);        

        // conversion from a symbol
        expr(const symbol& val);

        // conversion from an index
        expr(const index& val);

        // conversion from add expression
        expr(const add_expr& val);

        // conversion from mult expression
        expr(const mult_expr& val);

        // conversion from function expression
        expr(const function_expr& val);

    public:
        // return true if this object is uninitialized
        bool                is_null() const;

        // return true if this object is cannonized
        bool                is_cannonized() const;

        // cannonize this object; construction of additive
        // and multiplicative terms is delayed until expression
        // is fully formed; cannonize funcion finalize building
        // such terms (i.e. perform simplifications, common sub-
        // expression elimination if do_cse = true, etc.); this 
        // function is costly
        void                cannonize(bool do_cse = do_cse_default) const;

        // return pointer to this object
        const expr*         operator->() const;

    public:
        // create from internal representation; internal use only
        explicit expr(const ptr_type& p);
        explicit expr(ptr_type&& p);
        
        // create from internal representation; internal use only
        explicit expr(const ast::expr_base* p);

        // access to internal pointers; internal use only
        const ptr_type&     get_ptr() const;

        // access to internal pointer; internal use only
        ast::expr_handle    get_expr_handle() const;
};

#pragma warning(pop)

};

#include "sym_arrow/details/expr.inl"
