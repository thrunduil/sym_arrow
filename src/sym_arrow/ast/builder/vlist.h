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
#include <vector>

namespace sym_arrow { namespace ast
{

// class that stores dag nodes to destroy
class vlist_stack
{
    private:
        using expr_stack            = sym_dag::details::release_stack<ast::expr_base>;
        using expr_vec_handle       = expr_stack::handle_type;

    private:
        expr_stack*             m_parent_stack;
        expr_vec_handle         m_expr_stack;

    private:
        vlist_stack(const vlist_stack&) = delete;
        vlist_stack& operator=(const vlist_stack&) = delete;

    public:        
        // create a new stack
        vlist_stack();

        // create a new stack for binders; but put delayed expressions
        // to the existing stack
        vlist_stack(expr_stack& stack);

        // destructor; destroy all delayed nodes
        ~vlist_stack();

        // delay destruction of nodes
        void                    release(const expr& ex);

        // delay destruction of expressions
        void                    push_back(expr_handle ex);
};

// class storing items in add_build and mult_build; 
// this class is implemented as a linked list of arrays
// of increasing sizes; 
// Item_type       - type of stored values;
// Derived_type    - type of derived class
template<class Item_type, class Derived_type>
class vlist_base
{
    private:
        // initial size of the array; sizes of next arrays are
        // two times higher than previous ones
        static const int init_size = 3;        

    protected:
        // type of value assigned to expressions in stored elements
        using value_type        = typename Item_type::value_type;

        // type of derived class
        using vlist_type        = Derived_type;

        using stack_type        = sym_dag::details::release_stack<expr_base>;

        // tag class
        struct close_t{};

    private:
        struct header
        {
            size_t              m_has_prev  : 1;
            size_t              m_capacity  : sizeof(size_t) * 4;
            size_t              m_pos       : sizeof(size_t) * 4 - 1;            

            header(size_t cap, size_t pos, size_t prev)
                : m_capacity(cap), m_pos(pos), m_has_prev(prev)
            {};
        };

        header                  m_header;   
        Item_type*              m_vector;

    public:
        // construct a vector with zero elements
        vlist_base();

        // destructor
        ~vlist_base();

    protected:
        // copy vector from exisiting temporary list
        vlist_base(vlist_base&& other);

        vlist_base(const vlist_base& other) = delete;
        vlist_base& operator=(const vlist_base& other) = delete;

        // return true if this object stores a single regular item
        bool                    is_simple() const;

    public:
        // number of elements at current level
        size_t                  current_size() const;

        // return true, if this item has previous level
        bool                    has_previous() const;

        // return true, if this vector does not contain items
        bool                    empty() const;

        // return true, is this object is not initialized
        bool                    is_null() const;

        // return previous level
        vlist_type*             get_previous();
        const vlist_type*       get_previous() const;

        // get element at position pos
        const Item_type&        elem(size_t pos) const;
        Item_type&              elem(size_t pos);

        // destroy this object
        void                    close();

        // destroy this object; store delayed nodes in supplied stack
        void                    close(vlist_stack& stack);
        void                    close(stack_type& stack);

        // insert new item
        void                    push_back(const value_type& val, expr_handle ex);
        void                    push_back(const Item_type& it);

        // insert new special item
        void                    push_back_special(const value_type& val, expr_handle ex);

    private:
        // cast to derived type
        Derived_type*           get_derived();

        // this level is full; create a new level
        void                    finish_this();
};

};};