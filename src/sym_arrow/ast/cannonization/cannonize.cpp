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

#include "sym_arrow/ast/cannonization/cannonize.h"
#include "sym_arrow/ast/builder/add_build.h"

#include "sym_arrow/ast/cannonization/item_collector.inl"
#include "sym_arrow/ast/cannonization/simplifier.inl"
#include "sym_arrow/ast/add_rep.inl"
#include "sym_arrow/ast/mult_rep.inl"
#include "sym_arrow/ast/ast.h"
#include "sym_arrow/func/compound.h"
#include "sym_arrow/ast/cannonization/cse_hash.h"

#include "sym_arrow/ast/cannonization/subexpr_collector.h"
#include "dag/details/vector_provider.h"

namespace sym_arrow { namespace ast
{

namespace md = mmlib :: details;

//-------------------------------------------------------------------
//                  cannonize
//-------------------------------------------------------------------
cannonize::cannonize()
    :m_expr_stack(nullptr)
{};

cannonize::~cannonize()
{
    if (m_expr_stack)
        delete m_expr_stack;
};

void cannonize::hold_expr(const expr& ex)
{
    if (!m_expr_stack)
        m_expr_stack    = new expr_stack();

    m_expr_stack->push_back(ex);
}

bool cannonize::is_simple(const add_rep* h) const
{
    if (h->size() == 1 && h->has_log() == false && h->V0().is_zero())
        return true;
    else
        return false;
};

bool cannonize::is_normalized(const add_rep* h) const
{
    return h->is_normalized();
};

expr_ptr cannonize::normalize(const add_rep* h, value& scal) const
{
    if (h->is_normalized() == true)
        return expr_ptr(h, sym_dag::copy_t());

    scal = get_normalize_scaling(h->V0(), h->size(), h->VE());

    if (scal.is_one() == true)
    {
        const_cast<add_rep*>(h)->set_normalized();
        return expr_ptr(h, sym_dag::copy_t());
    }

    using item_handle   = build_item<value>::handle_type;
    static const size_t ih_buffer_size = 20;

    //destructor is trivial
    md::stack_array<md::pod_type<item_handle>,ih_buffer_size> 
    ih_array(h->size() + 1);

    item_handle* ih = (item_handle*)ih_array.get();

    for (size_t i = 0; i < h->size(); ++i)
        new(ih + i) item_handle(h->V(i) / scal, h->E(i));

    bool has_log = h->has_log();

    if (has_log == true)
        new(ih + h->size()) item_handle(value::make_one(), h->Log());

    add_rep_info<item_handle> ai(h->V0() / scal, h->size(), ih, 
                                 has_log ? ih + h->size() : nullptr);

    using add_rep_ptr = sym_dag::dag_ptr<add_rep>;

    add_rep_ptr res = add_rep::make(ai);
    const_cast<add_rep*>(res.get())->set_normalized();
    return res;
};

expr cannonize::make(const expr& ex, bool do_cse) 
{    
    if (!ex.get_ptr())
        return ex;

    switch (ex.get_ptr()->get_code())
    {
        case (size_t)term_types::add_build:
        {
            const add_build* ab = ex.get_ptr()->static_cast_to<add_build>();        
            return make_add(ab, do_cse);
        }
        case (size_t)term_types::mult_build:
        {
            const mult_build* ab = ex.get_ptr()->static_cast_to<mult_build>();
            return make_mult(ab, do_cse);
        }
        default:
        {
            return ex;
        }
    };
};

expr cannonize::make_cse(const expr& ex)
{    
    if (!ex.get_ptr())
        return ex;

    switch (ex.get_ptr()->get_code())
    {
        case (size_t)term_types::add_build:
        {
            const add_build* ab = ex.get_ptr()->static_cast_to<add_build>();        
            return make_add(ab, true);
        }
        case (size_t)term_types::mult_build:
        {
            const mult_build* ab = ex.get_ptr()->static_cast_to<mult_build>();
            return make_mult(ab, true);
        }
        case (size_t)term_types::add_rep:
        {
            const add_rep* ab   = ex.get_ptr()->static_cast_to<add_rep>();
            expr ret            = make_add_impl(ab);
            return ret;
        }
        default:
        {
            return ex;
        }
    };
};


bool cannonize::is_cannonized(expr_handle ex) const
{
    switch (ex->get_code())
    {
        case (size_t)term_types::add_build:
        case (size_t)term_types::mult_build:
            return false;
        default:
            return true;
    };
};

bool cannonize::is_cannonized(const expr& ex) const
{    
    if (!ex.get_ptr())
        return true;

    return is_cannonized(ex.get_ptr().get());
};

expr cannonize::make_add(const add_build* h, bool do_cse)
{
    value scal  = value::make_one();
    expr ret    = make_add_impl(h, scal, false, do_cse);
    return ret;
};

expr cannonize::make_mult(const mult_build* h, bool do_cse)
{
    expr ret = h->get_cannonized();
    
    if (ret.is_null() == false)
        return ret;

    ret = make_mult_impl(h, do_cse);
    h->set_cannonized(ret);

    return ret;
};

expr cannonize::make_normalize(const add_build* h, value& scal, bool do_cse)
{
    expr ret = h->get_cannonized(scal);
    
    if (ret.is_null() == false)
        return ret;

    ret = make_add_impl(h, scal, true, do_cse);
    h->set_cannonized(ret, scal);

    return ret;
};

expr cannonize::make_add_impl(const add_build* h, value& ret_scal, bool normalize,
                              bool do_cse)
{    
    size_t n            = calc_size_add(h->get_vlist());
    value scal          = value::make_one();

    item_collector_add ic(n);    
    collect_items_root(h->get_vlist(), ic, scal);

    return process_add(n, ic, ret_scal, normalize, do_cse);
}

expr cannonize::make_add_impl(const add_rep* h)
{    
    size_t n            = h->size();
    value scal          = value::make_one();

    item_collector_add ic(n);    
    collect_items(h, ic, scal);

    value ret_scal;
    return process_add(n, ic, ret_scal, false, true);
}

expr cannonize::make_normalize(const value& add, size_t size, 
                    const build_item<value>* h, value& ret_scal, bool do_cse)
{
    using item          = build_item<value>;
    using item_handle   = item::handle_type;

    size_t n            = 0;
    for (size_t i = 0; i < size; ++i)
        n               += calc_size_add(h[i]);    

    item_collector_add ic(n);

    ic.add_add(add);

    value scal          = value::make_one();
    collect_items_root(size, h, ic, scal);

    return process_add(n, ic, ret_scal, true, do_cse);
};

expr cannonize::process_add(size_t n, item_collector_add& ic, value& ret_scal,
                            bool normalize, bool do_cse)
{
    using item          = build_item<value>;
    using item_handle   = item::handle_type;

    process_log(ic, do_cse);

    n                   = ic.size();
    item_handle* ih     = ic.get_ih();
    ic.test();    

    simplify_expr<item_handle>::make(ih, n);

    if (n < 2 || do_cse == false)
        return finalize_add(ic, n, ret_scal, normalize, true);

    expr ex_test;
    value scal_test;
    bool check_hash     = false;
    cse_hash& hashed    = cse_hash::get();

    if (normalize == true)
    {
        check_hash      = hashed.check_hash();

        if (check_hash == true)
        {
            expr ex_simpl;
            value simpl_norm;

            ex_test         = finalize_add(ic, n, scal_test, true, false);
            size_t refcount = ex_test.get_expr_handle()->refcount();
            bool succ       = false;

            if (refcount != 1)
            {
                // only expr with refcoun > 1 could be hashed
                succ        = hashed.get_hashed_subexpr_elim(ex_test, 
                                simpl_norm, ex_simpl);
            }            

            if (succ == true)
            {
                hashed.add_observation(succ, check_hash);

                ret_scal    = simpl_norm * scal_test;
                return ex_simpl;
            }
        };
    };

    bool factorized     = false;

    for (;;)
    {
        expr fact;
        value scal;

        bool succ       = subexpr_collector().make(n, ih, fact, scal);
        factorized      |= succ;

        if (succ == false)
            break;        

        //std::cout << "\n";
        //disp(fact);

        process_add_factorization(fact, ic, n, scal, true);
        
        if (n < 2)
            break;
    };

    if (normalize == true)
    {
        hashed.add_observation(factorized, check_hash);

        if (factorized == false && check_hash == true)
        {
            // expr was not factorized but finalize_add was called;

            ret_scal = scal_test;
            return ex_test;
        };
    };

    expr ret    = finalize_add(ic, n, ret_scal, normalize, true);

    if (factorized == true && check_hash == true)
    {
        // add factorization result to hash table; rescale 
        // normalization constant as if normalized expr was factorized
        hashed.set_hashed_subexpr_elim(ex_test, ret_scal / scal_test, ret);
    };

    return ret;
};

void cannonize::process_add_factorization(const expr& fact, item_collector_add& ic,
                                          size_t& n, const value& scal, bool do_cse)
{
    hold_expr(fact);

    // common subexpression found, ih is no longer valid
    bool is_add     = fact.get_ptr()->isa<add_rep>();
    bool is_scal    = fact.get_ptr()->isa<scalar_rep>();
    size_t nf       = 1;

    if (is_add == true)
        nf          = fact.get_ptr()->static_cast_to<add_rep>()->size();
    else if (is_scal)
        nf          = 0;

    ic.set_size(n);
    ic.remove_empty();
    ic.reserve(nf);

    if (is_add == true)
    {
        const add_rep* ap   = fact.get_ptr()->static_cast_to<add_rep>();
        collect_items(ap, ic, scal);

        // we need to simplify log subterms immediately
        process_log(ic, do_cse);
    }
    else if (is_scal == true)
    {
        const scalar_rep* sp = fact.get_ptr()->static_cast_to<scalar_rep>();
        ic.add_add(sp->get_data());
    }
    else
    {
        ic.add(scal, fact.get_ptr().get());
    }

    using item          = build_item<value>;
    using item_handle   = item::handle_type;

    n                   = ic.size();
    item_handle* ih     = ic.get_ih();
    ic.test();    

    simplify_expr<item_handle>::make(ih, n);
};

void cannonize::process_log(item_collector_add& ic, bool do_cse)
{
    using item          = build_item<value>;
    using item_handle   = item::handle_type;

    bool has_log        = ic.has_log();

    if (has_log == false)
        return;

    expr ex_log         = ic.get_log();    

    ex_log              = make(ex_log, do_cse);

    if (ex_log.get_ptr()->isa<scalar_rep>() == true)
    {
        value add       = ic.get_add();
        value v         = log(ex_log.get_ptr()->static_cast_to<scalar_rep>()->get_data());
        add             = add + v;

        ic.set_log(expr());
        ic.set_add(add);

        return;
    }
    else if (ex_log.get_ptr()->isa<add_rep>() == true)
    {
        const add_rep* ah   = ex_log.get_ptr()->static_cast_to<add_rep>();

        // simplify simple add
        if (ah->size() == 1 && ah->has_log() == false && ah->V0().is_zero() == true)
        {
            value add       = ic.get_add();
            add             = add + log(ah->V(0));

            expr ex_log2    = expr(ah->E(0));

            ic.set_log(ex_log2);
            ic.set_add(add);

            return;
        };
    }
    else if (ex_log.get_ptr()->isa<mult_rep>() == true)
    {
        const mult_rep* mh  = ex_log.get_ptr()->static_cast_to<mult_rep>();

        //simplify exp
        if (mh->has_exp() == true)
        {
            // this case seems to be impossible; exp terms in log subterms
            // are removed immediately; new exp term can be introduced only
            // during common expression elimination, but current rules does
            // not allow for this; no test case was found, that visit this point
            assertion(false, "exp error in process_log");
        }
    };

    ic.set_log(ex_log);
    return;
};

expr cannonize::finalize_add(item_collector_add& ic, size_t n, value& ret_scal, 
                            bool normalize, bool can_modify_ic)
{
    using item          = build_item<value>;
    using item_handle   = item::handle_type;

    item_handle* ih     = ic.get_ih();
    bool has_log        = ic.has_log();
    value add           = ic.get_add();
    expr ex_log         = ic.get_log();
    ret_scal            = value::make_one();

    if (n == 0)
    {
        if (has_log == true)
        {
            item_handle ih_log(value::make_one(), ex_log.get_ptr().get());

            add_rep_info<item_handle> ci(add, 0, nullptr, &ih_log);
            return expr(add_rep::make(ci));
        }
        else
        {
            return expr(add);
        };        
    };

    expr ret;
    expr_ptr ep;

    if (has_log == true)
    {
        // add_rep must be created; last element can always be modified
        ih[n].m_value   = value::make_one();
        ih[n].m_expr    = ex_log.get_ptr().get();

        add_rep_info<item_handle> ai(add, n, ih, ih + n);
        ret = expr(add_rep::make(ai));
        return ret;
    };

    if (n == 1 && add.is_zero() == true)
    {
        if (normalize == true)
        {
            ret_scal = ih[0].m_value;
            return expr(ih[0].m_expr);
        };

        if (ih[0].m_value.is_one() == true)
            return expr(ih[0].m_expr);
    };

    if (normalize == true)
    {
        ret_scal = get_normalize_scaling(add, n, ih);

        if (ret_scal.is_one() == false)
        {
            if (can_modify_ic == false)
            {
                static const size_t stack_size  = 20;
                using stack_elem    = md::pod_type<item_handle>;
                using item_array    = md::stack_array<stack_elem, stack_size>;

                item_array loc_arr(n);

                item_handle* loc_ih = (item_handle*)loc_arr.get();

                for (size_t i = 0; i < n; ++i)
                    new (loc_ih +i) item_handle(ih[i].m_value / ret_scal, ih[i].m_expr);

                add = add / ret_scal;

                add_rep_info<item_handle> ci(add, n, loc_ih, nullptr);

                using add_rep_ptr   = sym_dag::dag_ptr<add_rep>;
                add_rep_ptr res     = add_rep::make(ci);

                return expr(std::move(res));
            };

            //scal items
            for (size_t i = 0; i < n; ++i)
                ih[i].m_value = ih[i].m_value / ret_scal;

            add = add / ret_scal;
        };
    };

    add_rep_info<item_handle> ci(add, n, ih, nullptr);

    using add_rep_ptr   = sym_dag::dag_ptr<add_rep>;
    add_rep_ptr res     = add_rep::make(ci);

    if (normalize == true)
        const_cast<add_rep*>(res.get())->set_normalized();

    return expr(std::move(res));    
};

expr cannonize::make_mult_impl(const mult_build* h, bool do_cse)
{
    using iitem             = build_item<int>;
    using ritem             = build_item<value>;
    using iitem_handle      = iitem::handle_type;
    using ritem_handle      = ritem::handle_type;

    item_collector_size ic_size;
    calc_size(h, ic_size);

    item_collector_mult_arrays arr(ic_size);
    item_collector_mult ic(ic_size, arr);

    ic.add_scal(h->get_scal());

    if (h->get_ilist())
        collect_items(h->get_ilist(), ic, int(1));

    if (h->get_rlist())
        collect_items(h->get_rlist(), ic, value::make_one());
    
    ic.test();

    value scal;
    expr ret                = process_mult(ic, scal, do_cse);

    if (scal.is_one())
        return ret;

    expr_ptr ep             = ret.get_ptr();

    if (ep->isa<add_rep>() == false)
    {
        struct val_expr
        {
            const value*        m_value;
            expr_handle         m_expr;
            const value&        get_value() const       { return *m_value; };
            expr_handle         get_expr_handle() const { return m_expr; };
        };

        val_expr data           = {&scal, ep.get()};

        add_rep_info<val_expr> ai2(value::make_zero(), 1, &data, nullptr);
        expr_ptr ep2 = add_rep::make(ai2);

        return expr(std::move(ep2));
    };

    expr tmp    = scal * expr(std::move(ep));
    tmp         = make(tmp, do_cse);

    return tmp;
}

expr cannonize::process_mult(item_collector_mult& ic, value& scal, bool do_cse)
{
    using iitem             = build_item<int>;
    using ritem             = build_item<value>;
    using iitem_handle      = iitem::handle_type;
    using ritem_handle      = ritem::handle_type;

    ic.collect_base();

    iitem_handle* iih   = ic.get_iih();
    ritem_handle* rih   = ic.get_rih();         
    scal                = ic.get_scal();    

    bool has_exp_term;
    expr ex_term;
    expr log_term;

    process_exp(ic, has_exp_term, scal, ex_term, log_term, do_cse);

    if (scal.is_zero() == true)
    {
        expr ret        = scalar_rep::make_zero();
        scal            = value::make_one();
        return ret;
    }

    size_t in       = ic.isize();
    size_t rn       = ic.rsize();

    simplify_expr<ritem_handle>::make(rih, rn);
    simplify_expr<iitem_handle>::make(iih, in);

    if (in == 0 && rn == 0)
    {
        if (has_exp_term == true)
        {
            if (log_term.is_null() == false)
                return make(make_exp_mult_rep(ex_term) * std::move(log_term), do_cse);
            else
                return make_exp_mult_rep(ex_term);
        }
        else if (log_term.is_null() == false)
        {
            return log_term;
        }
        
        expr ret    = scalar(scal);
        scal        = value::make_one();
        return ret;
    };
    
    expr_ptr ep;

    if (has_exp_term)
    {
        iih[in].m_value     = 1;
        iih[in].m_expr      = ex_term.get_ptr().get();
    };

    if (in == 1 && rn == 0 && has_exp_term == false && iih[0].m_value == 1)
    {
        //promote
        ep  = expr_ptr(iih[0].m_expr, sym_dag::copy_t());
    }
    else
    {
        mult_rep_info<iitem_handle,ritem_handle> 
        ai(in,iih, has_exp_term? iih + in : nullptr, rn,rih);

        ep = mult_rep::make(ai);
    }

    if (log_term.is_null() == true)
        return expr(std::move(ep));

    // log term is rare; it may happen only due to simplifications in exp term
    // just rebuild 
    return make(expr(std::move(ep)) * std::move(log_term), do_cse);
};

void cannonize::process_exp(item_collector_mult& ic, bool& has_exp, value& scal,
                            expr& ex_term, expr& log_term, bool do_cse)
{
    has_exp             = ic.has_exp();

    if (has_exp == false)
        return;

    ex_term             = ic.get_exp();        
    ex_term.cannonize(do_cse);

    if (ex_term.get_ptr()->isa<add_rep>() == true)
    {
        const add_rep* ah   = ex_term.get_ptr()->static_cast_to<add_rep>();
        bool has_log        = ah->has_log();

        if (ah->V0().is_zero() == false || has_log == true)
        {
            {
                const value& add    = ah->V0();
                scal                = scal * exp(add);
            };

            if (has_log == true)
                log_term        = expr(expr_ptr::from_this(ah->Log()));

            add_rep::remove_add_log(ah, ex_term);            
        };
    };

    if (ex_term.get_ptr()->isa<scalar_rep>() == true)
    {
        value v     = ex_term.get_ptr()->static_cast_to<scalar_rep>()->get_data();
        scal        = scal * exp(v);
        has_exp     = false;
    }
};

expr cannonize::make_mult(size_t isize, size_t rsize, bool has_exp, 
                    build_item_handle<int>* ih, build_item_handle<value>* rh, 
                    value& scal, bool do_cse)
{
    using iitem         = build_item<int>;
    using ritem         = build_item<value>;
    using iitem_handle  = iitem::handle_type;
    using ritem_handle  = ritem::handle_type;

    item_collector_size ic_size(isize, rsize);

    item_collector_mult ic(ic_size, ih, rh);

    ic.add_scal(scal);

    if (has_exp)
    {
        expr exp        = expr(expr_ptr::from_this(ih[isize].m_expr));
        ic.add_exp(exp);
    }

    return process_mult(ic, scal, do_cse);
};

void cannonize::calc_size(const mult_build* h, item_collector_size& ic) const
{
    if (h->get_ilist())
        calc_size(h->get_ilist(), ic);

    if (h->get_rlist())
        calc_size(h->get_rlist(), ic);
};

size_t cannonize::calc_size_add(const vlist_add& vl) const
{
    size_t size     = 0;
    size_t cur_size = vl.current_size();

    for (size_t i = 0; i < cur_size; ++i)
        size += calc_size_add(vl.elem(i));

    if (vl.has_previous())
        size += calc_size_add(*vl.get_previous());

    return size;
};

size_t cannonize::calc_size_add(const build_item<value>& it) const
{
    if (it.is_special() == true)
        return 0;

    auto type = it.get_expr_handle()->get_code();
    
    switch (type)
    {
        case (size_t)term_types::add_rep:
            return it.get_expr_handle()->static_cast_to<add_rep>()->size();

        case (size_t)term_types::mult_rep:
            return 1;

        case (size_t)term_types::add_build:
            return calc_size_add(it.get_expr_handle()->static_cast_to<add_build>());

        case (size_t)term_types::mult_build:
            assertion(0,"invalid case");
            return 1;

        default:
            return 1;
    };
};

size_t cannonize::calc_size_add(const add_build* h) const
{
    return calc_size_add(h->get_vlist());
};

template<class Value_t, class Derived>
void cannonize::calc_size(const vlist_mult<Value_t,Derived>* vl, 
                                item_collector_size& ic) const
{
    size_t size = vl->current_size();

    for (size_t i = 0; i < size; ++i)
        calc_size<Value_t, Derived>(ic, vl->elem(i));

    if (vl->has_previous())
        calc_size(vl->get_previous(), ic);
};

template<class Value_t, class Derived>
void cannonize::calc_size(item_collector_size& ic, const build_item<Value_t>& it) const
{
    if (it.is_special() == true)
    {
        ic.report_special();
        return;
    };

    auto type = it.get_expr_handle()->get_code();

    switch(type)
    {
        case (size_t)term_types::mult_rep:
        {
            ic.report_mult<Derived::is_ilist>(it.get_expr_handle()->static_cast_to<mult_rep>());
            return;
        }
        case (size_t)term_types::add_rep:
        {
            const add_rep* aa = it.get_expr_handle()->static_cast_to<add_rep>();

            if (is_simple(aa) == true)
            {
                if (aa->E(0)->isa<mult_rep>())
                {
                    const mult_rep* am = aa->E(0)->static_cast_to<mult_rep>();
                    ic.report_mult<Derived::is_ilist>(am);
                    return;
                };
            };

            ic.report_atom<Derived::is_ilist>();
            return;
        }
        case (size_t)term_types::mult_build:
        {
            item_collector_size ic2;
            calc_size(it.get_expr_handle()->static_cast_to<mult_build>(), ic2);

            ic.add<Derived::is_ilist>(ic2);
            return;
        }
        case (size_t)term_types::add_build:
        {
            assertion(0,"invalid case");
            return;
        }
        default:
        {
            ic.report_atom<Derived::is_ilist>();
            return;
        }
    };
};

void cannonize::collect_items_root(const vlist_add& h, item_collector_add& ic, 
                                  const value& scal) const
{
    using vector_handle     = sym_dag::vector_handle<aab_value>;
    using vector_provider   = sym_dag::vector_provider<aab_value>;
    
    vector_handle stack_handle  = vector_provider::get_global().get_vector();
    collect_stack_type& stack   = stack_handle.get();

    collect_items(h, ic, scal, stack);
    collect_items_stack(ic, stack);
}

void cannonize::collect_items_root(size_t size, const build_item<value>* arr, 
                    item_collector_add& ic, const value& scal) const
{
    using vector_handle     = sym_dag::vector_handle<aab_value>;
    using vector_provider   = sym_dag::vector_provider<aab_value>;
    
    vector_handle stack_handle  = vector_provider::get_global().get_vector();
    collect_stack_type& stack   = stack_handle.get();

    for (size_t i = 0; i < size; ++i)
        collect_items(ic, scal, arr[i], stack);

    collect_items_stack(ic, stack);
};

void cannonize::collect_items_stack(item_collector_add& ic, collect_stack_type& stack) const
{
    size_t i = 0;

    for(;;)
    {
        size_t N = stack.size();

        while (i < N)
        {
            aab_value tmp = stack[i];
            ++i;

            collect_items(tmp.first->get_vlist(), ic, tmp.second, stack);
        };

        if (N == stack.size())
            break;
    };

    stack.clear();
};

void cannonize::collect_items(const vlist_add& h, item_collector_add& ic, const value& scal, 
                    collect_stack_type& stack) const
{
    ic.add_add(h.get_add() * scal);

    size_t size = h.current_size();

    for(size_t i = 0; i < size; ++i)
        collect_items(ic, scal, h.elem(i), stack);

    if (h.has_previous())
        collect_items(*h.get_previous(), ic, scal * h.get_scal(), stack);
};

void cannonize::collect_items(item_collector_add& ic, const value& scal, 
                const build_item<value>& elem, collect_stack_type& stack) const
{
    if (elem.is_special() == true)
    {
        expr tmp_ex         = expr(elem.get_expr_handle());
        value tmp_scal      = elem.get_value() * scal;

        ic.add_log(tmp_scal, std::move(tmp_ex));
        return;
    };

    switch(elem.get_expr_handle()->get_code())
    {
        case (size_t)term_types::mult_build:
        {
            //not possible
            assertion(0,"invalid case");
            return;
        }
        case (size_t)term_types::add_build:
        {
            const add_build* mb = elem.get_expr_handle()->static_cast_to<add_build>();
            aab_value tmp(mb, elem.get_value()*scal);
            stack.push_back(tmp);
            return;
        }
        case (size_t)term_types::add_rep:
        {
            const add_rep* am   = elem.get_expr_handle()->static_cast_to<add_rep>();
            value tmp_scal      = elem.get_value() * scal;
            return collect_items(am, ic, tmp_scal);
        }
        case (size_t)term_types::mult_rep:
        default:
        {
            expr_handle tmp_ex  = elem.get_expr_handle();            
            value tmp_scal      = elem.get_value()*scal;

            ic.add(tmp_scal, tmp_ex);

            return;
        }
    };
};

void cannonize::collect_items(const add_rep* am, item_collector_add& ic, 
                              const value& tmp_scal) const
{                
    ic.add_add(am->V0() * tmp_scal);
            
    size_t n = am->size();            

    for (size_t i = 0; i < n; ++i)
    {
        expr_handle tmp_ex  = am->E(i);
        value tmp_scal2     = am->V(i) * tmp_scal;

        ic.add(tmp_scal2, tmp_ex);
    };
            
    if (am->has_log() == true)
    {
        expr_handle tmp_log = am->Log();
        value tmp_scal2     = tmp_scal;

        ic.add_log(tmp_scal, expr(tmp_log));
    };
    return;
};

template<class Pow_value>
void cannonize::collect_items(const ilist_mult* il, item_collector_mult& ic, 
                             const Pow_value& pow) const
{
    size_t size = il->current_size();

    for(size_t i = 0; i < size; ++i)
        collect_items<int>(ic, pow, il->elem(i));

    if (il->has_previous())
        collect_items(il->get_previous(), ic, pow * il->get_pow());
};

template<class Pow_value>
void cannonize::collect_items(const rlist_mult* rl, item_collector_mult& ic, 
                              const Pow_value& pow) const
{
    size_t size = rl->current_size();

    for(size_t i = 0; i < size; ++i)
        collect_items<value>(ic, pow, rl->elem(i));

    if (rl->has_previous())
        collect_items(rl->get_previous(), ic, pow * rl->get_pow());
};

static value operator*(const value& a, int b)
{
    return a * value::make_value(b);
}

static value operator*(int b, const value& a)
{
    return a * value::make_value(b);
}

static expr operator*(expr&& a, int b)
{
    return std::move(a) * value::make_value(b);
}

template<class Value_t, class Pow_value>
void cannonize::collect_items(item_collector_mult& ic, const Pow_value& pow, 
                    const build_item<Value_t>& elem) const
{
    bool is_spec            = elem.is_special();
    if (is_spec == true)
    {
        expr tmp_ex         = expr(elem.get_expr_handle());
        auto tmp_pow        = elem.get_value() * pow;
        ic.add_exp(std::move(tmp_ex) * std::move(tmp_pow));
        return;
    };

    switch(elem.get_expr_handle()->get_code())
    {
        case (size_t)term_types::add_build:
        {
            //not possible
            assertion(0,"invalid case");
            return;
        }
        case (size_t)term_types::mult_build:
        {
            const mult_build* mb = elem.get_expr_handle()->static_cast_to<mult_build>();
            collect_items(*mb, ic, elem.get_value() * pow);
            return;
        }
        case (size_t)term_types::mult_rep:
        {
            const mult_rep* am = elem.get_expr_handle()->static_cast_to<mult_rep>();
            ic.collect(am, elem.get_value() * pow);
            return;
        }
        case (size_t)term_types::add_rep:
        {
            const add_rep* aa = elem.get_expr_handle()->static_cast_to<add_rep>();

            if (is_simple(aa) == true)
            {
                expr_handle tmp_ex  = aa->E(0);
                auto pow2           = elem.get_value() * pow;

                ic.make_power(aa->V(0),pow2);

                if (tmp_ex->isa<mult_rep>() == false)
                {
                    ic.add(std::move(pow2), tmp_ex);
                    return;
                }
                else
                {
                    const mult_rep* am = tmp_ex->static_cast_to<mult_rep>();
                    ic.collect(am, std::move(pow2));
                    return;
                };
            }
            else
            {                
                expr_handle tmp_ex  = elem.get_expr_handle();
                auto tmp_pow        = elem.get_value() * pow;

                ic.add(std::move(tmp_pow), tmp_ex);
                return;
            };
        }
        default:
        {
            expr_handle tmp_ex      = elem.get_expr_handle();
            auto tmp_pow            = elem.get_value() * pow;

            ic.add(std::move(tmp_pow), tmp_ex);
            return;
        }
    };
};

template<class Pow_value>
void cannonize::collect_items(const mult_build& h, item_collector_mult& ic, const Pow_value& pow) const
{
    ic.make_power(h.get_scal(), pow);

    if (h.get_ilist())
        collect_items(h.get_ilist(), ic, pow);

    if (h.get_rlist())
        collect_items(h.get_rlist(), ic, pow);
};

expr cannonize::make_exp_mult_rep(const expr& ex_exp) const
{
    using iitem             = build_item<int>;
    using ritem             = build_item<value>;
    using iitem_handle      = iitem::handle_type;
    using ritem_handle      = ritem::handle_type;

    using atom_mult_info_type = mult_rep_info<iitem_handle, ritem_handle>;

    iitem_handle exp_expr_handle(1, ex_exp.get_ptr().get());

    atom_mult_info_type am_info(0, nullptr, &exp_expr_handle, 0, nullptr);
    expr_ptr am_ptr         = mult_rep::make(am_info);

    return expr(std::move(am_ptr));
};

};};
