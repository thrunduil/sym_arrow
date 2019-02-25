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

#include "sym_arrow/config.h"
#include "sym_arrow/nodes/expr.h"
#include "dag/dag.h"
#include "sym_arrow/ast/ast.h"
#include "sym_arrow/ast/builder/vlist_add.h"
#include "sym_arrow/ast/cannonization/cannonize.h"
#include "sym_arrow/ast/mult_rep.inl"
#include "sym_arrow/functions/expr_functions.h"

#include <sstream>

namespace sym_arrow { namespace details
{

class do_disp_vis : public sym_dag::dag_visitor<sym_arrow::ast::term_tag, do_disp_vis>
{
    public:
        static const int prec_atom      = 5;
        static const int prec_pow       = 4;
        static const int prec_mult      = 3;
        static const int prec_add       = 2;
        static const int prec_lowest    = 1;

    public:
        using tag_type  = sym_arrow::ast::term_tag;

        template<class Node>
        void eval(const Node* ast, std::ostream& os, int prec);

        void eval(const ast::scalar_rep* h, std::ostream& os, int prec);
        void eval(const ast::indexed_symbol_rep* h, std::ostream& os, int prec);
        void eval(const ast::add_build* h, std::ostream& os, int prec);
        void eval(const ast::mult_build* h, std::ostream& os, int prec);
        void eval(const ast::add_rep* h, std::ostream& os, int prec);
        void eval(const ast::mult_rep* h, std::ostream& os, int prec);
        void eval(const ast::function_rep* h, std::ostream& os, int prec);

    private:
        void add_paren(std::ostream& os, int this_prec, int min_prec, bool left);

        void disp_vlist_add(const ast::vlist_add* h, std::ostream& os, int prec);
        void ilist_mult_disp(const ast::ilist_mult* h, std::ostream& os, int prec);
        void rlist_mult_disp(const ast::rlist_mult* h, std::ostream& os, int prec);

        template<class Value_t, class Derived>
        void vlist_mult_disp(const ast::vlist_mult<Value_t,Derived>* h, std::ostream& os,
                int prec);

        void disp_vlist_mult_value(const int* h, std::ostream& os, int prec);
        void disp_vlist_mult_value(const value* h, std::ostream& os, int prec);
        void disp_real_power(const value& h, std::ostream& os, int prec);
        void disp_symbol(const ast::base_symbol_rep* h, std::ostream& os);
};

void do_disp_vis::add_paren(std::ostream& os, int this_prec, int min_prec, bool left)
{
    if (this_prec >= min_prec)
        return;

    if (left == true)
        os << "(";
    else
        os << ")";
}

void do_disp_vis::eval(const ast::scalar_rep* h, std::ostream& os, int prec)
{
    (void)prec;

    const value& v  = h->get_data();
    return sym_arrow::disp(os, v, false);
};  

void do_disp_vis::eval(const ast::indexed_symbol_rep* h, std::ostream& os, int prec)
{
    (void)prec;

    disp_symbol(h->get_name(), os);

    size_t n = h->size();

    if (n == 0)
        return;

    os << "<";
    
    visit(h->arg(0), os, prec_lowest);

    for (size_t j = 1; j < n; ++j)
    {
        os << ",";
        visit(h->arg(j), os, prec_lowest);
    };

    os << ">";
};

void do_disp_vis::disp_symbol(const ast::base_symbol_rep* h, std::ostream& os)
{
    os << h->get_name();
};

void do_disp_vis::eval(const ast::add_rep* h, std::ostream& os, int prec)
{
    size_t n = h->size();

    bool has_add    = (h->V0().is_zero() == false);
    bool has_elems  = (n > 0);
    bool has_log    = (h->has_log() == true);

    int this_prec   = prec_add;

    if (has_log == true && has_elems == false && has_add == false)
        this_prec   = prec_atom;

    add_paren(os, this_prec, prec, true);

    bool add_plus   = false;

    if (has_add == true)
    {
        sym_arrow::disp(os, h->V0(), false);
        add_plus    = true;
    };    

    for (size_t j = 0; j < n; ++j)
    {
        const value& b = h->V(j);

        if (b.is_one())
        {
            if (add_plus)
                os << '+'; 
        }
        else if (b.is_minus_one())
        {
            os << '-';
        }
        else if (b.is_positive())
        {
            if (add_plus)
                os << '+';

            sym_arrow::disp(os, b, false);
            os << '*';
        }
        else
        {
            sym_arrow::disp(os, b, false);
            os << '*';
        };

        visit(h->E(j), os, prec_mult);

        add_plus        = true;
    };

    if (has_log == true)
    {
        if (add_plus == true)
            os << '+';

        os << "log(";

        visit(h->Log(), os, prec_lowest);

        os << ")";
        add_plus = true;
    };

    add_paren(os, this_prec, prec, false);
};

void do_disp_vis::eval(const ast::mult_rep* h, std::ostream& os, int prec)
{    
    int this_prec   = prec_mult;
    
    if (h->isize() == 0 && h->rsize() == 0)
        this_prec   = prec_atom;

    add_paren(os, this_prec, prec, true);

    bool add_mult   = false;

    size_t n        = h->isize();

    for (size_t j = 0; j < n; ++j)
    {
        if (add_mult == true)
            os << '*';

        int b = h->IV(j);

        visit(h->IE(j), os, prec_mult);

        if (b != 1)
            os << '^' << b;

        add_mult = true;
    };

    n               = h->rsize();
    for (size_t j = 0; j < n; ++j)
    {
        if (add_mult == true)
            os << "*|";
        else
            os << "|";
        
        visit(h->RE(j), os, prec_lowest);

        os << "|";

        if (h->RV(j).is_one() == false)
        {
            os << "^";
            disp_real_power(h->RV(j), os, prec_pow);
        };
    
        add_mult = true;
    };

    if (h->has_exp() == true)
    {
        if (add_mult == true)
            os << '*';
        
        os << "exp(";

        visit(h->Exp(), os, prec_lowest);
        
        os << ")";

        add_mult = true;
    };

    add_paren(os, this_prec, prec, false);
};

void do_disp_vis::eval(const ast::function_rep* h, std::ostream& os, int prec)
{
    (void)prec;

    visit(h->name(), os, prec_lowest);

    os << "[";

    size_t n = h->size();

    if (n == 0)
    {
        os << "]";
        return;
    }

    visit(h->arg(0), os, prec_lowest);

    for (size_t j = 1; j < n; ++j)
    {
        os << ",";
        visit(h->arg(j), os, prec_lowest);
    };

    os << "]";
};

void do_disp_vis::disp_real_power(const value& h, std::ostream& os, int prec)
{
    (void)prec;
    sym_arrow::disp(os, h, false);
};

void do_disp_vis::eval(const ast::add_build* h, std::ostream& os, int prec)
{
    disp_vlist_add(&h->get_vlist(), os, prec);
};

void do_disp_vis::eval(const ast::mult_build* h, std::ostream& os, int prec)
{
    int this_prec   = prec_mult;
    add_paren(os, this_prec, prec, true);

    sym_arrow::disp(os, h->get_scal(), false);

    if (h->get_ilist())
        ilist_mult_disp(h->get_ilist(), os, prec_mult);

    if (h->get_rlist())
        rlist_mult_disp(h->get_rlist(), os, prec_mult);

    add_paren(os, this_prec, prec, false);
};

void do_disp_vis::disp_vlist_add(const ast::vlist_add* h, std::ostream& os, int prec)
{
    int this_prec   = prec_add;
    add_paren(os, this_prec, prec, true);

    value add = h->get_add();

    bool add_plus   = false;

    if (add.is_zero() == false)
    {
        sym_arrow::disp(os, add, false);
        add_plus    = true;
    };

    size_t n = h->current_size();

    for (size_t j = 0; j < n; ++j)
    {
        const value& b = h->elem(j).get_value();
    
        if (b.is_one())
        {
            if (add_plus)
                os << '+'; 
        }
        else if (b.is_minus_one())
        {
            os << '-';
        }
        else if (b.is_positive())
        {
            if (add_plus)
                os << '+';

            sym_arrow::disp(os, b, false);
            os << '*';
        }
        else
        {
            sym_arrow::disp(os, b, false);
            os << '*';
        };

        if (h->elem(j).is_special() == true)
        {
            os << "log" << "(";
            
            visit(h->elem(j).get_expr_handle(), os, prec_lowest);

            os << ")";
        }
        else
        {
            visit(h->elem(j).get_expr_handle(), os, prec_mult);
        };

        add_plus        = true;
    };

    if (h->has_previous())
    {
        const value& b = h->get_scal();
        os << "+";
        sym_arrow::disp(os, b, false);
        os << "*";

        disp_vlist_add(h->get_previous(), os, prec_mult);
    };

    add_paren(os, this_prec, prec, false);
};

void do_disp_vis::ilist_mult_disp(const ast::ilist_mult* h, std::ostream& os, int prec)
{
    using vlist_mult    = ast::ilist_mult::vlist_mult_type;
    const vlist_mult* h_base = h;
    vlist_mult_disp(h_base, os, prec);
};

void do_disp_vis::rlist_mult_disp(const ast::rlist_mult* h, std::ostream& os, int prec)
{
    using vlist_mult    = ast::rlist_mult::vlist_mult_type;
    const vlist_mult* h_base = h;
    vlist_mult_disp(h_base, os, prec);
};

template<class Value_t, class Derived>
void do_disp_vis::vlist_mult_disp(const ast::vlist_mult<Value_t,Derived>* h, std::ostream& os,
                                  int prec)
{
    using item_type = typename ast::vlist_mult<Value_t,Derived>::item_type;

    size_t n        = h->current_size();
    bool add_mult   = true;
    int this_prec   = prec_mult;

    add_paren(os, this_prec, prec, true);

    for (size_t j = 0; j < n; ++j)
    {
        if (add_mult == true)
        {
            os << '*';
        };

        const item_type& el = h->elem(j);
        Value_t b           = el.get_value();

        if (el.is_special() == false)
        {            
            visit(h->elem(j).get_expr_handle(), os, prec_pow);

            os << '^';

            disp_vlist_mult_value(&b, os, prec_pow);
        }
        else
        {
            os << "exp" << "(";

            bool has_expr   = el.get_expr_handle() != nullptr;
            int prec_loc    = has_expr ? prec_mult : prec_lowest;

            disp_vlist_mult_value(&b, os, prec_loc);

            if (has_expr == true)
            {
                os << "*";

                visit(h->elem(j).get_expr_handle(), os, prec_mult);
            };

            os << ")";
        };

        add_mult = true;
    };

    if (h->has_previous())
    {
        if (add_mult)
            os << "*";

        Value_t b = h->get_pow();

        os << "(";
        vlist_mult_disp(h->get_previous(), os, prec_lowest);
        os << ")^";

        disp_vlist_mult_value(&b, os, prec_pow);
    };

    add_paren(os, this_prec, prec, true);
};

void do_disp_vis::disp_vlist_mult_value(const int* h, std::ostream& os, int prec)
{
    (void)prec;
    os << *h;
};

void do_disp_vis::disp_vlist_mult_value(const value* h, std::ostream& os, int prec)
{
    (void)prec;
    return sym_arrow::disp(os, *h, false);
};

}};

void sym_arrow::disp_nocannonize(std::ostream& os, const expr& ex, bool add_newline)
{
    const ast::expr_base* h     = ex.get_ptr().get();

    int prec = details::do_disp_vis::prec_lowest;
    details::do_disp_vis().visit(h, os, prec);

    if (add_newline)
        os << "\n";
};

void sym_arrow::disp(std::ostream& os, const expr& ex, bool add_newline)
{
    ex.cannonize(do_cse_default);

    const ast::expr_base* h     = ex.get_ptr().get();

    int prec = details::do_disp_vis::prec_lowest;
    details::do_disp_vis().visit(h, os, prec);

    if (add_newline)
        os << "\n";
};

void sym_arrow::disp(const expr& ex, bool add_newline)
{
    return disp(std::cout, ex, add_newline);
};

std::string sym_arrow::to_string(const expr& ex)
{
    std::ostringstream os;
    disp(os, ex, false);

    return os.str();
};

static int g_disp_precision = 0;

int sym_arrow::get_disp_precision()
{
    return g_disp_precision;
}

void sym_arrow::set_disp_precision(int prec)
{
    g_disp_precision = prec;
}
