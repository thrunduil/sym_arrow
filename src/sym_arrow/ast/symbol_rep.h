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
class base_symbol_rep : public sym_dag::dag_item<base_symbol_rep, symbol_tag, true>
{
    private:
        using base_type         = sym_dag::dag_item<base_symbol_rep, symbol_tag, true>;

    private:
        details::string_data    m_name;
        size_t                  m_code;

    public:
        base_symbol_rep(const named_symbol_info& info);

        base_symbol_rep(const base_symbol_rep&) = delete;
        base_symbol_rep& operator=(const base_symbol_rep&) = delete;

    public:
        // destructor
        ~base_symbol_rep();
        
        // evaluate hash function
        static size_t       eval_hash(const named_symbol_info& info);

        // test for equality
        bool                equal(const named_symbol_info& info) const;

        // return hash value
        size_t              hash_value() const;

        // null terminated pointer to name of this symbol
        const char*         get_name() const;

        // number of characters in name of this symbol
        size_t              get_name_size() const;

        // return code of this symbol; different symbols have
        // differrent codes
        size_t              get_base_symbol_code() const;
};

// data representing indexed_symbol_rep node
class indexed_symbol_info
{
    public:
        // symbol name
        base_symbol_handle  m_name;

        // number of arguments
        size_t              m_size;

        // vector of arguments of length m_size
        const expr*         m_args;

        // hash value; will be set later
        mutable size_t      m_hash;

    public:
        // constructor; arguments must be cannonized
        indexed_symbol_info(base_symbol_handle name, size_t size, const expr* args)
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
        size_t          m_code;

        base_symbol_ptr m_name;
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
        // return number of indices
        size_t          size() const            { return m_size; };

        // get i-th index
        expr_handle     arg(size_t i) const     { return m_expr[i].get(); };

        // null terminated pointer to name of this symbol
        base_symbol_handle
                        get_name() const        { return m_name.get();}

        // return code of this symbol; different symbols have
        // differrent codes
        size_t          get_indexed_symbol_code() const;

        // return code of base symbol of this symbol; different 
        // base symbols have differrent codes
        size_t          get_base_symbol_code() const;
};

};}

#pragma warning(pop)

#include "sym_arrow/ast/symbol_rep.inl"
