/* 
 *  This file is a part of sym_arrow library.
 *
 *  Copyright (c) Pawe� Kowal 2017
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
            //auto x              = sym_arrow::symbol("x");
            //sym_arrow::expr ex  = sym_arrow::parse("A0");
            sym_arrow::expr ex  = sym_arrow::parse("A<0> - (A<0> - A<1>)^2 / (A<2> - A<1> + A<0> - A<1>)");
            disp(ex);

            auto v0             = sym_arrow::scalar::make_zero();
            auto v1             = sym_arrow::scalar::make_one();
            auto v2             = sym_arrow::scalar(2.0);

            auto dif_A0         = diff(ex, sym_arrow::symbol("A").index(v0));
            auto dif_A1         = diff(ex, sym_arrow::symbol("A").index(v1));
            auto dif_A2         = diff(ex, sym_arrow::symbol("A").index(v2));

            disp(dif_A0);
            disp(dif_A1);
            disp(dif_A2);
        }
        {
            //auto x              = sym_arrow::symbol("x");
            //sym_arrow::expr ex  = sym_arrow::parse("A0");
            sym_arrow::expr ex  = sym_arrow::parse("A0 - (A0 - A1)^2 / (A2 - A1 + A0 - A1)");
            auto dif_A0         = diff(ex, sym_arrow::symbol("A0"));
            auto dif_A1         = diff(ex, sym_arrow::symbol("A1"));
            auto dif_A2         = diff(ex, sym_arrow::symbol("A2"));

            disp(ex);
            disp(dif_A0);
            disp(dif_A1);
            disp(dif_A2);
        }
        /*
        {
            sym_arrow::set_disp_precision(-1);
            sym_arrow::set_default_precision(53);

            sym_arrow::symbol x("x");
            sym_arrow::expr ex  = log(3.564+x) * 1.007;
            //sym_arrow::expr ex  = (exp(x) - 1.0)/x;
            disp(ex);

            std::vector<sym_arrow::expr> coef;
            sym_arrow::taylor_coef(ex, x, 0.0, 5, coef);

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
    catch(sym_arrow::assert_exception& ex)
    {
        std::cout << ex.what();
    }    

    std::cout << "\n" << "finished" << "\n";
    return 0;
}
