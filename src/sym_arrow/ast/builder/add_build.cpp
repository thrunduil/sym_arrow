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

#include "sym_arrow/ast/builder/add_build.inl"
#include "sym_arrow/ast/ast.h"
#include "sym_arrow/nodes/expr.h"
#include "sym_arrow/ast/cannonization/cannonize.h"

namespace sym_arrow { namespace ast
{

//----------------------------------------------------------------------
//                        add_build
//----------------------------------------------------------------------
add_build::add_build(const add_log_build_info& bi)
    : base_type(this), m_vlist(value::make_zero(), value::make_one())
    , m_normalization(value::make_zero())
{
    insert_log_elem(bi.get_expr());
};

add_build::~add_build()
{
    m_vlist.close();
};

void add_build::release(stack_type& stack)
{
    vlist_stack vl_stack(stack);
    m_vlist.close(vl_stack);

    vl_stack.release(m_cannonized);
};

void add_build::update(expr& ret, const value& a, const value& scal_me, const value& b2, 
                       expr_handle s2)
{
    set_modified();

    if (scal_me.is_one() == false)
        make_scal(scal_me);

    make_add(a);
    insert_elem(b2, s2);

    ret = expr(this);
};

void add_build::update(expr& ret, const value& a, const value& scal_me)
{
    set_modified();

    if (scal_me.is_one() == false)
        make_scal(scal_me);

    make_add(a);
    ret = expr(this);
};

void add_build::insert_elem(const value& scal, expr_handle expr)
{
    if (expr->isa<scalar_rep>() == true)
    {
        make_add(scal* expr->static_cast_to<scalar_rep>()->get_data());
        return;
    }
    else if(expr->isa<mult_build>() == true)
    {
        const mult_build* mb = expr->static_cast_to<mult_build>();

        if (mb->is_simple_mult() == true)
        {
            expr_handle tmp_e       = mb->unique_ielem().get_expr_handle();
            const value& tmp_v      = mb->get_scal();
      
            insert_elem(tmp_v * scal, tmp_e);
            return;
        };

        sym_arrow::expr tmp = cannonize().make_mult(mb, do_cse_default);
        
        expr_handle be(tmp.get_ptr().get());
        return insert_elem(scal, be);
    }

    push_back(scal, expr);
};

void add_build::push_back(const value& val, expr_handle ex)
{
    if (val.is_zero() == false)
        m_vlist.push_back(val, ex);
};

void add_build::insert_log_elem(expr_handle expr)
{
    size_t code = expr->get_code();

    switch(code)
    {
        case (size_t)term_types::scalar:
        {
            make_add(log(expr->static_cast_to<scalar_rep>()->get_data()));
            return;
        }
        case (size_t)term_types::mult_rep:
        {
            const mult_rep* mh  = expr->static_cast_to<mult_rep>();

            if (mh->has_exp() == false)
            {
                push_back_log(expr);
                return;
            }

            expr_handle exp_h(mh->Exp());
            insert_elem(value::make_one(), exp_h);

            sym_arrow::expr res;
            mult_rep::remove_exp(mh, res);

            insert_log_elem(res.get_ptr().get());
            return;
        }
        case (size_t)term_types::add_rep:
        {
            const add_rep* ah  = expr->static_cast_to<add_rep>();

            if (ah->size() == 1 && ah->has_log() == false && ah->V0().is_zero() == true)
            {
                make_add(log(ah->V(0)));
                insert_log_elem(ah->E(0));
                return;
            }
            else
            {
                push_back_log(expr);
                return;
            }
        }
        // we can pertially avoid cannonization of mult and add build
        // but this will not lead to important improvement
        case (size_t)term_types::mult_build:
        {
            const mult_build* mb= expr->static_cast_to<mult_build>();
            sym_arrow::expr tmp  = cannonize().make_mult(mb, do_cse_default);
            return insert_log_elem(tmp.get_ptr().get());
        }
        case (size_t)term_types::add_build:
        {
            const add_build* ab = expr->static_cast_to<add_build>();
            sym_arrow::expr tmp  = cannonize().make_add(ab, do_cse_default);       
            return insert_log_elem(tmp.get_ptr().get());
        }
        default:
        {
            push_back_log(expr);
        }
    };    
};

void add_build::push_back_log(expr_handle ex)
{
    m_vlist.push_back_special(value::make_one(), ex);
};

};};
