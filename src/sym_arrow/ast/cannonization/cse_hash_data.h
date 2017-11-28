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
#include "sym_arrow/ast/ast_tags.h"
#include "sym_arrow/ast/ast_rep_details.h"

namespace sym_arrow { namespace ast
{

// class storing common subexpression elimination results
class cse_hash_data
{
    private:
        using stack_type    = expr_base::stack_type;

    private:
        value               m_normalization;
        weak_expr_ptr       m_simplified;

    public:
        // create uninitialized object
        cse_hash_data();

        // initialize with common subexpression elimination results
        cse_hash_data(const value& norm, const expr& simpl);

        // return true if this object is not initialized
        bool                is_empty() const;

        // return normalization constant
        const value&        get_normalization() const;

        // return simplified expression
        expr                get_simplified_expr() const;

        // delay destruction of dag nodes
        void                release(stack_type& st) const;
};

};};
