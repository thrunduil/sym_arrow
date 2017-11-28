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

#include "sym_arrow/config.h"
#include "sym_arrow/nodes/expr.h"
#include "dag/dag.h"
#include "sym_arrow/ast/ast.h"
#include "sym_arrow/ast/builder/vlist_add.h"
#include "sym_arrow/utils/stack_array.h"
#include "sym_arrow/functions/contexts.h"
#include "sym_arrow/ast/mult_rep.inl"
#include "sym_arrow/functions/expr_functions.h"

#include <sstream>

namespace sym_arrow { namespace details
{

namespace sd = sym_arrow :: details;

class do_eval_vis : public sym_dag::dag_visitor<sym_arrow::ast::term_tag, do_eval_vis>
{
    public:
        using tag_type  = sym_arrow::ast::term_tag;

    public:
        template<class Node>
        value eval(const Node* ast, const data_provider& dp);

        value eval(const ast::scalar_rep* h, const data_provider& dp);
        value eval(const ast::symbol_rep* h, const data_provider& dp);
        value eval(const ast::add_build* h, const data_provider& dp);
        value eval(const ast::mult_build* h, const data_provider& dp);
        value eval(const ast::add_rep* h, const data_provider& dp);
        value eval(const ast::mult_rep* h, const data_provider& dp);
        value eval(const ast::function_rep* h, const data_provider& dp);
};

class do_eval_vis_log : public sym_dag::dag_visitor<sym_arrow::ast::term_tag, do_eval_vis_log>
{
    public:
        using tag_type  = sym_arrow::ast::term_tag;

    public:
        template<class Node>
        value eval(const Node* ast, const data_provider& dp);

        value eval(const ast::scalar_rep* h, const data_provider& dp);
        value eval(const ast::symbol_rep* h, const data_provider& dp);
        value eval(const ast::add_build* h, const data_provider& dp);
        value eval(const ast::mult_build* h, const data_provider& dp);
        value eval(const ast::add_rep* h, const data_provider& dp);
        value eval(const ast::mult_rep* h, const data_provider& dp);
        value eval(const ast::function_rep* h, const data_provider& dp);
};

value do_eval_vis::eval(const ast::scalar_rep* h, const data_provider&)
{
    return h->get_data();
};
value do_eval_vis_log::eval(const ast::scalar_rep* h, const data_provider&)
{
    return log(h->get_data());
};

value do_eval_vis::eval(const ast::symbol_rep* h, const data_provider& dp)
{
    return dp.get_value(symbol(h));
};
value do_eval_vis_log::eval(const ast::symbol_rep* h, const data_provider& dp)
{
    return log(dp.get_value(symbol(h)));
};

value do_eval_vis::eval(const ast::add_build* h, const data_provider& dp)
{
    (void)h;
    (void)dp;
    assertion(0,"we should not be here");
    throw;
}
value do_eval_vis_log::eval(const ast::add_build* h, const data_provider& dp)
{
    (void)h;
    (void)dp;
    assertion(0,"we should not be here");
    throw;
}

value do_eval_vis::eval(const ast::mult_build* h, const data_provider& dp)
{
    (void)h;
    (void)dp;
    assertion(0,"we should not be here");
    throw;
}
value do_eval_vis_log::eval(const ast::mult_build* h, const data_provider& dp)
{
    (void)h;
    (void)dp;
    assertion(0,"we should not be here");
    throw;
}

value do_eval_vis::eval(const ast::add_rep* h, const data_provider& dp)
{
    value ret   = h->V0();
    size_t size = h->size();

    for(size_t i = 0; i < size; ++i)
    {
        value tmp = h->V(i) * visit(h->E(i), dp);
        ret = ret + tmp;
    };

    if (h->has_log())
        ret = ret + do_eval_vis_log().visit(h->Log(), dp);

    return ret;
};

value do_eval_vis_log::eval(const ast::add_rep* h, const data_provider& dp)
{
    value ret   = do_eval_vis().eval(h, dp);
    return log(ret);
};

value do_eval_vis::eval(const ast::mult_rep* h, const data_provider& dp)
{
    value ret = value::make_one();

    for(size_t i = 0; i < h->isize(); ++i)
        ret = ret * power_int(visit(h->IE(i), dp), h->IV(i));

    for(size_t i = 0; i < h->rsize(); ++i)
    {
        const value& tmp = h->RV(i);
        ret = ret * power_real(visit(h->RE(i), dp), tmp);
    };

    if (h->has_exp())
        ret = ret * exp(visit(h->Exp(), dp));

    return ret;
};

value do_eval_vis_log::eval(const ast::mult_rep* h, const data_provider& dp)
{
    value ret = value::make_zero();

    for(size_t i = 0; i < h->isize(); ++i)
        ret = ret + h->IV(i) * visit(h->IE(i), dp);

    for(size_t i = 0; i < h->rsize(); ++i)
    {
        const value& tmp = h->RV(i);
        ret = ret + tmp * visit(h->RE(i), dp);
    };

    if (h->has_exp())
        ret = ret + do_eval_vis().visit(h->Exp(), dp);

    return ret;
};

value do_eval_vis::eval(const ast::function_rep* h, const data_provider& dp)
{
    size_t size = h->size();

    using value_pod     =  sd::pod_type<value>;
    int size_counter    = 0;
    value_pod::destructor_type d(&size_counter);

    sd::stack_array<value_pod> buff(size, &d);    

    value* buff_ptr     = reinterpret_cast<value*>(buff.get());

    for(size_t i = 0; i < size; ++i)
    {
        value tmp = visit(h->arg(i), dp);

        new(buff_ptr + size_counter) value(tmp);
        ++size_counter;
    };

    symbol sym  = symbol(ast::symbol_ptr::from_this(h->name()));
    value ret   = dp.eval_function(sym, buff_ptr, size);
    return ret;
};

value do_eval_vis_log::eval(const ast::function_rep* h, const data_provider& dp)
{
    value ret = do_eval_vis().eval(h, dp);
    return log(ret);
};

}};

namespace sym_arrow
{

value sym_arrow::eval(const expr& ex, const data_provider& dp)
{
    ex.cannonize(do_cse_default);

    const ast::expr_base* h     = ex.get_ptr().get();

    value ret = details::do_eval_vis().visit(h, dp);
    return ret;
};

};