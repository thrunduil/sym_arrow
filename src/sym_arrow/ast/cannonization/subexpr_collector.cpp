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

#include "subexpr_collector.h"
#include "sym_arrow/ast/ast.h"
#include "sym_arrow/functions/expr_functions.h"
#include "sym_arrow/ast/cannonization/cannonize.h"
#include "sym_arrow/ast/mult_rep.inl"
#include "sym_arrow/utils/stack_array.h"
#include "sym_arrow/utils/sort.h"
#include "sym_arrow/ast/cannonization/simplifier.inl"
#include "sym_arrow/utils/tests.h"

namespace sym_arrow { namespace ast
{

namespace sd = sym_arrow::details;

//-------------------------------------------------------------------
//                  helpers
//-------------------------------------------------------------------
struct int_comparer
{
    bool operator()(int a, int b) const
    {
        return a < b;
    };
};

struct real_pow_comparer
{
    bool operator()(const value& a, const value& b) const
    {
        return a < b;
    };
};

struct add_pos_comparer
{
    template<class Item>
    bool operator()(const Item& i1, const Item& i2)const
    {
        return i1.m_add_pos < i2.m_add_pos;
    };
};

subs_counter::subs_counter()
    :n_ipow(0), n_rpow(0), n_exp(0)
{};

void subs_counter::add_ipow(size_t n_elems)
{
    n_ipow += n_elems;
}

void subs_counter::add_rpow(size_t n_elems)
{
    n_rpow += n_elems;
}

void subs_counter::add_exp()
{
    n_exp += 1;
}

bool ipow_comparer::operator()(const ipow_item& a, const ipow_item& b) const
{
    if (a.m_expr < b.m_expr)
        return true;

    if (a.m_expr > b.m_expr)
        return false;

    return a.m_pow < b.m_pow;
};

bool rpow_comparer::operator()(const rpow_item& a, const rpow_item& b) const
{
    if (a.m_expr < b.m_expr)
        return true;

    if (a.m_expr > b.m_expr)
        return false;

    return a.m_pow < b.m_pow;
};

bool exp_comparer::operator()(const exp_item& a, const exp_item& b) const
{
    return a.m_expr < b.m_expr;
};

factorizations::factorizations(subs_vec& subs)
    :m_vec(subs)
{};

size_t factorizations::size() const
{
    return m_vec.size();
}

void factorizations::add(const mult_subexpr& item)
{
    if (m_vec.empty() == true)
    {
        m_vec.push_back(item);
        return;
    };

    cmp_result res  = item.compare(m_vec[0]);

    if (res == cmp_result::better)
    {
        // if given substitution is better than the first one
        // then is better than all other in best set

        m_vec.clear();
        m_vec.push_back(item);
    }
    else if (res == cmp_result::equal)
    {
        // if given substitution is equivalent the first one
        // then is equivalent to all other in best set
        m_vec.push_back(item);
    };
};

factor_group_stats::factor_group_stats::factor_group_stats()
    :n_symbols(0),  m_base(nullptr)
{}

factor_group_stats::factor_group_stats(size_t symbols, const dbs& syms, 
                                       expr_handle eh_best)
    :n_symbols(symbols), m_symbol_set(syms), m_base(eh_best)
{}

bool factor_group_stats::operator<(const factor_group_stats& other) const
{
    if (this->n_symbols < other.n_symbols)
        return true;
    if (this->n_symbols > other.n_symbols)
        return false;

    dbs_lib::order_type ord  = compare(this->m_symbol_set, other.m_symbol_set);

    if (ord == dbs_lib::order_type::less)
        return true;
    if (ord == dbs_lib::order_type::greater)
        return false;

    size_t rc_1 = this->m_base->refcount();
    size_t rc_2 = other.m_base->refcount();

    if (rc_1 < rc_2)
        return true;
    if (rc_2 < rc_1)
        return false;
    
    return this->m_base < other.m_base;
};

//-------------------------------------------------------------------
//                  subexpr_collector
//-------------------------------------------------------------------
bool subexpr_collector::make(size_t n, add_item_handle* ih, expr& res, 
                             value& scal, temp_value& temp_vals)
{
    if (n < 2)
        return false;

    m_temp_vals = &temp_vals;
    subs_counter counter;
    
    for (size_t i = 0; i < n; ++i)
        calc_size(ih[i], counter);

    if (counter.n_ipow <= 1 && counter.n_rpow <= 1 && counter.n_exp <= 1)
    {
        // nothing to collect
        return false;
    };

    // collect subexpressions
    static const size_t buffer_size = 10;

    static_assert(sd::is_effective_pod<ipow_item>::value == true, "pod required");
    sd::stack_array<sd::pod_type<ipow_item>, buffer_size> ipow_array(counter.n_ipow);
    ipow_item* ibuf = (ipow_item*)ipow_array.get();

    static_assert(sd::is_effective_pod<exp_item>::value == true, "pod required");
    sd::stack_array<sd::pod_type<exp_item>, buffer_size> epow_array(counter.n_exp);
    exp_item* ebuf = (exp_item*)epow_array.get();

    using rpow_pod  = sd::pod_type<rpow_item>;
    int count_rpow  = (int)counter.n_rpow;
    rpow_pod::destructor_type d(&count_rpow);

    sd::stack_array<rpow_pod, buffer_size> rpow_array(counter.n_rpow, &d);
    rpow_item* rbuf = (rpow_item*)rpow_array.get();

    {
        size_t pos_ipow = 0;
        size_t pos_rpow = 0;
        size_t pos_epow = 0;

        for (size_t i = 0; i < n; ++i)
        {
            collect_mult_ipow(ih[i], i, ibuf, pos_ipow);
            collect_mult_rpow(ih[i], i, rbuf, pos_rpow);
            collect_mult_epow(ih[i], i, ebuf, pos_epow);
        }
    };

    // sort subexpressions
    if (counter.n_ipow > 1)
        sym_arrow::utils::sort_q(ibuf, counter.n_ipow, ipow_comparer()); 

    if (counter.n_rpow > 1)
        sym_arrow::utils::sort_q(rbuf, counter.n_rpow, rpow_comparer()); 

    if (counter.n_exp > 1)
        sym_arrow::utils::sort_q(ebuf, counter.n_exp, exp_comparer()); 

    // identify factorization possibilities

    using vec_provider  = sym_dag::vector_provider<mult_subexpr>;
    auto vec_handle     = vec_provider::get_global().get_vector();

    sub_type_vector& subs  = vec_handle.get();
    subs.clear();

    factorizations fact(subs);

    if (counter.n_exp > 1)
        select_subexpr_epow(ebuf, counter.n_exp, fact);

    if (counter.n_rpow > 1)
        select_subexpr_rpow(rbuf, counter.n_rpow, fact);

    if (counter.n_ipow > 1)
    {
        bool has_equal  = fact.size() > 0;
        select_subexpr_ipow(ibuf, counter.n_ipow, fact, has_equal);
    }

    size_t size = fact.size();

    if (size == 0)
        return false;    

    // select best subexpression to factorize
    size_t group_size   = 0;
    select_subexpr(subs, ibuf, rbuf, ebuf, group_size);

    // when group_size > 1, then factorization elements must be
    // sorted according to add_pos
    if (group_size > 1)
        sort_subexpr(subs, ibuf, rbuf, ebuf, group_size);

    if (group_size == 0)
        return false;

    factor_subexpr(ih, ibuf, rbuf, ebuf, subs, group_size, res, scal);
    return true;
};

void subexpr_collector::factor_subexpr(add_item_handle* ih, ipow_item* ipow_arr, 
                    rpow_item* rpow_arr, exp_item* exp_arr, sub_type_vector& sub_types,
                    size_t group_size, expr& res, value& ret_scal)
{
    // these values are the same for all factorizations
    size_t length       = sub_types[0].factor_length();    

    using iitem         = build_item<int>;
    using iitem_handle  = iitem::handle_type;

    using ritem         = build_item<value>;
    using ritem_handle  = ritem::handle_type;

    using item          = build_item<value>;
    using item_pod      = sd::pod_type<item>;

    int size_counter    = 0;
    item_pod::destructor_type d(&size_counter);
    sd::stack_array<item_pod> buff(length, &d);    

    value add           = value::make_zero();
    item* it            = (item*)buff.get();

    for (size_t i = 0; i < length; ++i)
    {
        size_t add_pos  = get_add_pos(sub_types[0], i, 
                            ipow_arr, rpow_arr,exp_arr);

        // this is not a special node (special nodes are not collected)
        const value& v  = ih[add_pos].get_value();
        expr_handle eh  = ih[add_pos].get_expr_handle();

        if (eh->isa<mult_rep>() == false)
        {
            //this is an atom
            add             = add + v;

            // remove factored item
            ih[add_pos].get_expr_ref()  = nullptr;
            continue;
        };

        const mult_rep* mh  = eh->static_cast_to<mult_rep>();

        // make local copy of subexpressions 
        size_t isize        = mh->isize();
        size_t rsize        = mh->rsize();
        bool has_exp        = mh->has_exp();

        //destructor is trivial
        static const size_t buffer_size = 20;
        
        static_assert(sd::is_effective_pod<iitem_handle>::value == true, "pod required");
        sd::stack_array<sd::pod_type<iitem_handle>, buffer_size> ih_array(isize + 1);
        iitem_handle* imh = (iitem_handle*)ih_array.get();

        using ritem_pod     = sd::pod_type<ritem_handle>;

        static_assert(sd::is_effective_pod<ritem_handle>::value == true, "pod required");
        sd::stack_array<ritem_pod, buffer_size> rh_array(rsize + 1);
        ritem_handle* rmh   = (ritem_handle*)rh_array.get();

        for (size_t i2 = 0; i2 < isize; ++i2)
            new(imh + i2) iitem_handle(mh->IV(i2), mh->IE(i2));

        if (has_exp == true)
            new(imh + isize) iitem_handle(0, mh->Exp());

        for (size_t i2 = 0; i2 < rsize; ++i2)
            new(rmh + i2) ritem_handle(value_handle_type<value>::make_handle(mh->RV(i2)), mh->RE(i2));

        // remove factored subexpressions

        for (size_t k = 0; k < group_size; ++k)
        {
            size_t add_pos2 = get_add_pos(sub_types[k], i, 
                                ipow_arr, rpow_arr,exp_arr);
            assertion(add_pos2 == add_pos, "error in factor_subexpr");
            (void)add_pos2;

            size_t pos_mult = get_mult_pos(sub_types[k], i, 
                                ipow_arr, rpow_arr, exp_arr);

            reduce_mult(pos_mult, imh, rmh, has_exp, sub_types[k]);
        }

        //build factored mult
        expr mult_red   = build_reduced_mult(isize, rsize, imh, rmh, has_exp);

        if (mult_red.get_ptr()->isa<scalar_rep>() == true)
        {
            value val   = mult_red.get_ptr()->static_cast_to<scalar_rep>()->get_data();
            add         = add + val * v;
        }
        else
        {
            new (it + size_counter) item(v, std::move(mult_red));
            ++size_counter;
        };

        // remove factored item
        ih[add_pos].get_expr_ref()  = nullptr;
    };

    expr sum;
    value scal          = value::make_one();
    finalize_sum(it, size_counter, add, sum, scal);

    //disp(sum);

    // build final expr;

    size_t isize        = 0;
    size_t rsize        = 0;
    size_t n_exp        = 0;
    const mult_rep* mh  = nullptr;

    // get size parameters of final mult
    measure_final_mult(isize, rsize, n_exp, sum, scal, mh,
                       group_size, sub_types.data());

    // collect subterms

    expr exp_term;

    if (n_exp > 1)
        collect_exp(sum, group_size, sub_types.data(), exp_term);

    //destructor is trivial
    static const size_t buffer_size = 10;
        
    static_assert(sd::is_effective_pod<iitem_handle>::value == true, "pod required");
    sd::stack_array<sd::pod_type<iitem_handle>, buffer_size> ih_array(isize + 1);
    iitem_handle* imh = (iitem_handle*)ih_array.get();

    using ritem_pod     = sd::pod_type<ritem_handle>;

    static_assert(sd::is_effective_pod<ritem_handle>::value == true, "pod required");
    sd::stack_array<ritem_pod, buffer_size> rh_array(rsize + 1);
    ritem_handle* rmh   = (ritem_handle*)rh_array.get();

    size_t ipow_size    = 0;
    size_t rpow_size    = 0;

    collect_final_mult(isize, ipow_size, rpow_size, imh, rmh,
                       rpow_arr, sum, group_size, sub_types.data());

    assertion(ipow_size == isize, "error in factor_subexpr");
    assertion(rpow_size == rsize, "error in factor_subexpr");

    // cannonize mult expr; add exp terms only when there is only one
    res         = cannonize().make_mult(isize, rsize, n_exp == 1, imh, 
                                        rmh, scal, true);

    if (n_exp > 1)
    {
        res     = res * exp(exp_term);
        res     = cannonize().make(res, true);
    }

    ret_scal    = scal;
};

void subexpr_collector::measure_final_mult(size_t& isize, size_t& rsize, size_t& n_exp,
                                    const expr& sum, value& scal, const mult_rep*& mh,
                                    size_t group_size, const mult_subexpr* factors)
{
    mh      = nullptr;
    isize   = 0;
    rsize   = 0;
    n_exp   = 0;

    // process sum
    if (sum.get_ptr()->isa<scalar_rep>() == true)
    {
        scal            = scal * sum.get_ptr()->static_cast_to<scalar_rep>()->get_data();
    }
    else if (sum.get_ptr()->isa<mult_rep>() == false)
    {
        // add expression is not simple
        ++isize;
    }
    else
    {
        mh              = sum.get_ptr()->static_cast_to<mult_rep>();
        isize           += mh->isize();
        rsize           += mh->rsize();
        n_exp           += (mh->has_exp() ? 1 : 0);
    };

    // process factors
    for (size_t i = 0; i < group_size; ++i)
    {
        switch(factors[i].get_type())
        {
            case mult_subexpr_type::ipow:
            case mult_subexpr_type::ipow_horner:
            {
                ++isize;
                break;
            }
            case mult_subexpr_type::rpow:
            {
                ++rsize;
                break;
            }
            case mult_subexpr_type::exp:
            {
                ++n_exp;
                break;
            }

            case mult_subexpr_type::empty:
            default:
            {
                break;
            }
        }
    };
};

void subexpr_collector::collect_exp(const expr& sum, size_t group_size, 
                                    const mult_subexpr* factors, expr& exp_term)
{
    // only two exp terms are possible; one in the sum expr
    // and one from factorizations
    expr_handle exp1    = nullptr;
    expr_handle exp2    = nullptr;

    // process sum
    if (sum.get_ptr()->isa<mult_rep>() == true)
    {
        const mult_rep* mh  = sum.get_ptr()->static_cast_to<mult_rep>();
        if (mh->has_exp() == true)
            exp1        = mh->Exp();
    };

    // process factors
    for (size_t i = 0; i < group_size; ++i)
    {
        switch(factors[i].get_type())
        {
            case mult_subexpr_type::ipow:
            case mult_subexpr_type::ipow_horner:
            case mult_subexpr_type::rpow:
            {
                break;
            }
            case mult_subexpr_type::exp:
            {
                exp2    = factors[i].get_base();
                break;
            }

            case mult_subexpr_type::empty:
            default:
            {
                break;
            }
        }
    };

    assertion(exp1 != nullptr, "error in collect_exp");
    assertion(exp2 != nullptr, "error in collect_exp");

    exp_term  = expr(expr_ptr::from_this(exp1)) + expr(expr_ptr::from_this(exp2));
};

void subexpr_collector::collect_final_mult(size_t isize, size_t& ipow_pos, size_t& rpow_pos, 
                        build_item_handle<int>* imh, build_item_handle<value>* rmh,
                        rpow_item* rpow_arr, const expr& sum, size_t group_size, 
                        const mult_subexpr* factors)
{
    for (size_t i = 0; i < group_size; ++i)
    {
        add_factor_to_mult(isize, ipow_pos, rpow_pos, 
                           imh, rmh, rpow_arr, factors[i]);
    }

    // process sum
    if (sum.get_ptr()->isa<scalar_rep>() == true)
    {
        return;
    }
    else if (sum.get_ptr()->isa<mult_rep>() == false)
    {
        // add expression is not simple
        new (imh + ipow_pos) build_item_handle<int>(1, sum.get_ptr().get());
        ++ipow_pos;
    }
    else
    {
        const mult_rep* mh  = sum.get_ptr()->static_cast_to<mult_rep>();
        add_factor_to_mult(isize, ipow_pos, rpow_pos, 
                           imh, rmh, mh);
    };
};

void subexpr_collector::add_factor_to_mult(size_t isize, size_t& ipow_size, size_t& rpow_size, 
                    build_item_handle<int>* ih, build_item_handle<value>* rh, 
                    const rpow_item* rpow_arr, const mult_subexpr& factor)
{
    switch(factor.get_type())
    {
        case mult_subexpr_type::ipow:
        case mult_subexpr_type::ipow_horner:
        {
            int pow             = factor.get_int_power();
            expr_handle base    = factor.get_base();

            new(ih + ipow_size) build_item_handle<int>(pow, base);
            ++ipow_size;
            return;
        }
        case mult_subexpr_type::rpow:
        {
            size_t pos_arr      = factor.get_pos_in_array();
            const value& pow    = rpow_arr[pos_arr].m_pow;
            expr_handle base    = factor.get_base();

            new(rh + rpow_size) build_item_handle<value>(m_temp_vals->make_handle(pow), base);
            ++rpow_size;
            return;
        }
        case mult_subexpr_type::exp:
        {
            expr_handle base    = factor.get_base();

            new(ih + isize) build_item_handle<int>(0, base);
            return;
        }

        case mult_subexpr_type::empty:
        default:
        {
            break;
        }
    }
};

void subexpr_collector::add_factor_to_mult(size_t isize, size_t& ipow_size, size_t& rpow_size, 
                    build_item_handle<int>* ih, build_item_handle<value>* rh, 
                    const mult_rep* mh)
{
    size_t h_isize  = mh->isize();
    size_t h_rsize  = mh->rsize();

    for (size_t i = 0; i < h_isize; ++i)
    {
        new (ih + ipow_size) build_item_handle<int>(mh->IV(i), mh->IE(i));
        ++ipow_size;
    };

    for (size_t i = 0; i < h_rsize; ++i)
    {
        new (rh + rpow_size) build_item_handle<value>(value_handle_type<value>::make_handle(mh->RV(i)), 
                                                    mh->RE(i));
        ++rpow_size;
    };

    if (mh->has_exp())
        new (ih + isize) build_item_handle<int>(0, mh->Exp());
};

size_t subexpr_collector::get_add_pos(const mult_subexpr& factor, size_t pos_arr_off,
                    ipow_item* ipow_arr, rpow_item* rpow_arr, exp_item* exp_arr)
{
    mult_subexpr_type type  = factor.get_type();
    size_t pos_arr          = factor.get_pos_in_array();

    switch(type)
    {
        case mult_subexpr_type::ipow:
            return ipow_arr[pos_arr + pos_arr_off].m_add_pos;
        case mult_subexpr_type::ipow_horner:
            return ipow_arr[pos_arr + pos_arr_off].m_add_pos;
        case mult_subexpr_type::rpow:
            return rpow_arr[pos_arr + pos_arr_off].m_add_pos;
        case mult_subexpr_type::exp:
            return exp_arr[pos_arr + pos_arr_off].m_add_pos;

        case mult_subexpr_type::empty:
        default:
        {
            //not possible
            assertion(0, "invalid case");
            throw;
        }
    };
};

size_t subexpr_collector::get_mult_pos(const mult_subexpr& factor, size_t pos_arr_off,
                    ipow_item* ipow_arr, rpow_item* rpow_arr, exp_item* exp_arr)
{
    mult_subexpr_type type  = factor.get_type();
    size_t pos_arr          = factor.get_pos_in_array();
    (void)exp_arr;

    switch(type)
    {
        case mult_subexpr_type::ipow:
            return ipow_arr[pos_arr + pos_arr_off].m_mult_pos;
        case mult_subexpr_type::ipow_horner:
            return ipow_arr[pos_arr + pos_arr_off].m_mult_pos;
        case mult_subexpr_type::rpow:
            return rpow_arr[pos_arr + pos_arr_off].m_mult_pos;
        case mult_subexpr_type::exp:
            return 0;

        case mult_subexpr_type::empty:
        default:
        {
            //not possible
            assertion(0, "invalid case");
            throw;
        }
    };
};

void subexpr_collector::finalize_sum(build_item<value>* it, size_t size, const value& add0,
                    expr& res, value& scal)
{
    value add  = add0;

    if (size == 0)
    {
        // this is just add;
        res     = scalar_rep::make_one();
        scal    = add;
        return;
    }

    if (size == 1)
    {
        if (it[0].get_expr_handle()->isa<add_rep>() == true)
        {
            // this is just add + v * [add_rep];
            const add_rep* ah   = it[0].get_expr_handle()->static_cast_to<add_rep>();
            const value& v      = it[0].get_value();

            add             = add/v;
            value norm_scal = add_rep::add_scalar_normalize(ah, add, res);
            scal            = norm_scal * v;
            return;
        }
        else
        {
            scal                = value::make_one();

            using item_type     = build_item<value>;
            add_rep_info<item_type> ai(&add, 1, it, nullptr);

            expr_ptr res_ptr    = add_rep::make(ai);
            res                 = expr(res_ptr);
            return;
        };
    };

    res                 = cannonize().make_add(add, size, it, true);

    if (res.get_ptr()->isa<scalar_rep>() == true)
    {
        value v         = res.get_ptr()->static_cast_to<scalar_rep>()->get_data();
        res             = scalar_rep::make_one();
        scal            = v;
    }
    else if (res.get_ptr()->isa<add_rep>() == true)
    {
        const add_rep* ah   = res.get_ptr()->static_cast_to<add_rep>();
        
        if (cannonize::is_simple_add(ah) == true)
        {
            expr_handle tmp_ex  = ah->E(0);
            scal                = ah->V(0);
            res                 = expr(expr_ptr::from_this(tmp_ex));
        }
        else
        {
            scal        = value::make_one();
        }
    }
    else
    {
        scal            = value::make_one();
    }

    return;
};

void subexpr_collector::reduce_mult(size_t pos_mult, build_item_handle<int>* ih, 
                build_item_handle<value>* rh, bool& has_exp, const mult_subexpr& factor)
{
    mult_subexpr_type type  = factor.get_type();

    switch(type)
    {
        case mult_subexpr_type::ipow:
        {
            assertion(ih[pos_mult].get_expr_handle() == factor.get_base(), "reduce_mult");
            assertion(ih[pos_mult].get_value() == factor.get_int_power(), "reduce_mult");

            ih[pos_mult].get_expr_ref() = nullptr;
            return;
        }
        case mult_subexpr_type::rpow:
        {
            assertion(rh[pos_mult].get_expr_handle() == factor.get_base(), "reduce_mult");

            rh[pos_mult].get_expr_ref() = nullptr;
            return;
        }
        case mult_subexpr_type::exp:
        {
            has_exp = false;
            return;
        }
        case mult_subexpr_type::ipow_horner:
        {
            int pow = factor.get_int_power();
            ih[pos_mult].get_value_handle_ref()    -= pow;

            assertion(ih[pos_mult].get_expr_handle() == factor.get_base(), "reduce_mult");
            assertion(ih[pos_mult].get_value() >= 0, "reduce_mult");

            if (ih[pos_mult].get_value() == 0)
                ih[pos_mult].get_expr_ref() = nullptr;

            return;
        }
    };
};

expr subexpr_collector::build_reduced_mult(size_t isize, size_t rsize, build_item_handle<int>* ih, 
                    build_item_handle<value>* rh, bool has_exp)
{
    using iitem         = build_item<int>;
    using ritem         = build_item<value>;
    using iitem_handle  = iitem::handle_type;
    using ritem_handle  = ritem::handle_type;    

    iitem_handle* exp   = has_exp ? ih + isize : nullptr;

    // remove factored items
    size_t last     = isize;
    isize           = 0;
    for (size_t i = 0; i < last; ++i)
    {        
        if (ih[i].get_expr_handle() != nullptr)
        {
            ih[isize]   = ih[i];
            ++isize;
        }
    }

    last        = rsize;
    rsize       = 0;

    for (size_t i = 0; i < last; ++i)
    {
        if (rh[i].get_expr_handle() != nullptr)
        {
            rh[rsize]   = rh[i];
            ++rsize;
        }
    }

    if (rsize == 0 && has_exp == false)
    {
        if (isize == 0)
            return scalar_rep::make_one();

        if (isize == 1 && ih[0].get_value() == 1)
            return expr(expr_ptr::from_this(ih[0].get_expr_handle()));
    }

    using mult_info     = mult_rep_info<iitem_handle,ritem_handle>;

    // after removal ordering is not changed
    mult_info info(isize, ih, exp, rsize, rh);

    expr_ptr ep = mult_rep::make(info);
    return expr(ep);
};

void subexpr_collector::collect_factorizations(sub_type_vector& sub_types,
                ipow_item* ipow_arr, rpow_item* rpow_arr, exp_item* exp_arr)
{
    size_t size     = sub_types.size();
    size_t first    = 0;

    assertion(size > 1, "error in collect_factorizations");

    for (; first + 1 < size;)
    {
        size_t pos      = first + 1;    
        size_t last_pos = size - 1;

        for (; pos <= last_pos;)
        {
            bool can_link   = can_link_factorizations(sub_types[first], 
                                sub_types[pos], ipow_arr, rpow_arr, exp_arr);

            if (can_link == true)
            {
                ++pos;
                sub_types[first].increase_group_size();
                continue;
            }

            std::swap(sub_types[pos], sub_types[last_pos]);
            --last_pos;
        };

        first           = pos + 1;
    };    
};

bool subexpr_collector::can_link_factorizations(const mult_subexpr& f1, 
                    const mult_subexpr& f2, ipow_item* ipow_arr, rpow_item* rpow_arr,
                    exp_item* exp_arr)
{
    // two factorizations can be merged, if they have the same length
    // and appears in the same subexpression of add_rep

    if (f1.factor_length() != f2.factor_length())
        return false;
    
    dbs pos1    = get_factor_positions(f1, ipow_arr, rpow_arr, exp_arr);
    dbs pos2    = get_factor_positions(f2, ipow_arr, rpow_arr, exp_arr);

    return pos1 == pos2;
};

subexpr_collector::dbs subexpr_collector::get_factor_positions(const mult_subexpr& f1,
                        ipow_item* ipow_arr, rpow_item* rpow_arr, exp_item* exp_arr)
{
    size_t length   = f1.factor_length();
    size_t pos_arr  = f1.get_pos_in_array();

    mult_subexpr_type type  = f1.get_type();

    switch(type)
    {
        case mult_subexpr_type::ipow:
        case mult_subexpr_type::ipow_horner:
        {
            size_t add_pos  = ipow_arr[pos_arr].m_add_pos;
            dbs ret(add_pos);

            for (size_t i = 1; i < length; ++i)
            {
                size_t pos  = ipow_arr[pos_arr + i].m_add_pos;
                ret         = ret.set(pos);
            }

            return ret;
        }
        case mult_subexpr_type::rpow:
        {
            size_t add_pos  = rpow_arr[pos_arr].m_add_pos;
            dbs ret(add_pos);

            for (size_t i = 1; i < length; ++i)
            {
                size_t pos  = rpow_arr[pos_arr + i].m_add_pos;
                ret         = ret.set(pos);
            }

            return ret;
        }
        case mult_subexpr_type::exp:
        {
            size_t add_pos  = exp_arr[pos_arr].m_add_pos;
            dbs ret(add_pos);

            for (size_t i = 1; i < length; ++i)
            {
                size_t pos  = exp_arr[pos_arr + i].m_add_pos;
                ret         = ret.set(pos);
            }

            return ret;
        }
        case mult_subexpr_type::empty:
        default:
        {
            return dbs();
        }
    };
};

size_t subexpr_collector::select_longest_group(sub_type_vector& sub_types)
{
    size_t size             = sub_types.size();
    size_t n_best           = 1;    
    size_t best_gr_size     = sub_types[0].get_group_size();
    size_t next_best_pos    = best_gr_size;
    size_t pos              = best_gr_size;

    for (; pos < size;)
    {
        size_t gr_size  = sub_types[pos].get_group_size();

        if (gr_size > best_gr_size)
        {
            n_best          = 1;
            best_gr_size    = gr_size;
            next_best_pos   = gr_size;

            for (size_t i = 0; i < gr_size; ++i)
                sub_types[i] = sub_types[pos + i];
        }
        else if (gr_size == best_gr_size)
        {
            ++n_best;

            for (size_t i = 0; i < gr_size; ++i)
                sub_types[next_best_pos + i] = sub_types[pos + i];

            next_best_pos   += gr_size;
        };

        pos += gr_size;
    };

    return n_best;
};

void subexpr_collector::select_ambiguous_group(sub_type_vector& sub_types)
{   
    using stats_type        = factor_group_stats;

    size_t size             = sub_types.size();
    size_t best_gr_size     = sub_types[0].get_group_size();
    size_t next_best_pos    = best_gr_size;
    size_t pos              = best_gr_size;
    
    stats_type best_stats   = measure_group(sub_types.data(), best_gr_size);

    for (; pos < size;)
    {
        size_t gr_size      = sub_types[pos].get_group_size();
        stats_type stats    = measure_group(sub_types.data() + pos, gr_size);

        if (best_stats < stats)
        {
            next_best_pos   = gr_size;
            best_stats      = stats;            

            for (size_t i = 0; i < gr_size; ++i)
                sub_types[i] = sub_types[pos + i];
        }

        pos += gr_size;
    };
};

factor_group_stats subexpr_collector::measure_group(const mult_subexpr* ptr, size_t size)
{
    assertion(size > 0, "error in measure_group");

    expr_handle eh      = ptr[0].get_base();
    expr_handle eh_best = eh;
    size_t n_symbols    = details::get_number_symbols(eh);    
    dbs syms;

    details::add_symbols(eh, syms);

    for (size_t i = 1; i < size; ++i)
    {
        expr_handle el  = ptr[i].get_base();
        n_symbols       += details::get_number_symbols(el);

        details::add_symbols(el, syms);

        if (el < eh_best)
            eh_best     = el;
    };

    return factor_group_stats(n_symbols, syms, eh_best);
}

void subexpr_collector::select_subexpr(sub_type_vector& sub_types,
                        ipow_item* ipow_arr, rpow_item* rpow_arr, 
                        exp_item* exp_arr, size_t& group_size)
{
    // find equivalent substitutions better than any other substitutions

    size_t size = sub_types.size();

    if (size == 1)
    {
        group_size  = 1;
        return;
    };

    // try to collect factarizations
    collect_factorizations(sub_types, ipow_arr, rpow_arr, exp_arr);

    if (sub_types[0].get_group_size() == sub_types.size())
    {
        // all factorizations are merged

        group_size = sub_types[0].get_group_size();
        return;
    };

    // select longest group
    size_t n_best       = select_longest_group(sub_types);

    if (n_best == 1)
    {
        group_size = sub_types[0].get_group_size();
        return;
    };

    // select one of groups
    select_ambiguous_group(sub_types);

    group_size = sub_types[0].get_group_size();
    return;
};

void subexpr_collector::sort_subexpr(sub_type_vector& sub_types, ipow_item* ipow_arr, 
                    rpow_item* rpow_arr, exp_item* exp_arr, size_t group_size)
{
    for (size_t i = 0; i < group_size; ++i)
    {
        const mult_subexpr& fact    = sub_types[i];
        mult_subexpr_type type      = fact.get_type();
        size_t start_pos            = fact.get_pos_in_array();
        size_t length               = fact.factor_length();

        switch(type)
        {
            case mult_subexpr_type::ipow:
            case mult_subexpr_type::ipow_horner:
            {
                sym_arrow::utils::sort_q(ipow_arr + start_pos, length, add_pos_comparer()); 
                break;
            }
            case mult_subexpr_type::rpow:
            {
                sym_arrow::utils::sort_q(rpow_arr + start_pos, length, add_pos_comparer()); 
                break;
            }
            case mult_subexpr_type::exp:
            {
                sym_arrow::utils::sort_q(exp_arr + start_pos, length, add_pos_comparer()); 
                break;
            }
        };
    };
};

template<class Array_item>
size_t subexpr_collector::check_can_deep_flag(const Array_item* arr, size_t arr_size)
{
    size_t ret = 0;

    for (size_t i = 0; i < arr_size; ++i)
    {
        if (arr[i].m_can_deep_simpl == true)
            ++ret;
    }

    return ret;
};

template<class Array_item>
size_t subexpr_collector::find_next_base(const Array_item* arr, size_t arr_size, size_t pos)
{
    expr_handle base    = arr[pos].m_expr;

    ++pos;
    while (pos < arr_size)
    {
        expr_handle loc = arr[pos].m_expr;

        if (loc != base)
            return pos;

        ++pos;
    };

    return pos;
};

void subexpr_collector::select_subexpr_ipow(const ipow_item* arr, size_t arr_size, 
                                        factorizations& fact, bool& has_eq)
{
    size_t pos              = 0;

    while (pos < arr_size)
    {
        size_t start        = pos;
        size_t end          = find_next_base(arr, arr_size, pos);
        pos                 = end;
        size_t n_elems      = end - start;

        if (n_elems < 2)
            continue;

        expr_handle base    = arr[start].m_expr;
        size_t num_neg      = number_negative_int(arr + start, n_elems);

        // negative powers are collected separately to positive powers

        if (num_neg > 1)
        {
            size_t length_equal = 0;
            int pow_equal       = 0;            
            size_t n_deep       = 0;
            bool pref_smaller   = true;

            size_t pos_arr      = longest_equal_sequence_int(arr + start, num_neg, 
                                    pref_smaller, length_equal, pow_equal, n_deep);
            pos_arr             = pos_arr + start;

            if (has_eq == false && can_horner(base) == true)
            {
                size_t n_deep_tot   = check_can_deep_flag(arr + start, num_neg);
                int pow_neg         = arr[start].m_pow;
            
                mult_subexpr it_neg = mult_subexpr::make_ipow_horner
                                        (start, base, num_neg, pow_neg, n_deep_tot);
                fact.add(it_neg);
            };

            if (length_equal > 1)
            {
                mult_subexpr it = mult_subexpr::make_ipow_equal(pos_arr, base, num_neg, 
                                                length_equal, pow_equal, n_deep);

                fact.add(it);
                has_eq          = true;
            };
        };

        start       += num_neg;
        n_elems     -= num_neg;

        if (n_elems <= 1)
            continue;

        // find longest sequence of equal powers; higher powers are preferred
        size_t max_length   = 0;
        int max_pow_equal   = 0;
        size_t n_deep       = 0;
        bool pref_smaller   = false;                

        size_t pos_arr      = longest_equal_sequence_int(arr + start, n_elems, 
                                pref_smaller, max_length, max_pow_equal, n_deep);
        pos_arr             = pos_arr + start;

        if (has_eq == false && can_horner(base) == true)
        {
            int max_abs_pow     = arr[start].m_pow;
            size_t n_deep_tot   = check_can_deep_flag(arr + start, n_elems);
            mult_subexpr it_pos = mult_subexpr::make_ipow_horner
                                    (start, base, n_elems, max_abs_pow, n_deep_tot);

            fact.add(it_pos);
        };

        if (max_length >= 2)
        {
            mult_subexpr it_equal = mult_subexpr::make_ipow_equal(pos_arr, base, n_elems, 
                                            max_length, max_pow_equal, n_deep);
            fact.add(it_equal);
            has_eq              = true;
        };
    };
};

void subexpr_collector::select_subexpr_epow(const exp_item* arr, size_t arr_size,
                                            factorizations& fact)
{
    static const size_t buffer_size = 20;

    size_t pos              = 0;

    while (pos < arr_size)
    {
        size_t start        = pos;
        size_t end          = find_next_base(arr, arr_size, pos);
        pos                 = end;
        size_t n_elems      = end - start;

        if (n_elems < 2)
            continue;

        expr_handle base    = arr[start].m_expr;
        size_t n_deep       = check_can_deep_flag(arr + start, n_elems);
        mult_subexpr it     = mult_subexpr::make_exp(start, base, n_elems, n_deep);
        fact.add(it);
    };
};

void subexpr_collector::select_subexpr_rpow(const rpow_item* arr, size_t arr_size,
                                            factorizations& fact)
{
    static const size_t buffer_size = 20;

    size_t pos              = 0;

    while (pos < arr_size)
    {
        size_t start        = pos;
        size_t end          = find_next_base(arr, arr_size, pos);
        pos                 = end;
        size_t n_elems      = end - start;

        if (n_elems < 2)
            continue;

        expr_handle base    = arr[start].m_expr;
        size_t max_length   = 0;
        size_t n_deep       = 0;
        bool pref_smaller   = false;
        size_t pos_arr      = longest_equal_sequence_real(arr + start, n_elems, 
                                   pref_smaller, max_length, n_deep);
        pos_arr             = pos_arr + start;

        if (max_length >= 2)
        {
            //we do not simplify expressions containts different powers
            mult_subexpr it_equal = mult_subexpr::make_rpow_equal
                                        (pos_arr, base, max_length, n_deep);

            fact.add(it_equal);
        };
    };
};

size_t subexpr_collector::number_negative_int(const ipow_item* arr, size_t n_elems)
{
    size_t n_neg   = 0;
    
    for (size_t i = 0; i < n_elems; ++i)
    {
        if (arr[i].m_pow < 0)
            ++n_neg;
        else
            break;
    };

    return n_neg;
};

struct seq_item
{
    size_t  length;
    size_t  n_deep;
    size_t  pos_start;

    seq_item(size_t start, size_t len, size_t deep)
        :length(len), n_deep(deep), pos_start(start)
    {};

    void add(bool can_deep)
    {
        length  += 1;
        n_deep  += can_deep ? 1 : 0;
    }

    bool is_better(const seq_item& other, bool sel_eq) const
    {
        if (n_deep > other.n_deep)
            return true;
        if (n_deep < other.n_deep)
            return false;

        if (length > other.length)
            return true;
        if (length < other.length)
            return false;

        if (sel_eq == true)
            return true;
        else
            return false;
    };
};

size_t subexpr_collector::longest_equal_sequence_int(const ipow_item* arr, size_t n_elems, 
                            bool pref_smaller, size_t& length, int& max_pow, size_t& n_deep)
{
    if (n_elems == 0)
    {
        length      = 0;
        return 0;
    }

    size_t pos          = 0;

    int cur_pow         = arr[pos].m_pow;
    bool sel_eq         = pref_smaller ? false : true;
    seq_item cur_item   = seq_item(0, 0, 0);

    max_pow             = arr[pos].m_pow;
    seq_item best       = seq_item(0, 0, 0);

    for(; pos < n_elems; ++pos)
    {
        int pow         = arr[pos].m_pow;

        if (pow == cur_pow)
        {
            cur_item.add(arr[pos].m_can_deep_simpl);
            continue;
        };

        if (cur_item.is_better(best, sel_eq) == true)
        {
            best        = cur_item;
            max_pow     = cur_pow;  
        }

        cur_pow     = pow;
        cur_item    = seq_item(pos, 1, 0);
    };

    if (cur_item.is_better(best, sel_eq) == true)
    {
        best        = cur_item;
        max_pow     = cur_pow;  
    }

    length  = best.length;
    n_deep  = best.n_deep;

    return best.pos_start;
};

size_t subexpr_collector::longest_equal_sequence_real(const rpow_item* arr, size_t n_elems, 
                    bool pref_small_pow, size_t& length, size_t& n_deep)
{
    if (n_elems == 0)
    {
        length          = 0;
        return 0;
    }

    using item_type     = const value*;
    size_t pos          = 0;
    
    item_type cur_pow   = &arr[pos].m_pow;
    seq_item cur_item   = seq_item(0, 0, 0);
    bool sel_eq         = pref_small_pow ? false : true;

    seq_item best       = seq_item(0, 0, 0);

    for(; pos < n_elems; ++pos)
    {
        item_type pow   = &arr[pos].m_pow;

        if (*pow == *cur_pow)
        {
            cur_item.add(arr[pos].m_can_deep_simpl);
            continue;
        };

        if (cur_item.is_better(best, sel_eq) == true)
            best        = cur_item;

        cur_pow     = pow;
        cur_item    = seq_item(pos, 1, 0);
    };

    if (cur_item.is_better(best, sel_eq) == true)
        best        = cur_item;

    length  = best.length;
    n_deep  = best.n_deep;

    return best.pos_start;
};

void subexpr_collector::calc_size(const add_item_handle& it, subs_counter& count)
{
    //ignore log item
    if (it.is_special() == true)
        return;

    expr_handle eh      = it.get_expr_handle();

    //add item can be a mul_rep or an atom
    if (eh->isa<mult_rep>() == false)
    {
        //this is an atom; it is artificially promoted to x^1
        count.add_ipow(1);
        return;
    };

    const mult_rep* mr  = eh->static_cast_to<mult_rep>();

    size_t in       = mr->isize();
    size_t rn       = mr->rsize();
    bool has_exp    = mr->has_exp();

    count.add_ipow(in);
    count.add_rpow(rn);

    if (has_exp)
        count.add_exp();

    return;
};

void subexpr_collector::collect_mult_ipow(const add_item_handle& it, size_t pos,
                                          ipow_item* arr, size_t& arr_pos)
{
    //ignore log item
    if (it.is_special() == true)
        return;

    expr_handle eh      = it.get_expr_handle();

    //add item can be a mul_rep or an atom
    if (eh->isa<mult_rep>() == false)
    {
        //this is an atom; it is artificially promoted to x^1
        new (arr + arr_pos) ipow_item(pos, 1, eh);
        ++arr_pos;
        return;
    };

    const mult_rep* mr  = eh->static_cast_to<mult_rep>();
    size_t in           = mr->isize();
    size_t rn           = mr->rsize();
    bool has_exp        = mr->has_exp();

    // can_deep_simpl is true, when after factoring one subexpr out,
    // resulting expression is not a mult_rep
    // when this expr is sub^k, then deep simplicifaction is not possible
    bool can_deep_simpl = rn == 0 && has_exp == false && in == 2;

    if (can_deep_simpl == true)
    {
        int it_pow_0        = mr->IV(0);
        int it_pow_1        = mr->IV(1);
        expr_handle it_e_0  = mr->IE(0);
        expr_handle it_e_1  = mr->IE(1);

        bool can_deep_0     = false;
        bool can_deep_1     = false;

        if (it_pow_1 == 1)
            can_deep_0      = true;

        new (arr + arr_pos) ipow_item(pos, 0, it_pow_0, it_e_0, can_deep_0);
        ++arr_pos;

        if (it_pow_0 == 1)
            can_deep_1      = true;

        new (arr + arr_pos) ipow_item(pos, 1, it_pow_1, it_e_1, can_deep_1);
        ++arr_pos;
    }
    else
    {
        for (size_t i = 0; i < in; ++i)
        {
            int it_pow              = mr->IV(i);
            expr_handle it_e        = mr->IE(i);

            new (arr + arr_pos) ipow_item(pos, i, it_pow, it_e, false);
            ++arr_pos;
        };
    };
};

void subexpr_collector::collect_mult_rpow(const add_item_handle& it, size_t pos,
                                          rpow_item* arr, size_t& arr_pos)
{
    //ignore log item
    if (it.is_special() == true)
        return;

    expr_handle eh      = it.get_expr_handle();

    if (eh->isa<mult_rep>() == false)
        return;

    const mult_rep* mr  = eh->static_cast_to<mult_rep>();
    size_t in           = mr->isize();
    size_t rn           = mr->rsize();
    bool has_exp        = mr->has_exp();

    // can_deep_simpl is true, when after factoring one subexpr out,
    // resulting expression is not a mult_rep
    bool can_deep_simpl = rn == 1 && has_exp == false && in == 1;

    if (can_deep_simpl == true)
    {
        //then in = 1;
        int pow         = mr->IV(0);

        if (pow != 1)
            can_deep_simpl = false;
    };

    for (size_t i = 0; i < rn; ++i)
    {
        const value& it_pow     = mr->RV(i);
        expr_handle it_e        = mr->RE(i);

        new (arr + arr_pos) rpow_item(pos, (int)i, it_pow, it_e, can_deep_simpl);
        ++arr_pos;
    };
};

void subexpr_collector::collect_mult_epow(const add_item_handle& it, size_t pos,
                                          exp_item* arr, size_t& arr_pos)
{
    //ignore log item
    if (it.is_special() == true)
        return;

    expr_handle eh      = it.get_expr_handle();

    if (eh->isa<mult_rep>() == false)
        return;

    const mult_rep* mr  = eh->static_cast_to<mult_rep>();
    bool has_exp        = mr->has_exp();

    if (has_exp == false)
        return;

    size_t in           = mr->isize();
    size_t rn           = mr->rsize();    

    // can_deep_simpl is true, when after factoring one subexpr out,
    // resulting expression is not a mult_rep
    bool can_deep_simpl = rn == 0 && in == 1;

    if (can_deep_simpl == true)
    {
        //then in = 1;
        int pow         = mr->IV(0);

        if (pow != 1)
            can_deep_simpl = false;
    };

    expr_handle it_e    = mr->Exp();

    new (arr + arr_pos) exp_item(pos, it_e, can_deep_simpl);
    ++arr_pos;

    return;
};

bool subexpr_collector::can_horner(expr_handle h)
{
    // only symbols can be a base of horner factorization;
    // other bases can be disastrous for differentiation

    // currently horner is disabled; cost is too high
    (void)h;
    return false;
};

};};
