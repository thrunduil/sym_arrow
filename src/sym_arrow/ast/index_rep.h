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
#include "sym_arrow/nodes/value.h"
#include "dag/dag.h"
#include "sym_arrow/ast/ast_tags.h"
#include "sym_arrow/ast/symbol_rep.h"

namespace sym_arrow { namespace ast
{

// data representing index_rep node; 
// instance of this class should not be created directly; 
// use the constructor of class index instead, which perform necessary checks
class index_rep_info
{
    public:
        // symbol name
        identifier_handle   m_name;

        // set name
        identifier_handle   m_set;

        // hash value; will be set later
        mutable size_t      m_hash;

    public:
        // constructor
        index_rep_info(identifier_handle name, identifier_handle set)
            : m_name(name), m_hash(0), m_set(set)
        {};
};

// dag_item representing an index
class index_rep : public expr_symbols<index_rep>
{
    private:
        using base_type = expr_symbols<index_rep>;

    private:
        size_t          m_hash;
        symbol_ptr      m_name;
        identifier_ptr  m_set;

    private:
        index_rep(const index_rep&) = delete;
        index_rep& operator=(const index_rep&) = delete;        

    public:
        // construct from a data represented by index_rep_info class
        explicit index_rep(const index_rep_info&);

        // destructor
        ~index_rep();                                

        // test for equality
        bool            equal(const index_rep_info& pi) const;

        // evaluate hash function
        static size_t   eval_hash(const index_rep_info& pi);

        // return hash value
        size_t          hash_value() const              { return m_hash; };

        // delay destruction of directly accessible dag items
        void            release(stack_type& st);

    public:
        // get index name
        symbol_handle   name() const                    { return m_name.get(); };

        // get name of index set
        identifier_handle   
                        set_name() const                { return m_set.get(); };
};

};};
