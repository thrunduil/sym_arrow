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
#include "dag/dag.h"
#include "sym_arrow/fwd_decls.h"
#include "sym_arrow/ast/helpers/registered_symbols.h"
#include "sym_arrow/nodes/scope.h"

namespace sym_arrow { namespace ast { namespace details
{

namespace sd = sym_arrow ::details;

// additional data stored in dag context for term nodes
class symbol_context_data : public sym_dag::context_data_base
{
    private:
        using reg_sym_ptr   = registered_symbols*;
        using sym_tab_ptr   = sd::sym_table_impl*;

    private:
        reg_sym_ptr         m_reg_symbols;
        sym_tab_ptr         m_sym_tab;

        type                m_default_type;
        scope_ptr           m_global_scope;

        virtual ~symbol_context_data() override;

        virtual void        initialize() override;
        virtual void        close() override;

    public:
        // return a code not used by other identifiers
        size_t              get_fresh_ident_code();

        // newly created identifiers must be registered by this function
        void                register_ident(const identifier_rep* h);

        // symbols about be beining destroyed must be unregistered 
        // by this function
        void                unregister_ident(const identifier_rep* h);

        const type&         default_type() const;
        const scope_ptr&    global_scope() const;
};

}}};

#include "sym_arrow/ast/symbol_context_data.inl"
