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
#include "dag/dag.h"
#include "sym_arrow/fwd_decls.h"

#pragma warning(push)
#pragma warning(disable:4251) //needs to have dll-interface to be used by clients

namespace sym_arrow
{

// representation of a symbol name
class SYM_ARROW_EXPORT identifier
{
    private:
        using ptr_type      = ast::identifier_ptr;

    private:
        ptr_type            m_expr;

    public:
        // create uninitialized symbol
        identifier();

        // create symbol with given name; name != nullptr; name must be null
        // terminated and cannot begin with '$' sign
        explicit identifier(const char* name);

        // create symbol with given name; name.size() > 0
        // name cannot begin with '$' sign
        explicit identifier(const std::string& name);

        // create symbol with given name; name must point to an array with
        // at least num_char characters; num_char > 0; 
        identifier(const char* name, size_t num_char);

        // destructor
        ~identifier();

        // copy and move constructors
        identifier(const identifier& other);
        identifier(identifier&& other);

        // assignment and move assignment
        identifier& operator=(const identifier& other);
        identifier& operator=(identifier&& other);

    public:
        // return true if this symbol is not initialized
        bool                is_null() const;

        // null terminated pointer to name of this symbol
        const char*         get_name() const;

        // return code of base symbol of this symbol; different 
        // base symbols have differrent codes
        size_t              get_base_symbol_code() const;

    public:
        // create function f[], where f is this identifier name
        expr                operator()() const;

        // create function f[x1, x2, ...], where f is this symbol name
        expr                operator()(const expr& x1) const;
        expr                operator()(const expr& x1, const expr& x2) const;
        expr                operator()(std::initializer_list<expr> args) const;
        expr                operator()(const std::vector<expr>& args) const;

        // create symbol f<x1, x2, ...> of type t, where f is this identifier name;
        // if t is not initialized, then type is infered based on existing declarations
        symbol              index(const identifier& t = identifier()) const;
        symbol              index(const expr& x1, const identifier& t = identifier()) const;
        symbol              index(const expr& x1, const expr& x2,
                                const identifier& t = identifier()) const;
        symbol              index(std::initializer_list<expr> args,
                                const identifier& t = identifier()) const;
        symbol              index(const std::vector<expr>& args,
                                const identifier& t = identifier()) const;

    public:
        // create a symbol from internal representation; internal use only
        explicit identifier(const ptr_type& ex);
        explicit identifier(const ast::identifier_rep* h);

        // access to internal pointers; internal use only
        const ptr_type&     get_ptr() const;
};

// representation of a symbol
class SYM_ARROW_EXPORT symbol
{
    private:
        using ptr_type      = ast::symbol_ptr;

    private:
        ptr_type            m_expr;

    public:
        // create uninitialized symbol
        symbol();

        // create symbol with given name; name != nullptr; name must be null
        // terminated and cannot begin with '$' sign
        explicit symbol(const char* name);

        // create symbol with given name; name.size() > 0
        // name cannot begin with '$' sign
        explicit symbol(const std::string& name);

        // create symbol with given name; name must point to an array with
        // at least num_char characters; num_char > 0; 
        symbol(const char* name, size_t num_char);

        // destructor
        ~symbol();

        // copy and move constructors
        symbol(const symbol& other);
        symbol(symbol&& other);

        // assignment and move assignment
        symbol& operator=(const symbol& other);
        symbol& operator=(symbol&& other);

    public:
        // return true if this symbol is not initialized
        bool                is_null() const;

        // name of this symbol
        identifier          get_name() const;

        // type of this symbol; can be nullpr
        identifier          get_type() const;

        // return number of indices
        size_t              size() const;

        // get i-th index
        expr                arg(size_t i) const;

        // return code of this symbol; different symbols have
        // differrent codes
        size_t              get_indexed_symbol_code() const;

        // return code of base symbol of this symbol; different 
        // base symbols have differrent codes
        size_t              get_base_symbol_code() const;

    public:
        // create a symbol from internal representation; internal use only
        explicit symbol(const ptr_type& ex);
        explicit symbol(const ast::indexed_symbol_rep* h);

        // access to internal pointers; internal use only
        const ptr_type&     get_ptr() const;
};

// comparison function based on addresses of pointers;
// these functions may return different results than 
// corresponding functions defined for class expr
bool SYM_ARROW_EXPORT    operator<(const symbol& s1, const symbol& s2);
bool SYM_ARROW_EXPORT    operator>(const symbol& s1, const symbol& s2);
bool SYM_ARROW_EXPORT    operator<=(const symbol& s1, const symbol& s2);
bool SYM_ARROW_EXPORT    operator>=(const symbol& s1, const symbol& s2);
bool SYM_ARROW_EXPORT    operator==(const symbol& s1, const symbol& s2);
bool SYM_ARROW_EXPORT    operator!=(const symbol& s1, const symbol& s2);

// comparison function based on addresses of pointers;
// these functions may return different results than 
// corresponding functions defined for class expr
bool SYM_ARROW_EXPORT    operator<(const identifier& s1, const identifier& s2);
bool SYM_ARROW_EXPORT    operator>(const identifier& s1, const identifier& s2);
bool SYM_ARROW_EXPORT    operator<=(const identifier& s1, const identifier& s2);
bool SYM_ARROW_EXPORT    operator>=(const identifier& s1, const identifier& s2);
bool SYM_ARROW_EXPORT    operator==(const identifier& s1, const identifier& s2);
bool SYM_ARROW_EXPORT    operator!=(const identifier& s1, const identifier& s2);

};

#pragma warning(pop)
