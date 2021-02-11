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
#include "dag/dag.h"
#include "sym_arrow/ast/helpers/string_data.h"
#include "sym_arrow/ast/ast_tags.h"
#include "sym_arrow/nodes/type.h"

#pragma warning(push)
#pragma warning(disable : 4251) //needs to have dll-interface to be used by clients

namespace sym_arrow { namespace ast
{

// data to construct a symbol with given name
struct identifier_info
{
    public:
        const char*             m_name;
        size_t                  m_size;
        const scope_rep*        m_scope;        

        // create a named symbol with name stored in first 'size'
        // charackters of a char ptr 'name' (null terminator need not be
        // added)
        identifier_info(const char* name, size_t size, const scope_rep* sc)
            :m_name(name), m_size(size), m_scope(sc), m_special(false)
        {};

    private:
        bool                    m_special;

        void set_special()
        {
            m_special = true;
        };

        friend identifier_rep;
};

// class representing a symbol
class identifier_rep : public sym_dag::dag_item<identifier_rep, unique_nodes_tag, true>
{
    private:
        using base_type         = sym_dag::dag_item<identifier_rep, unique_nodes_tag, true>;

    private:
        details::string_data    m_name;
        size_t                  m_code;
        scope_ptr               m_scope;

    public:
        identifier_rep(const identifier_info& info);

        identifier_rep(const identifier_rep&) = delete;
        identifier_rep& operator=(const identifier_rep&) = delete;

    public:
        // destructor
        ~identifier_rep();
        
        // evaluate hash function
        static size_t       eval_hash(const identifier_info& info);

        // test for equality
        bool                equal(const identifier_info& info) const;

        // return hash value
        size_t              hash_value() const;

        // null terminated pointer to name of this symbol
        const char*         get_name() const;

        // number of characters in name of this symbol
        size_t              get_name_size() const;

        // return code of this identifier; different identifier have
        // differrent codes
        size_t              get_identifier_code() const;

        // return true if identifier scope is set
        bool                is_qualified() const;

        // make special identifier used internally by this library
        static identifier_ptr
                            make_special(size_t code, const scope_rep* scope);

        // make special identifier used internally by this library
        // name must begin with '$' and an alphabetic letter
        static identifier_ptr
                            make_special(const std::string& name, const scope_rep* scope);
};

// data representing symbol_rep node
class symbol_info
{
    public:
        // symbol name
        identifier_handle   m_name;

        // number of arguments
        size_t              m_size;

        // vector of arguments of length m_size
        const expr*         m_args;

        // symbol type
        const type*         m_type;

        // const flag
        bool                m_is_const;

        // hash value; will be set later
        mutable size_t      m_hash;

    public:
        // constructor; arguments must be cannonized
        symbol_info(identifier_handle name, size_t size, const expr* args,
                    const type*  t)
            : m_name(name), m_size(size), m_args(args), m_type(t), m_hash(0)
        {};
};

// class representing an indexed symbol
class symbol_rep : public expr_symbols<symbol_rep>
{
    private:
        using base_type = expr_symbols<symbol_rep>;

    private:
        size_t          m_hash;
        size_t          m_size;

        identifier_ptr  m_name;
        expr_ptr*       m_expr;
        type            m_type;

    private:
        symbol_rep(const symbol_rep&) = delete;
        symbol_rep& operator=(const symbol_rep&) = delete;

    public:
        // construct from a data represented by ndexed_symbol_info class
        explicit symbol_rep(const symbol_info&);

        // destructor
        ~symbol_rep();                                

        // test for equality
        bool            equal(const symbol_info& pi) const;

        // evaluate hash function
        static size_t   eval_hash(const symbol_info& pi);

        // return hash value
        size_t          hash_value() const      { return m_hash; };

        // delay destruction of directly accessible dag items
        void            release(stack_type& st);

    public:        
        // return number of indices
        size_t          size() const            { return m_size; };

        // get i-th index
        expr_handle     arg(size_t i) const     { return m_expr[i].get(); };

        // name of this symbol
        identifier_handle
                        get_name() const        { return m_name.get();}

        // type of this symbol; can be nullpr
        const type&     get_type() const        { return m_type;}

        // return true if this symbol is constant
        bool            is_const() const        { return m_type.is_const(); };

        // return code of name of this symbol; different names have
        // differrent codes
        size_t          get_identifier_code() const;
};

};}

#pragma warning(pop)

#include "sym_arrow/ast/symbol_rep.inl"
