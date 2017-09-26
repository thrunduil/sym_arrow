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

#include "sym_arrow/ast/builder/mult_build.inl"
#include "sym_arrow/ast/ast.h"
#include "sym_arrow/ast/cannonization/cannonize.h"

namespace sym_arrow { namespace ast
{

mult_build::mult_build(const mult_build_info_real& bi)
    : base_type(this), m_ilist(nullptr), m_rlist(nullptr)
    , m_scal(value::make_one())
{
    make_rlist();

    insert_relem(bi.get_rpow(), bi.get_rexpr());
};

mult_build::mult_build(const mult_exp_build_info& bi)
    : base_type(this), m_ilist(nullptr), m_rlist(nullptr)
    , m_scal(value::make_one())
{
    using context_type  = sym_dag::dag_context<term_tag>;
    context_type& c     = context_type::get();

    m_rlist = (rlist_type*)c.malloc(sizeof(rlist_type));
    new(m_rlist) rlist_type(value::make_one());

    insert_elem_exp(bi.get_exp_handle());
};

mult_build::~mult_build()
{
    if (m_ilist)
        m_ilist->close();

    if (m_rlist)
        m_rlist->close();
};

void mult_build::release(stack_type& stack)
{
    vlist_stack vl_stack(stack);

    using context_type  = sym_dag::dag_context<term_tag>;
    context_type& c     = context_type::get();

    if (m_ilist)
    {
        m_ilist->close(vl_stack);
        m_ilist->~ilist_type();
        c.free(m_ilist, sizeof(ilist_type));
        m_ilist = nullptr;
    };

    if (m_rlist)
    {
        m_rlist->close(vl_stack);
        m_rlist->~rlist_type();
        c.free(m_rlist, sizeof(rlist_type));
        m_rlist = nullptr;
    };

    vl_stack.release(m_cannonized);
};

void mult_build::update(expr& ret, const value& a, int pow_me)
{
    set_modified();

    if (pow_me != 1)
        make_pow(pow_me);

    make_scal(a);

    ret = expr(this);
};

void mult_build::update(expr& ret, const value& a, int pow_me, int b2, expr_handle s2)
{
    set_modified();

    if (pow_me != 1)
        make_pow(pow_me);

    make_scal(a);
    make_ilist();
    insert_ielem(b2,s2);

    ret = expr(this);
};

void mult_build::make_pow(expr& ret, const value& p)
{
    set_modified();

    using context_type  = sym_dag::dag_context<term_tag>;
    context_type& c     = context_type::get();

    if (m_rlist)
        m_rlist->make_pow(p);

    //make scalar
    m_scal  = power_real(m_scal, p);

    if (m_ilist != nullptr)
    {
        make_rlist();

        // m_rlist is initialized in make_rlist
        m_ilist->make_pow(p, *m_rlist);

        vlist_stack stack;

        m_ilist->close(stack);
        m_ilist->~ilist_type();
        c.free(m_ilist, sizeof(ilist_type));
        m_ilist = nullptr;
    };    

    ret = expr(this);
};

void mult_build::insert_ielem(int vpow, expr_handle expr)
{
    if (vpow == 0)
        return;

    if (expr->isa<scalar_rep>() == true)
    {
        make_scal(power_int(expr->static_cast_to<scalar_rep>()->get_data(),vpow));
        return;
    }
    else if(expr->isa<add_build>() == true)
    {
        const add_build* ab = expr->static_cast_to<add_build>();
     
        if (ab->is_simple_add() == true)
        {
            expr_handle tmp_e   = ab->unique_elem().get_expr_handle();
            value tmp_v         = ab->unique_elem().get_value();
            
            make_scal(power_int(tmp_v, vpow));
            insert_ielem(vpow, tmp_e);
            return;
        };

        value scal = value::make_one();
        sym_arrow::expr tmp = cannonize().make_normalize(ab, scal, do_cse_default);

        expr_handle be(tmp.get_ptr().get());

        if (scal.is_one() == false)
        {
            this->make_scal(power_int(scal,vpow));

            ipush_back(vpow, be);
            return;
        };

        return insert_ielem(vpow,be);
    }
    else if (expr->isa<add_rep>() == true)
    {
        value scal = value::make_one();
        expr_ptr tmp = cannonize().normalize(expr->static_cast_to<add_rep>(), scal);

        if (scal.is_one() == false)
            make_scal(power_int(scal,vpow));

        ipush_back(vpow, tmp.get());
        return;
    };

    ipush_back(vpow, expr);
};

void mult_build::insert_relem(const value& pow, expr_handle expr)
{
    if (pow.is_zero() == true)
        return;

    if (expr->isa<scalar_rep>() == true)
    {
        value v = abs(expr->static_cast_to<scalar_rep>()->get_data());
        make_scal(power_real(v, pow));
        return;
    }
    else if(expr->isa<add_build>() == true)
    {
        const add_build* ab = expr->static_cast_to<add_build>();

        if (ab->is_simple_add() == true)
        {
            expr_handle tmp_e   = ab->unique_elem().get_expr_handle();
            value tmp_v         = ab->unique_elem().get_value();

            make_scal(power_real(tmp_v, pow));
            insert_relem(pow, tmp_e);
            return;
        };

        value scal          = value::make_one();
        sym_arrow::expr tmp  = cannonize().make_normalize(ab, scal, do_cse_default);

        expr_handle be(tmp.get_ptr().get());

        if (scal.is_one() == false)
        {
            make_scal(power_real(scal, pow));
            rpush_back(pow, be);
            return;
        };
        
        return insert_relem(pow, be);
    }
    else if (expr->isa<add_rep>() == true)
    {
        value scal   = value::make_one();
        expr_ptr tmp = cannonize().normalize(expr->static_cast_to<add_rep>(), scal);

        if (scal.is_one() == false)
            make_scal(power_real(scal, pow));

        rpush_back(pow, tmp.get());
        return;
    };

    rpush_back(pow, expr);
};

void mult_build::make_ilist()
{
    if (!m_ilist)
    {
        using context_type  = sym_dag::dag_context<term_tag>;
        context_type& c     = context_type::get();

        m_ilist = (ilist_type*)c.malloc(sizeof(ilist_type));
        new(m_ilist) ilist_type(1);
    };
};

void mult_build::make_rlist()
{
    if (!m_rlist)
    {
        using context_type  = sym_dag::dag_context<term_tag>;
        context_type& c     = context_type::get();

        m_rlist = (rlist_type*)c.malloc(sizeof(rlist_type));
        new(m_rlist) rlist_type(value::make_one());
    };
};

void mult_build::make_pow(int val)
{
    m_scal = power_int(m_scal,val);

    if (m_ilist)
        m_ilist->make_pow(val);

    if (m_rlist)
        m_rlist->make_pow(val);
};

void mult_build::insert_elem_exp(expr_handle expr)
{
    size_t code = expr->get_code();

    switch(code)
    {
        case (size_t)term_types::scalar:
        {
            make_scal(exp(expr->static_cast_to<scalar_rep>()->get_data()));
            return;
        }
        case (size_t)term_types::mult_rep:
        {
            push_back_exp(expr);
            return;
        }
        case (size_t)term_types::add_rep:
        {
            const add_rep* ah  = expr->static_cast_to<add_rep>();

            if (ah->has_log() == false)
            {
                //add term will be processed later
                push_back_exp(expr);
                return;
            }

            expr_handle log_h(ah->Log());
            insert_relem(value::make_one(), log_h);

            const value& add = ah->V0();
            make_scal(exp(add));

            sym_arrow::expr res;
            add_rep::remove_add_log(ah, res);

            insert_elem_exp(res.get_ptr().get());
            return;
        }
        // we can pertially avoid cannonization of mult and add build
        // but this will not lead to important improvement
        case (size_t)term_types::mult_build:
        {
            const mult_build* mb  = expr->static_cast_to<mult_build>();
            sym_arrow::expr tmp = cannonize().make_mult(mb, do_cse_default);
        
            expr_handle be(tmp.get_ptr().get());
            return insert_elem_exp(be);
        }
        case (size_t)term_types::add_build:
        {
            const add_build* ab  = expr->static_cast_to<add_build>();
            sym_arrow::expr tmp = cannonize().make_add(ab, do_cse_default);
        
            expr_handle be(tmp.get_ptr().get());
            return insert_elem_exp(be);
        }
        default:
        {
            push_back_exp(expr);
        }
    };    
};

void mult_build::ipush_back(int val, expr_handle ex)
{
    m_ilist->push_back(val, ex);
};

void mult_build::rpush_back(const value& pow, expr_handle ex)
{
    m_rlist->push_back(pow, ex);
};

void mult_build::push_back_exp(expr_handle ex)
{
    m_rlist->push_back_special(value::make_one(), ex);
};

};};
