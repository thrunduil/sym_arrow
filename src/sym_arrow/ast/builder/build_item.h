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

#include "sym_arrow/fwd_decls.h"
#include "sym_arrow/nodes/value.h"

namespace sym_arrow { namespace ast
{

// handle to item stored in vlist (in add_build and mult_build nodes)
template<class Value_type>
class build_item_handle
{
    public:
        // type of value assigned to term
        using value_type            = Value_type;

    public:
        // value assigned to this item
        value_type              m_value;

        // handle to expression
        expr_handle             m_expr;

        // true if this is a special item (i.e. log or exp subterm)
        bool                    m_is_special;

    public:
        // constructor of regular item; set expression handle to nullptr
        build_item_handle(const Value_type&);

        // constructor of regular item
        build_item_handle(const Value_type& p, expr_handle e);

        // return true is this is a special item
        bool                    is_special() const;

        // return value assigned to term
        const Value_type&       get_value() const                   { return m_value;};

        // return expression handle
        expr_handle             get_expr_handle() const             { return m_expr; };

        // arrow operator; return this object
        const build_item_handle*    
                                operator->() const                  { return this; };

        // compare two items; values are ignored:
        bool                    compare(const build_item_handle&) const;
};

// item stored in vlist (in add_build and mult_build nodes)
template<class Value_type>
class build_item
{
    private:
        using stack_type        = sym_dag::details::release_stack<expr_base>;

    public:
        // type of value assigned to term
        using value_type        = Value_type;

        // type of handle to this item
        using handle_type       = build_item_handle<Value_type>;

        // type to mark special items (i.e. log or exp items)
        struct special_t{};

    private:
        // value assigned to this item (power or multiplication scalar)
        value_type              m_value;

        // expression
        expr_ptr                m_expr;

        // true is this is a special item
        bool                    m_is_special;               

    public:
        // create a regular item
        build_item(const value_type& scal, expr_handle expr);
        build_item(const value_type& scal, const expr& expr);
        build_item(const value_type& scal, expr&& expr);

        build_item(const build_item& other);
        build_item(build_item&& other);

        build_item& operator=(const build_item& other);
        build_item& operator=(build_item&& other);

        // create a special item
        build_item(special_t, const value_type& scal, expr_handle special_expr);

    public:
        // return value assigned to term
        const value_type&       get_value() const;

        // return value assigned to term; value can be modified
        value_type&             get_value_ref();

        // return true is this is a special item
        bool                    is_special() const;

        // return handle to expression
        expr_handle             get_expr_handle() const;

        // delay destruction of directly accessible dag nodes
        void                    release(vlist_stack& stack);

        // compare two items; values are ignored:
        bool                    compare(const build_item& other) const;
};

};};
