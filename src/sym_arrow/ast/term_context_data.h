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
#include "sym_arrow/nodes/scalar.h"
#include "sym_arrow/ast/helpers/registered_symbols.h"

namespace sym_arrow { namespace ast { namespace details
{

// additional data stored in dag context for term nodes
class term_context_data : public sym_dag::context_data_base
{
    public:
        using expr_ptr      = sym_dag::dag_ptr<expr_base, sym_arrow::ast::term_tag>;

    private:
        scalar              m_scalar_zero;
        scalar              m_scalar_one;
        scalar              m_scalar_minus_one;

        registered_symbols  m_reg_symbols;

        virtual ~term_context_data() override;

        virtual void        initialize() override;
        virtual void        close() override;

    public:
        // scalar representing value 0
        const scalar&       get_scalar_zero();

        // scalar representing value 1
        const scalar&       get_scalar_one();

        // scalar representing value -1
        const scalar&       get_scalar_minus_one();

        // return a code not used by other symbols
        size_t              get_fresh_symbol_code();

        // newly created symbols must be registered by this function
        void                register_symbol(const symbol_rep* h);

        // symbols about be beining destroyed must be unregistered 
        // by this function
        void                unregister_symbol(const symbol_rep* h);
};

}}};

#include "sym_arrow/ast/term_context_data.inl"
