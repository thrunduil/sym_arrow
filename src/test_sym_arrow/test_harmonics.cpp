#include "sym_arrow/sym_arrow.h"
#include "sym_arrow/utils/timer.h"

#include "../../sym_arrow/func/symbol_functions.h"
#include "test_set.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <sstream>

namespace sym_arrow { namespace testing
{

expr S(int m, const symbol& x, const symbol& y);
expr C(int m, const symbol& x, const symbol& y);

expr S(int m, const symbol& x, const symbol& y)
{
    if (m == 0)
        return scalar::make_zero();
    else 
        return x * C(m - 1, x, y) + y * S(m - 1, x, y);
};

expr C(int m, const symbol& x, const symbol& y)
{
    if (m == 0)
        return scalar::make_one();
    else 
        return -x * S(m - 1, x, y) + y * C(m - 1, x, y);
};

expr P(int l, int m, const symbol& z)
{
    if (l == 0 && m == 0)
        return scalar::make_one();

    if (l == m)
        return scalar(1 - 2 * m) * P(m - 1, m - 1, z);

    if (l == m + 1)
        return scalar(2 * m + 1) * z * P(m,m,z);

    return value::make_value(2 * l - 1.0) / value::make_value(l - m) * z * P(l - 1,m,z)
        - (value::make_value(l + m - 1.0) / value::make_value(l - m)) * P(l - 2, m, z);
}

expr spherical_harmonic(int l, int m, const symbol& x, const symbol& y, const symbol& z)
{
    int abs_m   = std::abs(m);

    if (m < 0)
        return P(l, abs_m, z) * S(abs_m, x, y);
    else
        return P(l, abs_m, z) * C(abs_m, x, y);
};

void test_set::test_harmonics()
{
    std::cout << "\n" << "test harmonics" << "\n";

    expr ret    = expr(0.0);

    tic();

    symbol x("x");
    symbol y("y");
    symbol z("z");

    std::vector<expr> diffs;

    #ifndef _DEBUG
        int max_l   = 20;
    #else
        int max_l   = 15;
    #endif
    
    for (int l = 0; l < max_l; ++l)
    for (int m = -l; m <= l; ++m)
    {
        std::ostringstream sym_name;
        sym_name << "l_" << l;
        if (m < 0)
            sym_name << "m" << -m;
        else
            sym_name << "p" << m;

        expr ex     = spherical_harmonic(l, m, x, y, z);
        expr ex_x   = diff(ex, x);
        expr ex_y   = diff(ex, y);
        expr ex_z   = diff(ex, z);

        symbol sl(sym_name.str());
        ret     = std::move(ret) + sl * std::move(ex);

        diffs.push_back(ex_x);
        diffs.push_back(ex_y);
        diffs.push_back(ex_z);
    }

    expr ret_x   = diff(ret, x);
    expr ret_y   = diff(ret, y);
    expr ret_z   = diff(ret, z);

    sym_arrow::ast::details::expr_complexity stats;

    ret.cannonize();
    sym_arrow::ast::details::measure_complexity(ret.get_ptr().get(), stats);

    double d = toc();
    
    std::cout << "spherical harmonics lev " << max_l << " time: " << d << "\n";

    std::cout << "\n";
    std::cout << "expr stast: " << "\n";
    stats.disp();    

    //disp(ret);

    //std::cout << "\n";
    //sym_dag::registered_dag_context::get().print_reuse_stats(std::cout);
    //sym_dag::registered_dag_context::get().print_memory_stats(std::cout);
    //sym_dag::registered_dag_context::get().print_collisions(std::cout);
};

}}