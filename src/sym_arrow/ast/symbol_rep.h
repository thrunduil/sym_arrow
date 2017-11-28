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

};}

#pragma warning(pop)

#include "sym_arrow/ast/symbol_rep.inl"
