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
#include "sym_arrow/ast/symbol_rep.h"

namespace sym_arrow { namespace ast
{

// data representing function_rep node
class function_rep_info
{
    public:
        // function name
        symbol_handle   m_name;

        // number of arguments
        size_t          m_size;

        // vector of arguments of length m_size
        const expr*     m_args;

        // hash value; will be set later
        mutable size_t  m_hash;

    public:
        // constructor; arguments must be cannonized
        function_rep_info(symbol_handle name, size_t size, const expr* args)
            : m_name(name), m_size(size), m_args(args)
            , m_hash(0) 
        {};
};

// dag_item representing function term term; such term is represented as:
// symbol[arg1, ..., argn], where argi is i-th argument, n >= 0
class function_rep : public expr_symbols<function_rep>
{
    private:
        using base_type = expr_symbols<function_rep>;

    private:
        size_t          m_hash;
        size_t          m_size;

        symbol_ptr      m_name;
        expr_ptr*       m_expr;

    private:
        function_rep(const function_rep&) = delete;
        function_rep& operator=(const function_rep&) = delete;        

    public:
        // construct from a data represented by function_rep_info class
        explicit function_rep(const function_rep_info&);

        // destructor
        ~function_rep();                                

        // test for equality
        bool            equal(const function_rep_info& pi) const;

        // evaluate hash function
        static size_t   eval_hash(const function_rep_info& pi);

        // return hash value
        size_t          hash_value() const              { return m_hash; };

        // delay destruction of directly accessible dag items
        void            release(stack_type& st);

    public:
        // return number of arguments
        size_t          size() const                    { return m_size; };

        // get i-th argument
        expr_handle     arg(size_t i) const             { return m_expr[i].get(); };

        // get function name
        symbol_handle   name() const                    { return m_name.get(); };
};

};};
