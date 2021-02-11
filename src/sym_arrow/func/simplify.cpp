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

#include "sym_arrow/config.h"
#include "sym_arrow/nodes/expr.h"
#include "dag/dag.h"
#include "sym_arrow/ast/ast.h"
#include "sym_arrow/ast/builder/vlist_add.h"
#include "sym_arrow/utils/stack_array.h"
#include "sym_arrow/ast/mult_rep.inl"
#include "sym_arrow/func/symbol_functions.h"
#include "sym_arrow/ast/cannonization/cannonize.h"
#include "sym_arrow/utils/pool_hash_map.h"
#include "sym_arrow/functions/expr_functions.h"
#include "sym_arrow/functions/sym_functions.h"
#include "sym_arrow/functions/constructor_functions.h"

#include <sstream>
#include <map>

namespace sym_arrow { namespace details
{

namespace sd = sym_arrow :: details;

class do_simplify_vis : public sym_dag::dag_visitor<sym_arrow::ast::term_tag, do_simplify_vis>
{
    private:
        static const size_t max_size    = 10000;

    private:
        using hash_map      = utils::pool_hash_map<ast::expr_handle, ast::expr_ptr, 
                                utils::expr_hash_equal>;
        using handle_type   = hash_map::handle_type;

    private:
        hash_map        m_hash_map;

        do_simplify_vis(const do_simplify_vis&) = delete;
        do_simplify_vis& operator=(const do_simplify_vis&) = delete;

    public:
        using tag_type  = sym_arrow::ast::term_tag;

        expr make(const ast::expr_base* ast);

    public:
        do_simplify_vis();
        ~do_simplify_vis();

        template<class Node>
        expr eval(const Node* ast);

        expr eval(const ast::scalar_rep* h);
        expr eval(const ast::symbol_rep* h);
        expr eval(const ast::add_build* h);
        expr eval(const ast::mult_build* h);
        expr eval(const ast::add_rep* h);
        expr eval(const ast::mult_rep* h);
        expr eval(const ast::function_rep* h);

    private:
        handle_type find(ast::expr_handle h);
        void        insert(ast::expr_handle h, const expr& simpl);
};

do_simplify_vis::do_simplify_vis()
{};

do_simplify_vis::~do_simplify_vis()
{
    using context   = ast::expr_base::context_type;
    auto st         = context::get().get_stack();

    m_hash_map.clear(st.get());
};

expr do_simplify_vis::make(const ast::expr_base* ast)
{
    //expression must be rebuilded in order to activate full cannonization
    expr ret = visit(ast);
    return ret;
};

inline do_simplify_vis::handle_type 
do_simplify_vis::find(ast::expr_handle h)
{
    return m_hash_map.find(h);
}

void do_simplify_vis::insert(ast::expr_handle h, const expr& simpl)
{
    if (m_hash_map.size() > max_size)
    {
        using context   = ast::expr_base::context_type;
        auto st         = context::get().get_stack();

        m_hash_map.clear(st.get());
    };

    m_hash_map.insert(h, simpl.get_ptr());
}

expr do_simplify_vis::eval(const ast::scalar_rep* h)
{
    return expr(ast::expr_ptr::from_this(h));
};

expr do_simplify_vis::eval(const ast::symbol_rep* h)
{
    //nothing to do
    return expr(ast::expr_ptr::from_this(h));
};

expr do_simplify_vis::eval(const ast::add_build* h)
{
    (void)h;
    assertion(0,"expression not explicit");
    throw;
};

expr do_simplify_vis::eval(const ast::mult_build* h)
{
    (void)h;
    assertion(0,"expression not explicit");
    throw;
};

expr do_simplify_vis::eval(const ast::add_rep* h)
{
    auto ex             = find(h);

    if (ex.empty() == false)
        return expr(ex->get_value());

    size_t n            = h->size();

    using item          = ast::build_item_ptr<value>;
    using item_pod      = sd::pod_type<item>;

    int size_counter    = 0;
    size_t length       = n + 1;
    item_pod::destructor_type d(&size_counter);
    sd::stack_array<item_pod> sum_buff(length, &d);    

    item* sum_buff_ptr  = sum_buff.get_cast<item>();

    for (size_t j = 0; j < n; ++j)
    {
        expr simpl      = make(h->E(j));

        new (sum_buff_ptr + size_counter) item(&h->V(j), std::move(simpl));
        ++size_counter;
    };

    value add           = h->V0();
    expr log_term;

    if (h->has_log() == true)
    {
        expr simpl      = make(h->Log());

        if (simpl.get_ptr()->isa<ast::scalar_rep>() == true)
        {
            add         = add + log(cast_scalar(simpl).get_value());
        }
        else
        {
            log_term    = std::move(simpl);
        };
    };

    ast::expr_ptr simpl;

    value one   = value::make_one();

    if (log_term.is_null() == true)
    {
        ast::add_build_info2<item> bi(&add, size_counter, sum_buff_ptr, nullptr);
        simpl = ast::add_build::make(bi);
    }
    else
    {
        item log_it(&one, log_term);
        ast::add_build_info2<item> bi(&add, size_counter, sum_buff_ptr, &log_it);
        simpl = ast::add_build::make(bi);
    };

    expr ret    = expr(std::move(simpl));
    ret.cannonize(true);

    insert(h, ret);
    return ret;
};

expr do_simplify_vis::eval(const ast::mult_rep* h)
{
    auto ex     = find(h);

    if (ex.empty() == false)
        return expr(ex->get_value());

    namespace sd = sym_arrow :: details;

    size_t in           = h->isize();
    size_t rn           = h->rsize();

    using iitem         = ast::build_item<int>;
    using ritem         = ast::build_item_ptr<value>;
    using iitem_pod     = sd::pod_type<iitem>;
    using ritem_pod     = sd::pod_type<ritem>;

    int ipow_counter    = 0;
    iitem_pod::destructor_type d_ipow(&ipow_counter);
    sd::stack_array<iitem_pod> ipow_buff(in, &d_ipow);

    iitem* ipow_ptr     = ipow_buff.get_cast<iitem>();

    for (size_t i = 0; i < in; ++i)
    {
        expr simpl      = make(h->IE(i));
        int pow         = h->IV(i);   

        new (ipow_ptr + ipow_counter) iitem(pow, std::move(simpl));
        ++ipow_counter;
    };

    int rpow_counter    = 0;
    ritem_pod::destructor_type d_rpow(&rpow_counter);
    sd::stack_array<ritem_pod> rpow_buff(rn, &d_rpow);    

    ritem* rpow_ptr     = rpow_buff.get_cast<ritem>();

    for (size_t i = 0; i < rn; ++i)
    {
        expr simpl      = make(h->RE(i));

        new (rpow_ptr + rpow_counter) ritem(&h->RV(i), std::move(simpl));
        ++rpow_counter;
    };

    expr exp_term;

    if (h->has_exp() == true)
        exp_term    = make(h->Exp());

    ast::expr_handle ex_h   = exp_term.is_null() ? nullptr : exp_term.get_ptr().get();

    ast::mult_build_info<iitem, ritem> bi(in, ipow_ptr, rn, rpow_ptr, ex_h);
    ast::expr_ptr simpl = ast::mult_build::make(bi);

    expr ret    = expr(std::move(simpl));
    ret.cannonize(true);

    insert(h, ret);

    return ret;
};

expr do_simplify_vis::eval(const ast::function_rep* h)
{
    auto ex             = find(h);

    if (ex.empty() == false)
        return expr(ex->get_value());

    size_t n                = h->size();

    int size_counter        = 0;
    using expr_pod          =  sd::pod_type<expr>;
    expr_pod::destructor_type d(&size_counter);
    sd::stack_array<expr_pod> buff(n, &d);    

    expr* buff_ptr          = reinterpret_cast<expr*>(buff.get());

    for (size_t j = 0; j < n; ++j)
    {
        expr simpl          = visit(h->arg(j));

        new(buff_ptr + size_counter) expr(std::move(simpl));
        ++size_counter;
    };

    //TODO: avoid full rebuilding of function
    expr ret                = function(identifier(h->name()), buff_ptr, n);

    ret.cannonize(true);
    insert(h, ret);

    return ret;
};

}};

namespace sym_arrow
{

expr sym_arrow::simplify(const expr& ex)
{
    ex.cannonize(false);

    const ast::expr_base* h     = ex.get_ptr().get();
    expr ret    = details::do_simplify_vis().make(h);
    return ret;
};

};