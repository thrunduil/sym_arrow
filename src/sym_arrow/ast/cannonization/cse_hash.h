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

#include "sym_arrow/nodes/expr.h"
#include "sym_arrow/ast/cannonization/branch_predictor.h"
#include "sym_arrow/ast/cannonization/cse_hash_data.h"
#include "sym_arrow/ast/helpers/utils.h"
#include "sym_arrow/ast/expr_cache.h"
#include "sym_arrow/utils/pool_hash_map.h"

namespace sym_arrow { namespace ast
{

// hasing common subexpression elemination
class cse_hash : public sym_dag::node_cache
{
    private:
        static const size_t case_size   = 1000;
        static const size_t num_rounds  = 16;
        
    private:
        using hash_map      = utils::pool_hash_map<ast::expr_handle, cse_hash_data, 
                                utils::expr_hash_equal>;
        using expr_cache    = ast::expr_cache<case_size, num_rounds>;
        using stack_type    = ast::expr_base::stack_type;

    private:
        static const int    num_predictors  = 5;

    private:
        branch_predictor    m_predictors[num_predictors];
        int                 m_nest_level;
        expr_cache          m_cache;
        hash_map            m_hash_map;

    private:
        cse_hash();

        cse_hash(const cse_hash&) = delete;
        cse_hash& operator=(const cse_hash&) = delete;

        friend sym_dag::global_objects;

    public:
        ~cse_hash();

        static cse_hash&    get();

        bool                check_hash();
        void                add_observation(bool obs, bool pred);

        // get and set result of common subexpr elimination
        bool                get_hashed_subexpr_elim(const expr& ex, 
                                value& norm, expr& simpl);
        void                set_hashed_subexpr_elim(const expr& ex, 
                                const value& norm, const expr& simpl);
        branch_predictor&   get_predictor(int level);

        virtual void        clear() override;

    public:
        void                unregister(expr_handle h, stack_type& st);
};

};};
