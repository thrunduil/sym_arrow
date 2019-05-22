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

#include "sym_arrow/func/compound.h"
#include "sym_arrow/ast/builder/add_build.h"
#include "sym_arrow/ast/builder/mult_build.h"
#include "sym_arrow/ast/ast.h"
#include "sym_arrow/functions/expr_functions.h"

namespace sym_arrow { namespace details
{

//TODO: check if expr is real

//----------------------------------------------------------------------
//                      ROOTS OF CONSTRUCTORS
//----------------------------------------------------------------------
void compound::plus_build(expr& ret, const value& a, const value& b,  
                          ast::expr_handle s)
{
    // forma a + b*s

    // fast exit
    if (b.is_zero())
    {
        ret = expr(a);
        return;
    };

    if (a.is_zero() && b.is_one())
    {
        ret = expr(s);
        return;
    };

    //make_build
    value be = b;

    bool is_build_1 = s->isa<ast::add_build>();

    if (is_build_1)
    {
        if (s->is_temporary())
            return get_add_build_temp(s)->update(ret, a, be);
    };

    ast::add_build_info<ast::expr_handle> ai(&a, 1, &be, &s, nullptr);

    ret = expr(ast::add_build::make(ai));
};

void compound::plus_build(expr& ret, const value& a, const value& b1, 
            ast::expr_handle s1, const value& b2, ast::expr_handle s2)
{
    //form a + b1 * s1 + b2 * s2

    // fast exit
    if (b1.is_zero())
        return plus_build(ret, a, b2, s2);

    if (b2.is_zero())
        return plus_build(ret, a, b1, s1);

    if (s1 == s2)
        return plus_build(ret, a, b1 + b2, s1);

    //make_build
    bool is_build_1 = s1->isa<ast::add_build>();
    bool is_build_2 = s2->isa<ast::add_build>();

    value b1_e  = b1;
    value b2_e  = b2;

    if (is_build_1 || is_build_2)
    {
        if (is_build_1 && s1->is_temporary())
            return get_add_build_temp(s1)->update(ret, a, b1_e, b2_e, s2);

        if (is_build_2 && s2->is_temporary())
            return get_add_build_temp(s2)->update(ret, a, b2_e, b1_e, s1);
    };

    value v[]               = {b1_e,b2_e};
    ast::expr_handle ex[]   = {s1,s2};

    ast::add_build_info<ast::expr_handle> ai(&a, 2, v, ex, nullptr);

    ret = expr(ast::add_build::make(ai));
};

ast::add_build* compound::get_add_build_temp(ast::expr_handle h)
{
    #ifdef SYM_ARROW_DEBUG_EXPR
        assertion(h->isa<ast::add_build>() == true && h->is_temporary() == true,"invalid cast");
    #endif

    return const_cast<ast::add_build*>(static_cast<const ast::add_build*>(h));
};

ast::mult_build* compound::get_mult_build_temp(ast::expr_handle h)
{
    #ifdef SYM_ARROW_DEBUG_EXPR
        assertion(h->isa<ast::mult_build>() == true && h->is_temporary() == true,"invalid cast");
    #endif

    return const_cast<ast::mult_build*>(static_cast<const ast::mult_build*>(h));
};


void compound::log_build(expr& ret, ast::expr_handle x1)
{
    //make_build
    ast::add_log_build_info ai(x1);

    ret = expr(ast::add_build::make(ai));
};

void compound::mult_build(expr& ret, const value& a, ast::expr_handle x1,  int b1)
{
    // forma a * (x1 ^ b1)

    // fast exit
    if (a.is_zero())
    {
        ret = ast::scalar_rep::make_zero();
        return;
    }

    if (b1 == 0)
    {
        ret = expr(a);
        return;
    };
    
    if (b1 == 1)
        return plus_build(ret, value::make_zero(), a, x1);

    //make_build
    bool is_build_1 = x1->isa<ast::mult_build>();
    
    if (is_build_1)
    {
        if (x1->is_temporary())
            return get_mult_build_temp(x1)->update(ret, a, b1);
    };

    ast::mult_build_info_int<ast::expr_handle> ai(&a, 1, &b1, &x1);

    ret = expr(ast::mult_build::make(ai));
};

void compound::exp_build(expr& ret, ast::expr_handle x1)
{
    ast::mult_exp_build_info ai(x1);

    ret = expr(ast::mult_build::make(ai));
};

void compound::mult_build(expr& ret, ast::expr_handle x1,  int b1, 
                          ast::expr_handle x2,  int b2)
{
    //form (x1 ^ b1) * (x2 ^ b2)

    // fast exit
    if (b1 == 0)
        return mult_build(ret, value::make_one(), x2, b2);

    if (b2 == 0)
        return mult_build(ret, value::make_one(), x1, b1);

    if (x1 == x2)
    {
        int tmp = b1+b2;
        return mult_build(ret, value::make_one(), x1, tmp);
    };

    //make_build
    bool is_build_1 = x1->isa<ast::mult_build>();
    bool is_build_2 = x2->isa<ast::mult_build>();

    if (is_build_1 || is_build_2)
    {
        if (is_build_1 && x1->is_temporary())
            return get_mult_build_temp(x1)->update(ret, value::make_one(),b1, b2, x2);

        if (is_build_2 && x2->is_temporary())
            return get_mult_build_temp(x2)->update(ret, value::make_one(), b2, b1, x1);
    };

    const int  v[]          = {b1,b2};
    ast::expr_handle ex[]   = {x1,x2};

    value one   = value::make_one();
    ast::mult_build_info_int<ast::expr_handle> ai(&one, 2, v, ex);

    ret = expr(ast::mult_build::make(ai));
};

void compound::mult_build(expr& ret, const expr& x0, const expr& p)
{
    p.cannonize(do_cse_default);
    x0.cannonize(do_cse_default);    

    //fast exit
    if (p.get_ptr()->isa<ast::scalar_rep>())
    {
        value v = p.get_ptr()->static_cast_to<ast::scalar_rep>()->get_data();
        return mult_build(ret, x0, v);
    };

    ast::expr_handle x = x0.get_expr_handle();

    if (x->isa<ast::scalar_rep>())
    {
        value v     = x->static_cast_to<ast::scalar_rep>()->get_data();

        ast::expr_handle p_h    = p.get_ptr().get();        
        return mult_build_pow(ret, v, p_h);
    }
    else
    {
        ast::expr_handle p_h    = p.get_ptr().get();

        expr log;
        log_build(log, x);        

        expr log_p;

        log.get_ptr()->make_temporary(true);        
        mult_build(log_p, log.get_ptr().get(), 1, p_h, 1);
        log.get_ptr()->make_temporary(false);

        log_p.get_ptr()->make_temporary(true);        
        exp_build(ret, log_p.get_ptr().get());
        log_p.get_ptr()->make_temporary(false);

        return;
    };
};

void compound::mult_build(expr& ret, const expr& x0, const value& p)
{
    x0.cannonize(do_cse_default);

    //forma x^b

    //fast exit
    if (p.is_zero() == true)
    {
        ret = ast::scalar_rep::make_one();
        return;
    };

    ast::expr_handle x = x0.get_expr_handle();

    if (x->isa<ast::scalar_rep>())
    {
        value v = power_real(x->static_cast_to<ast::scalar_rep>()->get_data(), p);
        ret = expr(v);
        return;
    };

    //make_build
    bool is_build_1 = x->isa<ast::mult_build>();

    if (is_build_1 && x->is_temporary())
        return get_mult_build_temp(x)->make_pow(ret, p);

    ast::mult_build_info_real ai(&x, &p);

    ret = expr(ast::mult_build::make(ai));
};

void compound::mult_build(expr& ret, const value& x, ast::expr_handle b)
{
    //fast exit
    if (b->isa<ast::scalar_rep>())
    {
        value v = power_real(x, b->static_cast_to<ast::scalar_rep>()->get_data());
        ret = expr(v);
        return;
    };

    return mult_build_pow(ret, x, b);
};

void compound::mult_build_pow(expr& ret, const value& x, ast::expr_handle b)
{
    // ! Important assumption: a^x = e^(x*log(|a|)) => (-a)^x = a^x for real a
    value v = abs(x);

    if (v.is_one())
    {
        ret = ast::scalar_rep::make_one();
        return;
    }
    else if (v.is_e())
    {
        return exp_build(ret, b);
    }
    else if (v.is_zero())
    {
        // 0^b = 0      if b > 0
        //       Nan    if b <= 0
        scalar zero = scalar::make_zero();
        expr cond   = bool_gt(expr(b), zero);
        ret         = if_then(cond, zero);
        return;
    }

    value v2 = log(v);
    
    expr ex;
    mult_build(ex, v2, b, 1);
    
    ex.get_ptr()->make_temporary(true);

    exp_build(ret, ex.get_ptr().get());

    ex.get_ptr()->make_temporary(false);
};

};};