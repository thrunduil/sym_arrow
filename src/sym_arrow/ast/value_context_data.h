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
#include "sym_arrow/nodes/value.h"

namespace sym_arrow { namespace ast { namespace details
{

namespace sd = sym_arrow ::details;

// additional data stored in dag context for term nodes
class value_context_data : public sym_dag::context_data_base
{
    public:
        using expr_ptr      = sym_dag::dag_ptr<expr_base, sym_arrow::ast::term_tag>;

    private:
        virtual ~value_context_data() override;

        virtual void        initialize() override;
        virtual void        close() override;

    public:
        sd::value_mp*       val_zero_ptr;
        sd::value_mp*       val_one_ptr;
        sd::value_mp*       val_mone_ptr;
        sd::value_mp*       val_nan_ptr;
};

}}};

#include "sym_arrow/ast/value_context_data.inl"
