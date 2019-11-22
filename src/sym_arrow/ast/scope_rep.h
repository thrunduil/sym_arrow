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
#include "sym_arrow/fwd_decls.h"
#include "dag/dag.h"
#include "sym_arrow/ast/ast_tags.h"

namespace sym_arrow { namespace ast
{

// data representing scope_rep node
class scope_rep_info
{
    public:
        // name of scope
        const identifier_rep*   m_name;

        // parent scope
        const scope_rep*        m_parent;

        // hash value; will be set later
        mutable size_t          m_hash;

    public:
        // scope name cannot be qualified
        scope_rep_info(const identifier_rep* name, const scope_rep* parent)
            : m_name(name), m_parent(parent)
        {};
};

// dag_item representing a scope
class scope_rep : public sym_dag::dag_item<scope_rep, unique_nodes_tag, true>
{
    private:
        using base_type     = sym_dag::dag_item<scope_rep, unique_nodes_tag, true>;

    private:
        size_t              m_hash;

        identifier_ptr      m_name;
        scope_ptr           m_parent;
        mutable size_t      m_fresh_symbol_code;

    private:
        scope_rep(const scope_rep&) = delete;
        scope_rep& operator=(const scope_rep&) = delete;        

    public:
        // construct from a data represented by function_rep_info class
        explicit scope_rep(const scope_rep_info&);

        // destructor
        ~scope_rep();                                

        // return true if this object represents global scope
        bool                is_global() const;

        // return global scope
        static scope_ptr    global_scope();

        // create new (unnamed) scope within parent scope
        // different than any existing scope
        static scope_ptr    make_scope(const scope_rep* parent);

        // create new scope within parent scope with given name or
        // return existing one
        static scope_ptr    make_scope(const scope_rep* parent, 
                                const std::string& name);

        // create fresh identifier qualified with this scope; 
        identifier          get_unique_identifier() const;

        // return parent scope of this scope; if this scope is
        // global, then this scope is returned
        scope_ptr           get_parent_scope() const;

    public:
        // test for equality
        bool                equal(const scope_rep_info& pi) const;

        // evaluate hash function
        static size_t       eval_hash(const scope_rep_info& pi);

        // return hash value
        size_t              hash_value() const          { return m_hash; };

    private:
        static scope_ptr    make_global_scope();

        friend details::symbol_context_data;
};

};};
