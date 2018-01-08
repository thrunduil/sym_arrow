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

#pragma once

#include "test_set.h"
#include "expr_rand.h"
#include "sym_arrow/utils/timer.h"
#include "../../sym_arrow/func/symbol_functions.h"


#include <fstream>

namespace sym_arrow { namespace testing
{

void test_set::test_special_cases()
{
    expr ex = parse("y1 + y2 + y3 + x*(1/x*(y1 + y2 + y3 + y4 + y5 + y6 + y7 + y8) + 1)"
                    " + x*(1/x*(y1 + y2 + y3 + y4 + y5 + y6 + y7 + y8) - 1)");

    // checking resize in item_collector_add
    ex.cannonize();
};

void test_set::test_random_diff(size_t n_rep)
{
    std::cout << "\n";
    std::cout << "test random diff" << "\n";

    init_genrand(204);

    int n_sym           = 2;
    rand_state r(n_sym, 12, false, false);
    //rand_state r(n_sym, 12, false, false);

    bool show           = false;

    // set stop_index to the value you want the loop to give
    // comments about calculations; value -1 if you don't want comments at all
    size_t stop_index   = (size_t)-1;

    // display progress every nr_disp_gap
    int nr_disp_gap     = 20000;

    // diff symbol; must be the same for all random expr
    symbol sym          = r.rand_sym();

    tic();

    for(size_t i = 0; i < n_rep; ++i)
    {
        if (i % nr_disp_gap == 0)
            std::cout << i << "\n";        

        if (i == stop_index)
        {
            r.set_test_value(true);
            r.get_debug_flag()  = true;
            show                = true;
        };
        
        expr_value_dif exv  = r.rand_expr_dif(0, sym, show);
        expr ex_base        = exv.get_base();
        expr ex_base2       = exv.get_base();
        expr ex_dif         = diff(ex_base2, sym);

        error_value ev_dif  = exv.get_dif_value();        

        if (i == stop_index)
        {
            disp_nocannonize(std::cout, ex_base);
            disp_nocannonize(std::cout, ex_dif);
        }

        //#ifdef _DEBUG
            bool is_valid = check_expression(ex_dif);

            if (is_valid == false)
            {
                check_expression(ex_dif);

                disp_nocannonize(std::cout, ex_base);
                disp_nocannonize(std::cout, ex_dif);


                std::cout << "\n" << i << "\n";
            };			
            
            value ev2		    = eval(ex_dif, rand_data_provider(&r));
            double ulp_error;
            bool is_value_valid = rand_state::values_equal(ev_dif, ev2, ulp_error);

            if (is_value_valid == false)
            {				                
                disp_nocannonize(std::cout, ex_dif);

                std::cout << "\n" << i << " " << "ulp error: " << ulp_error << "\n";

                disp(ev_dif);
                disp(ev2);				
            };
        //#endif        
    };

    double d = toc();
    std::cout << "time: " << d << "\n";
};

void test_set::test_diff()
{
    symbol x("x");
    symbol y("y");
    symbol z("z");

    expr e1  = 1. * exp(-x*x + y*x + y*z) + x/ power_int(1.0 + x*x + y * x + z/x, 2);
    expr e2 = parse("1/(1 + exp[-1/x^2])");    

  #ifndef _DEBUG
    int n1  = 20;
    int n2  = 40;
  #else
    int n1  = 10;
    int n2  = 20;
  #endif

    test_diff(e1, n1, n2, false);
    test_diff(e2, n1, n2, false);
}

void test_set::test_diff(const expr& ex, int n1, int n2, bool disp_stats)
{
    std::cout << "\n" << "test diff: ";
    disp(ex);

    value v;  
    symbol x("x");
    expr ed = ex;    

    for (int i = 0; i < n1; ++i)
        ed = diff(ed, x); 

    expr ed2    = ed;

    tic();

    for (int i = n1 + 1; i < n2; ++i)
        ed = diff(ed, x); 

    double t1 = toc();

    ed2     = simplify(ed2);

    std::cout << "diff time 1: " << t1 << "\n";

    tic();

    for (int i = n1 + 1; i < n2; ++i)
        ed2 = diff(ed2, x); 

    double t2 = toc();
    std::cout << "diff time 2: " << t2 << "\n";

    tic();

    ed      = simplify(ed);
    ed2     = simplify(ed2);

    double t3 = toc();
    std::cout << "simpl time: " << t3 << "\n";

    if (disp_stats == true)
    {
        sym_arrow::ast::details::expr_complexity stats1;    
        sym_arrow::ast::details::measure_complexity(ed.get_ptr().get(), stats1);

        std::cout << "\n";
        stats1.disp();
    
        sym_arrow::ast::details::expr_complexity stats2;
        sym_arrow::ast::details::measure_complexity(ed2.get_ptr().get(), stats2);

        std::cout << "\n";
        std::cout << "simpl" << "\n";
        stats2.disp();    
    };
};

void test_set::test_expression(size_t n_rep)
{
    std::cout << "\n";
    std::cout << "test random expressions" << "\n";

    init_genrand(203);

    //int n_sym         = 1000000;
    int n_sym           = 5;
    //rand_state r(n_sym, 12, false, false);
    rand_state r(n_sym, 12, false, false);

    bool show           = false;
    size_t stop_index   = (size_t)-1;

    tic();
    for(size_t i = 0; i < n_rep; ++i)
    {
        if (i % 20000 == 0)
            std::cout << i << "\n";        

        if (i == stop_index)
        {
            r.set_test_value(true);
            r.get_debug_flag()  = true;
            show                = true;
        };

        expr_value exv  = r.rand_expr(0, show);
        error_value ev  = exv.second;
        expr ex			= exv.first;

        if (i == stop_index)
            disp_nocannonize(std::cout, ex);

        #ifdef _DEBUG
            bool is_valid = check_expression(ex);

            if (is_valid == false)
            {
                check_expression(ex);
                disp_nocannonize(std::cout, ex);

                std::cout << "\n" << i << "\n";
            };			
            
            value ev2		    = eval(ex, rand_data_provider(&r));
            double ulp_error;
            bool is_value_valid = rand_state::values_equal(ev, ev2, ulp_error);

            if (is_value_valid == false)
            {				                
                disp_nocannonize(std::cout, ex);

                std::cout << "\n" << i << " " << "ulp error: " << ulp_error << "\n";

                disp(ev);
                disp(ev2);				
            };
        #endif
    };

    double d = toc();

    std::cout << "time: " << d << "\n";

    //std::cout <<"\n";
    //sym_dag::registered_dag_context::get().print_reuse_stats(std::cout);        

    std::cout <<"\n";
    sym_dag::registered_dag_context::get().print_memory_stats(std::cout);

    //std::cout <<"\n";
    //sym_dag::registered_dag_context::get().print_collisions(std::cout);
};

void test_set::init_genrand(size_t seed)
{
    return testing::init_genrand((unsigned long)seed);
}

void test_set::tic()
{
    return sym_arrow::tic();
}

double test_set::toc()
{
    return sym_arrow::toc();
}

struct num_syms : sym_arrow::expr_traversal_visitor<num_syms>
{
    using sym_arrow::expr_traversal_visitor<num_syms>::eval;

    void eval(const sym_arrow::symbol&, size_t& n_syms)
    { 
        ++n_syms;
    };
};

void test_set::test_visitor()
{
    sym_arrow::expr ex   = sym_arrow::parse("x + sin[1+x] + cos[2*exp[x] + log[y]] + x^2*y^y");
    size_t num = 0;
    num_syms().visit(ex, num);
    
    std::cout << "\n";

    if (num != 7)
        std::cout << "test_visitor: FAILED" << "\n";
    else
        std::cout << "test_visitor: OK" << "\n";
};

void test_set::test_diff_context()
{
    diff_context dc;

    symbol sin("sin");
    symbol cos("cos");
    symbol x("x");

    dc.add_diff_rule(sin, 1, &x, 0, cos(x), true);
    dc.add_diff_rule(cos, 1, &x, 0, -sin(x), true);

    expr ex         = parse("sin[x^2]*cos[x^2]]");

    int n_dif       = 100;
    expr ex_dif     = ex;
    
    tic();

    for (int i = 0; i < n_dif; ++i)
        ex_dif      = diff(ex_dif, x, dc);

    double t = toc();

    std::cout << "\n" << "test diff context:" << "\n";
    std::cout << "func diff time: " << t << "\n";  
};


}};
