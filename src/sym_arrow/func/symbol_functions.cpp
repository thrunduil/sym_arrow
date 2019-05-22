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

#include "sym_arrow/func/symbol_functions.h"
#include "sym_arrow/ast/ast.h"
#include "sym_arrow/ast/traversal_visitor.h"
#include "sym_arrow/functions/expr_functions.h"

bool sym_arrow::contain_symbol(const expr& ex, const symbol& sym)
{
    ast::symbol_handle sh = sym.get_ptr().get();

    if (ex.is_null() == true || !sh)
        return false;

    ex.cannonize(do_cse_default);

    return ast::details::has_symbol(ex.get_ptr().get(), sh->get_base_symbol_code());
};

bool sym_arrow::contain_any(const expr& ex, const std::vector<symbol>& syms)
{
    if (syms.size() == 0)
        return true;

    if (ex.is_null() == true)
        return false;

    ex.cannonize(do_cse_default);

    if (syms.size() == 1)
        return ast::details::has_symbol(ex.get_ptr().get(), syms[0].get_ptr()->get_base_symbol_code());

    std::vector<size_t> codes(syms.size());

    size_t N = syms.size();

    for (size_t i = 0; i < N; ++i)
        codes[i] = syms[i].get_ptr()->get_base_symbol_code();

    std::sort(codes.begin(), codes.end());

    using dbs   = dbs_lib::dbs;

    dbs f = dbs(N, codes.data());
    return ast::details::has_any_symbol(ex.get_ptr().get(), f);
};

bool sym_arrow::contain_all(const expr& ex, const std::vector<symbol>& syms)
{
    if (syms.size() == 0)
        return true;

    if (ex.is_null() == true)
        return false;

    ex.cannonize(do_cse_default);

    if (syms.size() == 1)
    {
        return ast::details::has_symbol(ex.get_ptr().get(), 
                    syms[0].get_ptr()->get_base_symbol_code());
    }

    std::vector<size_t> codes(syms.size());

    size_t N = syms.size();

    for (size_t i = 0; i < N; ++i)
        codes[i] = syms[i].get_ptr()->get_base_symbol_code();

    std::sort(codes.begin(), codes.end());

    using dbs   = dbs_lib::dbs;

    dbs f = dbs(N, codes.data());
    return ast::details::has_all_symbols(ex.get_ptr().get(), f);
};

namespace sym_arrow { namespace ast { namespace details
{

class do_has_symbol : public sym_dag::dag_visitor<sym_arrow::ast::term_tag, 
                                    do_has_symbol>
{
    public:
        using tag_type  = sym_arrow::ast::term_tag;

    public:
        template<class Node>
        bool eval(const Node* ast, size_t code);

        bool eval(const ast::scalar_rep*, size_t)       { return false; };        
        bool eval(const ast::add_build*, size_t)        { return false; };
        bool eval(const ast::mult_build*, size_t)       { return false; };
        bool eval(const ast::add_rep* h, size_t code)   { return h->has_symbol(code); };
        bool eval(const ast::mult_rep* h, size_t code)  { return h->has_symbol(code); };
        bool eval(const ast::function_rep* h, size_t c) { return h->has_symbol(c); };
        bool eval(const ast::indexed_symbol_rep* h, size_t c)
                                                        { return h->has_symbol(c); };

        bool eval(const ast::index_rep* h, size_t c)    { return h->has_symbol(c); };
};

class do_has_any_symbol : public sym_dag::dag_visitor<sym_arrow::ast::term_tag, 
                                    do_has_any_symbol>
{
    public:
        using tag_type  = sym_arrow::ast::term_tag;
        using dbs       = dbs_lib::dbs;

    public:
        template<class Node>
        bool eval(const Node* ast, const dbs& set);

        bool eval(const ast::scalar_rep*, const dbs&)       { return false; };        
        bool eval(const ast::add_build*, const dbs&)        { return false; };
        bool eval(const ast::mult_build*, const dbs&)       { return false; };
        bool eval(const ast::add_rep* h, const dbs& f)      { return h->has_any_symbol(f); };
        bool eval(const ast::mult_rep* h, const dbs& f)     { return h->has_any_symbol(f); };
        bool eval(const ast::function_rep* h, const dbs& f) { return h->has_any_symbol(f); };
        bool eval(const ast::indexed_symbol_rep* h, const dbs& f)   
                                                            { return h->has_any_symbol(f); };

        bool eval(const ast::index_rep* h, const dbs& f)    { return h->has_any_symbol(f); };
};

class do_has_all_symbol : public sym_dag::dag_visitor<sym_arrow::ast::term_tag, 
                                    do_has_all_symbol>
{
    public:
        using tag_type  = sym_arrow::ast::term_tag;
        using dbs       = dbs_lib::dbs;

    public:
        template<class Node>
        bool eval(const Node* ast, const dbs& set);

        bool eval(const ast::scalar_rep*, const dbs&)       { return false; };        
        bool eval(const ast::add_build*, const dbs&)        { return false; };
        bool eval(const ast::mult_build*, const dbs&)       { return false; };

        bool eval(const ast::add_rep* h, const dbs& f)      { return h->has_all_symbols(f); };
        bool eval(const ast::mult_rep* h, const dbs& f)     { return h->has_all_symbols(f); };
        bool eval(const ast::function_rep* h, const dbs& f) { return h->has_all_symbols(f); };
        bool eval(const ast::indexed_symbol_rep* h, const dbs& f)   
                                                            { return h->has_all_symbols(f); };

        bool eval(const ast::index_rep* h, const dbs& f)    { return h->has_all_symbols(f); };
};

class do_add_symbols : public sym_dag::dag_visitor<sym_arrow::ast::term_tag, 
                                    do_add_symbols>
{
    public:
        using tag_type  = sym_arrow::ast::term_tag;
        using dbs       = dbs_lib::dbs;

    public:
        template<class Node>
        void eval(const Node* h, dbs& set)
        {
            set = set | h->get_symbol_set();
        }

        void eval(const ast::scalar_rep*, dbs&)       
        {};
        
        void eval(const ast::add_build*, dbs&)
        {};

        void eval(const ast::mult_build*, dbs&)
        {};

        void eval(const ast::indexed_symbol_rep* h, dbs& set)
        { 
            set = set | h->get_symbol_set();
        };
};

class do_measure_complexity : public sym_arrow::ast::traversal_visitor<do_measure_complexity>
{
    public:
        using tag_type  = sym_arrow::ast::term_tag;
        using base_type = ast::traversal_visitor<do_measure_complexity>;

    public:
        template<class Node>
        void eval(const Node* h, expr_complexity& compl);

        void eval(const ast::scalar_rep*, expr_complexity& compl)       
        {
            compl.add_scalar();
        };
        
        void eval(const ast::add_build*, expr_complexity&)
        {};

        void eval(const ast::mult_build*, expr_complexity&)
        {};

        void eval(const ast::indexed_symbol_rep*, expr_complexity& compl)
        { 
            compl.add_symbol();
        };
        void eval(const ast::index_rep*, expr_complexity& compl)
        { 
            compl.add_symbol();
        };

        void eval(const ast::add_rep* h, expr_complexity& compl)
        { 
            compl.add_add_rep(h->size(), h->has_log());
            base_type::eval(h, compl);
        };
        
        void eval(const ast::mult_rep* h, expr_complexity& compl)
        { 
            compl.add_mult_rep(h->isize(), h->rsize(), h->has_exp());
            base_type::eval(h, compl);
        };

        void eval(const ast::function_rep* h, expr_complexity& compl)
        { 
            compl.add_function_rep(h->size());
            base_type::eval(h, compl);
        };
};

}}}

namespace sym_arrow { namespace ast
{

bool details::has_symbol(expr_handle h, size_t symbol_code)
{
    return details::do_has_symbol().visit(h, symbol_code);
}

bool details::has_any_symbol(expr_handle h, const dbs_lib::dbs& set)
{
    return details::do_has_any_symbol().visit(h, set);
}

bool details::has_all_symbols(expr_handle h, const dbs_lib::dbs& set)
{
    return details::do_has_all_symbol().visit(h, set);
}

void details::add_symbols(expr_handle h, dbs_lib::dbs& set)
{
    return details::do_add_symbols().visit(h, set);
};

void details::measure_complexity(expr_handle h, expr_complexity& compl)
{
    return details::do_measure_complexity().visit(h, compl);
};

//-------------------------------------------------------------------
//                  expr_complexity
//-------------------------------------------------------------------
details::expr_complexity::expr_complexity()
    : m_subnodes(0), m_add_subnodes(0), m_mult_subnodes(0), m_function_subnodes(0)
    , m_scalar_subnodes(0), m_symbol_subnodes(0), m_add_children(0), m_mult_children_imult(0)
    , m_mult_children_rmult(0), m_func_children(0), m_log_subnodes(0), m_exp_subnodes(0)
{};

void details::expr_complexity::add_scalar()
{
    ++m_subnodes;
    ++m_scalar_subnodes;
}

void details::expr_complexity::add_symbol()
{
    ++m_subnodes;
    ++m_symbol_subnodes;
}

void details::expr_complexity::add_add_rep(size_t size, bool has_log)
{
    (void)size;
    ++m_subnodes;
    ++m_add_subnodes;

    m_add_children += size;

    if (has_log)
        ++m_log_subnodes;
};

void details::expr_complexity::add_mult_rep(size_t isize, size_t rsize, bool has_exp)
{
    ++m_subnodes;
    ++m_mult_subnodes;

    m_mult_children_imult += isize;
    m_mult_children_rmult += rsize;

    if (has_exp)
        ++m_exp_subnodes;
};

void details::expr_complexity::add_function_rep(size_t size)
{
    ++m_subnodes;
    ++m_function_subnodes;
    m_func_children += size;
};

bool details::expr_complexity::operator<(const expr_complexity& other) const
{
    if (this->m_subnodes < other.m_subnodes)
        return true;
    if (this->m_subnodes > other.m_subnodes)
        return false;

    if (this->m_mult_subnodes < other.m_mult_subnodes)
        return true;
    if (this->m_mult_subnodes > other.m_mult_subnodes)
        return false;

    if (this->m_add_subnodes < other.m_add_subnodes)
        return true;
    if (this->m_add_subnodes > other.m_add_subnodes)
        return false;

    if (this->m_log_subnodes < other.m_log_subnodes)
        return true;
    if (this->m_log_subnodes > other.m_log_subnodes)
        return false;

    if (this->m_exp_subnodes < other.m_exp_subnodes)
        return true;
    if (this->m_exp_subnodes > other.m_exp_subnodes)
        return false;

    if (this->m_function_subnodes < other.m_function_subnodes)
        return true;
    if (this->m_function_subnodes > other.m_function_subnodes)
        return false;

    if (this->m_scalar_subnodes < other.m_scalar_subnodes)
        return true;
    if (this->m_scalar_subnodes > other.m_scalar_subnodes)
        return false;

    if (this->m_symbol_subnodes < other.m_symbol_subnodes)
        return true;
    if (this->m_symbol_subnodes > other.m_symbol_subnodes)
        return false;

    if (this->m_add_children < other.m_add_children)
        return true;
    if (this->m_add_children > other.m_add_children)
        return false;

    if (this->m_mult_children_imult < other.m_mult_children_imult)
        return true;
    if (this->m_mult_children_imult > other.m_mult_children_imult)
        return false;

    if (this->m_mult_children_rmult < other.m_mult_children_rmult)
        return true;
    if (this->m_mult_children_rmult > other.m_mult_children_rmult)
        return false;

    if (this->m_func_children < other.m_func_children)
        return true;
    if (this->m_func_children > other.m_func_children)
        return false;

    return false;
};

void details::expr_complexity::disp(std::ostream& os)
{
    os << "subnodes: "          << this->m_subnodes << "\n";
    os << "add subnodes: "      << this->m_add_subnodes << "\n";
    os << "mult subnodes: "     << this->m_mult_subnodes << "\n";
    os << "scalar subnodes: "   << this->m_scalar_subnodes << "\n";
    os << "symbol subnodes: "   << this->m_symbol_subnodes << "\n";
    os << "add children: "      << this->m_add_children << "\n";
    os << "mult children ipow: "<< this->m_mult_children_imult << "\n";
    os << "mult children rmult: "<<this->m_mult_children_rmult << "\n";
    os << "func children: "     << this->m_func_children << "\n";
    os << "log subnodes: "      << this->m_log_subnodes << "\n";
    os << "exp subnodes: "      << this->m_exp_subnodes << "\n";
};

}}