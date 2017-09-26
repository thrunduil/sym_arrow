/* 
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

#include "sym_arrow/ast/symbol_rep.h"
#include "dbs/dbs.h"

namespace sym_arrow { namespace ast
{

// assign unique code to a new symbol and maintain code -> symbol mapping
class registered_symbols
{
    private:
        using code_sym          = std::pair<size_t, const symbol_rep*>;
        using allocator         = sym_dag::details::symbolic_allocator<registered_symbols>;
        using pool_allocator    = sym_dag::details::object_allocator<allocator>;
        using dbs               = dbs_lib::dbs;
        using dbs_initializer   = dbs_lib::details::dbs_initializer;

        struct cs_h
        {
            size_t operator()(size_t c) const           { return c;};
            size_t operator()(const code_sym* c) const  { return c->first;};
        };

        struct cs_e
        {
            bool operator()(const code_sym* c1, size_t, size_t c2) const 
                                                        { return c1->first == c2; };
        };

        using code_sym_map      = mmlib::hash_table<code_sym, cs_h, cs_e>;

    private:
        pool_allocator          m_pool;
        code_sym_map            m_code_sym_map;
        dbs                     m_free_codes;        
        size_t                  m_current_symbol_code;

        // make sure, that dbs is not closed before destructor of this object
        dbs_initializer         m_dbs_init;

    public:
        registered_symbols();
        ~registered_symbols();

        // release all memory
        void                    close();

        // register a new symbol; should be called after construction
        // of symbol_rep
        void                    register_sym(const symbol_rep* h);

        // unregister a symbol; should be called during destruction
        // of symbol_rep
        void                    unregister_sym(const symbol_rep* h);

        // get unused code; should be called during constructor of 
        // symbol_rep
        size_t                  get_fresh_symbol_code();

        // get symbol with given code; code must be valid, i.e. there
        // must exist a symbol with given code, otherwise nullptr is
        // returned
        const symbol_rep*       get_symbol_from_code(size_t code) const;
};

};};
