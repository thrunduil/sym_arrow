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

#include "sym_arrow/config.h"
#include "sym_arrow/fwd_decls.h"
#include "sym_arrow/nodes/value.h"
#include "dag/dag.h"
#include "sym_arrow/ast/ast_tags.h"

namespace sym_arrow { namespace ast
{

// data representing set_rep node
class set_rep_info
{
    public:
        // type of a set member
        using elem_type     = identifier;

    public:
        // number of arguments
        size_t              m_size;

        // vector of arguments of length m_size
        const elem_type*    m_args;

        // hash value; will be set later
        mutable size_t      m_hash;

    public:
        // constructor; arguments cannot repeat and must be sorted
        set_rep_info(size_t size, const elem_type* args)
            : m_size(size), m_args(args), m_hash(0) 
        {};
};

// dag_item representing a set
class set_rep : public sym_dag::dag_item<set_rep, unique_nodes_tag, true>
{
    private:
        using base_type     = sym_dag::dag_item<set_rep, unique_nodes_tag, true>;
        using elem_type     = identifier_ptr;
        using elem_handle   = identifier_handle;

    private:
        size_t              m_hash;
        size_t              m_size;
        elem_type*          m_elements;

    private:
        set_rep(const set_rep&) = delete;
        set_rep& operator=(const set_rep&) = delete;        

    public:
        // construct from a data represented by function_rep_info class
        explicit set_rep(const set_rep_info&);

        // destructor
        ~set_rep();                                

    public:
        // return number of elements
        size_t              size() const                { return m_size; };

        // get i-th element
        elem_handle         arg(size_t i) const         { return m_elements[i].get(); };

        bool                is_member(const expr& x) const;

    public:
        // test for equality
        bool                equal(const set_rep_info& pi) const;

        // evaluate hash function
        static size_t       eval_hash(const set_rep_info& pi);

        // return hash value
        size_t              hash_value() const          { return m_hash; };
};

};};
