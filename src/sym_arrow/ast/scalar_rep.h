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
#include "sym_arrow/nodes/value.h"

namespace sym_arrow { namespace ast
{

// dag_item representing value
class scalar_rep : public sym_dag::dag_item<scalar_rep, term_tag, true>
{
    private:
        using base_type = sym_dag::dag_item<scalar_rep, term_tag, true>;

        size_t          m_hash;
        value           m_data;

    public:
        // construct scalar_rep from a double value
        explicit scalar_rep(double val);

        // construt scalar_rep from a value
        explicit scalar_rep(const value& val);

        // destructor
        ~scalar_rep();

        // return hash value
        size_t          hash_value() const              { return m_hash; }

        // evaluate hash function
        static size_t   eval_hash(const value& val)     { return value::eval_hash(val);};

        // test for equality
        bool            equal(const value& val) const   { return get_data().equal(val); };        

        // return true when this value is equal to 0
        bool            is_zero() const                 { return get_data().is_zero(); }

        // return true when this value is equal to 1
        bool            is_one() const                  { return get_data().is_one(); }

        // return true when this value is equal to -1
        bool            is_minus_one() const            { return get_data().is_minus_one(); }

        // return true when this value has positive sign
        bool            is_positive() const             { return get_data().is_positive(); }

        // return true when this value has negative sign
        bool            is_negative() const             { return get_data().is_negative(); }

        // return stored value
        const value&    get_data() const                { return m_data; }

        // delay destruction of directly accessible dag items
        void            release(stack_type&)            {};

    public:
        // return preallocated scalar storing value zero
        static const scalar&    make_zero();

        // return preallocated scalar storing value one
        static const scalar&    make_one();

        // return preallocated scalar storing value minus one
        static const scalar&    make_minus_one();
};

};};

#include "sym_arrow/ast/scalar_rep.inl"
