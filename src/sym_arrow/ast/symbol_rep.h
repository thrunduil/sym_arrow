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
#include "sym_arrow/ast/helpers/string_data.h"
#include "sym_arrow/ast/ast_tags.h"

#pragma warning(push)
#pragma warning(disable : 4251) //needs to have dll-interface to be used by clients

namespace sym_arrow { namespace ast
{

// data to construct a symbol with given name
struct named_symbol_info
{
    const char*             m_name;
    size_t                  m_size;

    // create a named symbol with name stored in first 'size'
    // charackters of a char ptr 'name' (null terminator need not be
    // added)
    named_symbol_info(const char* name, size_t size)
        :m_name(name), m_size(size)
    {};
};

// class representing a symbol
class symbol_rep : public sym_dag::dag_item<symbol_rep, term_tag, true>
{
    private:
        using base_type         = sym_dag::dag_item<symbol_rep, term_tag, true>;

    private:
        details::string_data    m_name;
        size_t                  m_code;

    public:
        symbol_rep(const named_symbol_info& info);

        symbol_rep(const symbol_rep&) = delete;
        symbol_rep& operator=(const symbol_rep&) = delete;

    public:
        // destructor
        ~symbol_rep();
        
        // evaluate hash function
        static size_t       eval_hash(const named_symbol_info& info);

        // test for equality
        bool                equal(const named_symbol_info& info) const;

        // delay destruction of directly accessible dag items
        void                release(stack_type& stack);

        // return hash value
        size_t              hash_value() const;

        // null terminated pointer to name of this symbol
        const char*         get_name() const;

        // number of characters in name of this symbol
        size_t              get_name_size() const;

        // return code of this symbol; different symbols have
        // differrent codes
        size_t              get_symbol_code() const;
};

// data representing indexed_symbol_rep node
class indexed_symbol_info
{
    public:
        // symbol name
        symbol_handle   m_name;

        // number of arguments
        size_t          m_size;

        // vector of arguments of length m_size
        const expr*     m_args;

        // hash value; will be set later
        mutable size_t  m_hash;

    public:
        // constructor; arguments must be cannonized
        indexed_symbol_info(symbol_handle name, size_t size, const expr* args)
            : m_name(name), m_size(size), m_args(args)
            , m_hash(0) 
        {};
};

// class representing an indexed symbol
class indexed_symbol_rep : public sym_dag::dag_item<indexed_symbol_rep, term_tag, true>
{
    private:
        using base_type = sym_dag::dag_item<indexed_symbol_rep, term_tag, true>;

    private:
        size_t          m_hash;
        size_t          m_size;

        symbol_ptr      m_name;
        expr_ptr*       m_expr;

    private:
        indexed_symbol_rep(const indexed_symbol_rep&) = delete;
        indexed_symbol_rep& operator=(const indexed_symbol_rep&) = delete;

    public:
        // construct from a data represented by ndexed_symbol_info class
        explicit indexed_symbol_rep(const indexed_symbol_info&);

        // destructor
        ~indexed_symbol_rep();                                

        // test for equality
        bool            equal(const indexed_symbol_info& pi) const;

        // evaluate hash function
        static size_t   eval_hash(const indexed_symbol_info& pi);

        // return hash value
        size_t          hash_value() const      { return m_hash; };

        // delay destruction of directly accessible dag items
        void            release(stack_type& st);

    public:        
        // return number of arguments
        size_t          size() const            { return m_size; };

        // get i-th argument
        expr_handle     arg(size_t i) const     { return m_expr[i].get(); };

        // get symbol name
        symbol_handle   name() const            { return m_name.get(); };

        // return code of this symbol; different symbols have
        // differrent codes
        size_t          get_symbol_code() const;
};

};}

#pragma warning(pop)

#include "sym_arrow/ast/symbol_rep.inl"
