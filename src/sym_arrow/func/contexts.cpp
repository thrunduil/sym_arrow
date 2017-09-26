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

#include "sym_arrow/functions/contexts.h"
#include "sym_arrow/ast/symbol_rep.h"
#include "sym_arrow/nodes/symbol.inl"
#include "sym_arrow/ast/ast.h"
#include "sym_arrow/functions/expr_functions.h"
#include "sym_arrow/error/error_formatter.h"

#include <sstream>

namespace sym_arrow { namespace details
{

class subs_context_impl
{
    public:
        using expr_vec      = std::vector<expr>;
        using sym_map       = std::map<symbol, size_t>;
        using dbs           = dbs_lib::dbs;

    public:
        subs_context_impl();

        dbs                 m_set;
        sym_map             m_map;        
        expr_vec            m_buffer;
        const expr*         m_bind;

    public:
        void                remove_bind();

    public:
        void                error_invalid_bind_size(size_t size, size_t exp_size) const;
};    

subs_context_impl::subs_context_impl()
    :m_bind(nullptr)
{};

void subs_context_impl::remove_bind()
{
    m_bind = nullptr;
}

void subs_context_impl::error_invalid_bind_size(size_t size, size_t exp_size) const
{
    error::error_formatter ef;
    ef.head() << "invalid size of bind array";

    ef.new_info();
    ef.line() << "expecting array of size: " << exp_size;

    ef.new_info();
    ef.line() << "supplied array has size: " << size;

    throw std::runtime_error(ef.str());
};

}};

namespace sym_arrow
{

subs_context::subs_context()
    :m_impl(new sym_arrow::details::subs_context_impl())
{};

void subs_context::add_symbol(const symbol& sym, const expr& ex)
{
    m_impl->remove_bind();
    
    size_t code = m_impl->m_buffer.size();
    m_impl->m_buffer.push_back(ex);
    return add_symbol(sym, code);
};

size_t subs_context::add_symbol(const symbol& sym)
{
    m_impl->remove_bind();

    size_t code = m_impl->m_buffer.size();
    m_impl->m_buffer.push_back(expr());

    add_symbol(sym, code);
    return code;
};

void subs_context::set_bind(size_t size, const expr* ex)
{
    m_impl->remove_bind();

    m_impl->m_bind  = ex;
    size_t exp_size = this->bind_array_length();

    if (size != exp_size)
        m_impl->error_invalid_bind_size(size, exp_size);
};

void subs_context::remove_bind()
{
    m_impl->remove_bind();
};

void subs_context::add_symbol(const symbol& sym, size_t code)
{
    m_impl->remove_bind();

    m_impl->m_map[sym]  = code;
    m_impl->m_set       = m_impl->m_set.set(sym.get_ptr()->get_symbol_code());
};

void subs_context::remove_symbol(const symbol& sym)
{
    remove_bind();

    m_impl->m_map.erase(sym);
    m_impl->m_set = m_impl->m_set.reset(sym.get_ptr()->get_symbol_code());
};

size_t subs_context::size() const
{
    return m_impl->m_map.size();
};

size_t subs_context::bind_array_length() const
{
    return m_impl->m_buffer.size();
};

const dbs_lib::dbs& subs_context::get_symbol_set() const
{
    return m_impl->m_set;
};

void subs_context::disp(std::ostream& os) const
{
    for (auto pos = m_impl->m_map.begin(); pos != m_impl->m_map.end(); ++pos)
    {
        sym_arrow::disp(os, pos->first, false);
        os << " -> ";

        size_t code = pos->second;
        expr subs   = m_impl->m_buffer[code];

        if (subs.is_null() == false)
            sym_arrow::disp(os, subs, false);
        else
            os << "$[" << code << "]";

        os << "\n";
    };
};

expr subs_context::subs(const symbol& sh) const
{
    auto pos = m_impl->m_map.find(sh);

    if (pos == m_impl->m_map.end())
        return expr();

    size_t code = pos->second;
    expr ex     = m_impl->m_buffer[code];

    if (ex.is_null() == true && m_impl->m_bind == nullptr)
        return ex;
    else
        return m_impl->m_bind[code];
};

void subs_context::visit_substitutions(substitution_vis& info) const
{
    size_t n        = this->size();
    size_t n_codes  = this->bind_array_length();

    info.report_size(n, n_codes);

    for (const auto& elem :  m_impl->m_map)
    {
        const symbol& sym   = elem.first;
        size_t code         = elem.second;
        const expr* ex      = &m_impl->m_buffer[code];

        if (ex->is_null() == true)
            ex              = &m_impl->m_bind[code];

        info.report_subs(sym, *ex, code);
    };
};

//-------------------------------------------------------------------
//                        diff_context
//-------------------------------------------------------------------

namespace details
{

class diff_rule
{
    private:
        expr            m_expr;
        subs_context    m_subs;

    public:
        diff_rule(size_t n_args, const symbol* args, const expr& dif);

        const expr&     get_diff_result() const;
        size_t          number_args() const;
        void            get_function_args(std::vector<symbol>& rule_args) const;
        expr            make_subs(size_t n_args, const expr* args);
};

class diff_context_impl
{
    private:
                        // function_name x number args x arg number
        using key       = std::tuple<symbol, size_t, size_t>;
        using value     = diff_rule;

        using diff_map  = std::map<key, value>;

    private:
        diff_map        m_diff_map;

    public:
        diff_context_impl();

        expr            diff(const symbol& func_name, size_t arg_num, const expr* args,
                            size_t n_args);
        void            add_diff_rule(const symbol& func_name, size_t n_args,
                            const symbol* args, size_t diff_arg, const expr& dif);

    private:
        void            error_rule_defined(const symbol& func_name, size_t n_args,
                            const symbol* args, size_t diff_arg, const expr& dif,
                            const diff_rule& prev_rule);
        void            disp_diff_def(std::ostream& os, const symbol& func_name, size_t n_args,
                            const symbol* args, size_t diff_arg, const expr& dif);
        void            disp_function(std::ostream& os, const symbol& func_name, size_t n_args, 
                            const symbol* args);
};

diff_rule::diff_rule(size_t n_args, const symbol* args, const expr& dif)
    :m_expr(dif)
{
    for (size_t i = 0; i < n_args; ++i)
        m_subs.add_symbol(args[i]);
};

const expr& diff_rule::get_diff_result() const
{
    return m_expr;
}

size_t diff_rule::number_args() const
{
    return m_subs.size();
}

expr diff_rule::make_subs(size_t n_args, const expr* args)
{
    m_subs.set_bind(n_args, args);
    return subs(m_expr, m_subs);
};

void diff_rule::get_function_args(std::vector<symbol>& rule_args) const
{
    struct substitution_info_impl : public substitution_vis
    {
        std::vector<symbol>& rule_args;

        substitution_info_impl(std::vector<symbol>& args)
            :rule_args(args)
        {};

        void report_size(size_t size, size_t bind_length) override
        {
            (void)size;
            rule_args.resize(bind_length);
        };

        void report_subs(const symbol& sym, const expr& ex, size_t code) override
        {
            (void)ex;
            rule_args[code] = sym;
        };
    };

    substitution_info_impl si(rule_args);
    m_subs.visit_substitutions(si);
};

diff_context_impl::diff_context_impl()
{};

void diff_context_impl::add_diff_rule(const symbol& func_name, size_t n_args,
                    const symbol* args, size_t diff_arg, const expr& dif)
{
    using value_type    = diff_map::value_type;

    key k       = key(func_name, n_args, diff_arg);
    auto pos    = m_diff_map.find(k);

    if (pos != m_diff_map.end())
        error_rule_defined(func_name, n_args, args, diff_arg, dif, pos->second);

    m_diff_map.insert(pos, value_type(k, diff_rule(n_args, args, dif)));
};

expr diff_context_impl::diff(const symbol& func_name, size_t arg_num, const expr* args,
                    size_t n_args)
{
    key k       = key(func_name, n_args, arg_num);

    auto pos = m_diff_map.find(k);

    if (pos == m_diff_map.end())
        return expr();

    return pos->second.make_subs(n_args, args);
};

void diff_context_impl::error_rule_defined(const symbol& func_name, size_t n_args,
                        const symbol* args, size_t diff_arg, const expr& dif,
                        const diff_rule& prev_rule)
{
    error::error_formatter ef;
    ef.head() << "differentiation rule already defined";

    auto l1 = ef.new_info();

    ef.line() << "current definition:";
    ef.new_line(l1);
        disp_diff_def(ef.line(), func_name, n_args, args, diff_arg, dif);

    auto l2 = ef.new_info();
    ef.line() << "previous definition:";
    ef.new_line(l1);
    {
        std::vector<symbol> rule_args;
        prev_rule.get_function_args(rule_args);
        disp_diff_def(ef.line(), func_name, rule_args.size(), rule_args.data(), 
                      diff_arg, prev_rule.get_diff_result());
    }

    throw std::runtime_error(ef.str());
};

void diff_context_impl::disp_diff_def(std::ostream& os, const symbol& func_name, size_t n_args,
                    const symbol* args, size_t diff_arg, const expr& dif)
{    
    os << "d/d"; disp(os, args[diff_arg], false);
    os << " ";
    disp_function(os, func_name, n_args, args);
    os << " -> ";
    disp(os, dif, false);
};

void diff_context_impl::disp_function(std::ostream& os, const symbol& func_name, size_t n_args, 
                    const symbol* args)
{
    disp(os, func_name, false);
    
    if (n_args == 0)
    {
        os << "[]";
        return;
    }

    os << "[";
    disp(os, args[0], false);

    for (size_t i = 1; i < n_args; ++i)
    {
        os << ", ";
        disp(os, args[i], false);
    };

    os << "]";
    return;
};

};

diff_context::diff_context()
    :m_impl(new sym_arrow::details::diff_context_impl())
{};

expr diff_context::diff(const symbol& func_name, size_t arg_num, const expr* args,
                    size_t n_args)
{
    return m_impl->diff(func_name, arg_num, args, n_args);
};

void diff_context::add_diff_rule(const symbol& func_name, size_t n_args,
                    const symbol* args, size_t diff_arg, const expr& dif)
{
    return m_impl->add_diff_rule(func_name, n_args, args, diff_arg, dif);
};

// global instance
diff_context* g_diff_context = sym_dag::global_objects::make_before<diff_context>();

const diff_context& sym_arrow::global_diff_context()
{
    return *g_diff_context;
};

};