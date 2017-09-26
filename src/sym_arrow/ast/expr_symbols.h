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

#include "sym_arrow/config.h"
#include "sym_arrow/fwd_decls.h"
#include "dbs/dbs.h"

namespace sym_arrow { namespace ast
{

template<class Derived>
class expr_symbols : public sym_dag::dag_item<Derived, term_tag, true>
{
    private:
        using base_type         = sym_dag::dag_item<Derived, term_tag, true>;
        using dbs               = dbs_lib::dbs;

    private:
        dbs                     m_symbols;

        expr_symbols(const expr_symbols&) = delete;
        expr_symbols& operator=(const expr_symbols&) = delete;

    public:
        expr_symbols(const Derived* tag);

        // return symbol set for this binder
        const dbs&              get_symbol_set() const;

        // return number of symbols
        size_t                  number_symbols() const;

        // add symbol with code c to this set
        void                    add_symbol(size_t c);

        // add symbols with codes stored in a bitset
        void                    add_symbols(const dbs& h);

        // add symbols from expression h
        void                    add_symbols(expr_handle h);

        // return true if a symbol with code is in this set
        bool                    has_symbol(size_t code) const;

        // return true if any of symbol in bitset is in this set
        bool                    has_any_symbol(const dbs& h) const;

        // return true if all symbols in bitset is in this set
        bool                    has_all_symbols(const dbs& h) const;
};

};};

#include "sym_arrow/ast/expr_symbols.inl"
