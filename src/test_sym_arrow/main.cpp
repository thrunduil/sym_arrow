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

#include "sym_arrow/utils/timer.h"
#include "test_set.h"
#include "sym_arrow/sym_arrow.h"

#include <iostream>
//#include <vld.h>

namespace sa = sym_arrow;

int main(int argc, const char* argv[])
{
    using namespace sym_arrow::testing;

    (void)argc;
    (void)argv;    

    // number of random trials
    //size_t n_rep = 100000;
    size_t n_rep = 100000;

    try
    {
        {
            sa::parse_def("type real");
            sa::parse_def("set A := {a1, a2, a3}");
            sa::parse_def("set B := {b1, b2, b3}");
            sa::parse_def("sym x<A,B> : real");

            sa::parse_def("set S := {a,b,c}");
            sa::parse_def("set T := {a,b,c}");
                        
            sa::index i         = sa::parse_index("i : S");
            sa::index j         = sa::parse_index("j : T");

            sa::expr e          = sa::identifier("A").indexed(i) + sa::symbol("x");

            disp(e);

            sa::expr e2         = sa::subs(e, sa::symbol("i"), j);
            sa::expr e3         = sa::subs(e, sa::symbol("i"), sa::symbol("a"));
            //sa::expr e4         = sa::subs(e, sa::symbol("i"), sa::symbol("d"));

            disp(e2);
            disp(e3);
            //disp(e4);
        }

        sym_dag::registered_dag_context::get().close();
        sym_dag::registered_dag_context::get().print_memory_leaks(std::cout);

        {
            sa::index i         = sa::index(sa::identifier("i"), sa::identifier("A"));
            sa::expr k          = i + i;
            disp(i);
            disp(k);

            sa::expr ex  = sa::parse("A<0> - (A<0> - A<1>)^2 / (A<2> - A<1> + A<0> - A<1>)");
            disp(ex);

            auto v0             = sa::scalar::make_zero();
            auto v1             = sa::scalar::make_one();
            auto v2             = sa::scalar(2.0);

            auto dif_A          = diff(ex, sa::identifier("A").indexed(i));
            auto dif_A0         = diff(ex, sa::identifier("A").indexed(v0));
            auto dif_A1         = diff(ex, sa::identifier("A").indexed(v1));
            auto dif_A2         = diff(ex, sa::identifier("A").indexed(v2));

            disp(dif_A);
            disp(dif_A0);
            disp(dif_A1);
            disp(dif_A2);
        }
        {
            //auto x              = sa::symbol("x");
            //sa::expr ex  = sa::parse("A0");
            sa::expr ex  = sa::parse("A0 - (A0 - A1)^2 / (A2 - A1 + A0 - A1)");
            auto dif_A0         = diff(ex, sa::symbol("A0"));
            auto dif_A1         = diff(ex, sa::symbol("A1"));
            auto dif_A2         = diff(ex, sa::symbol("A2"));

            disp(ex);
            disp(dif_A0);
            disp(dif_A1);
            disp(dif_A2);
        }
        /*
        {
            sa::set_disp_precision(-1);
            sa::set_default_precision(53);

            sa::symbol x("x");
            sa::expr ex  = log(3.564+x) * 1.007;
            //sa::expr ex  = (exp(x) - 1.0)/x;
            disp(ex);

            std::vector<sa::expr> coef;
            sa::taylor_coef(ex, x, 0.0, 5, coef);

            for (const auto& it : coef)
                disp(it);
        };
        */

        test_set::example();

        test_set::test_diff();
        test_set::test_diff_context();

        test_set::test_harmonics();

        test_set::test_special_cases();
        test_set::test_visitor();        

        test_set::test_random_diff(n_rep);
        test_set::test_expression(n_rep);

        std::cout << "\n";

        //sym_dag::registered_dag_context::get().print_reuse_stats(std::cout);
        //sym_dag::registered_dag_context::get().print_memory_stats(std::cout);
        //sym_dag::registered_dag_context::get().print_collisions(std::cout);

        sym_dag::registered_dag_context::get().close();
        sym_dag::registered_dag_context::get().print_memory_leaks(std::cout);
    }
    catch(std::exception& ex)
    {
        std::cout << ex.what();
    }
    catch(sa::assert_exception& ex)
    {
        std::cout << ex.what();
    }    

    std::cout << "\n" << "finished" << "\n";
    return 0;
}
