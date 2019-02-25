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

#include "sym_arrow/func/contexts_impl.h"
#include "sym_arrow/ast/symbol_rep.h"
#include "sym_arrow/nodes/symbol.inl"
#include "sym_arrow/ast/ast.h"
#include "sym_arrow/functions/expr_functions.h"
#include "sym_arrow/functions/sym_functions.h"
#include "sym_arrow/error/error_formatter.h"
#include "sym_arrow/functions/global_context_initializers.h"
#include "sym_arrow/utils/stack_array.h"

#include <sstream>

namespace sym_arrow { namespace details
{

//-------------------------------------------------------------------
//                        subs_context_impl
//-------------------------------------------------------------------

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

//-------------------------------------------------------------------
//                        diff_rule
//-------------------------------------------------------------------

diff_rule::diff_rule(size_t n_args, const symbol* args, const expr& dif, bool make_dif)
    :m_expr(dif), m_make_dif(make_dif)
{
    for (size_t i = 0; i < n_args; ++i)
        m_subs.add_symbol(args[i]);
};

const expr& diff_rule::get_diff_result(bool& make_dif) const
{
    make_dif = m_make_dif;
    return m_expr;
}

size_t diff_rule::number_args() const
{
    return m_subs.size();
}

bool diff_rule::make_subs(size_t n_args, const expr* args, expr& res)
{
    if (m_make_dif == false)
    {
        res = expr();
        return false;
    }

    m_subs.set_bind(n_args, args);
    res = subs(m_expr, m_subs);
    return true;
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

//-------------------------------------------------------------------
//                        diff_context_impl
//-------------------------------------------------------------------

diff_context_impl::diff_context_impl()
    :m_is_global(false)
{};

void diff_context_impl::set_global()
{
    m_is_global = true;
};

void diff_context_impl::add_diff_rule(const symbol& func_name, size_t n_args,
                    const symbol* args, size_t diff_arg, const expr& dif, bool make_diff)
{
    using value_type    = diff_map::value_type;

    key k       = key(func_name, n_args, diff_arg);
    auto pos    = m_diff_map.find(k);

    if (pos != m_diff_map.end())
        error_rule_defined(func_name, n_args, args, diff_arg, dif, make_diff, pos->second);

    m_diff_map.insert(pos, value_type(k, diff_rule(n_args, args, dif, make_diff)));
};

bool diff_context_impl::diff(const symbol& func_name, size_t arg_num, const expr* args,
                    size_t n_args, expr& res)
{
    key k       = key(func_name, n_args, arg_num);

    auto pos    = m_diff_map.find(k);

    if (pos == m_diff_map.end())
    {
        if (m_is_global == true)
        {
            res     = expr();
            return true;
        }
        else
        {
            diff_context gl = global_diff_context();
            return gl.diff(func_name, arg_num, args, n_args, res);
        }
    }

    return pos->second.make_subs(n_args, args, res);
};

void diff_context_impl::error_rule_defined(const symbol& func_name, size_t n_args,
                        const symbol* args, size_t diff_arg, const expr& dif, bool make_diff,
                        const diff_rule& prev_rule)
{
    error::error_formatter ef;
    ef.head() << "differentiation rule already defined";

    auto l1 = ef.new_info();

    ef.line() << "current definition:";
    ef.new_line(l1);
        disp_diff_def(ef.line(), func_name, n_args, args, diff_arg, dif, make_diff);

    auto l2 = ef.new_info();
    ef.line() << "previous definition:";
    ef.new_line(l1);
    {
        std::vector<symbol> rule_args;
        prev_rule.get_function_args(rule_args);

        bool make_diff_prev;
        expr dif_prev = prev_rule.get_diff_result(make_diff_prev);

        disp_diff_def(ef.line(), func_name, rule_args.size(), rule_args.data(), 
                      diff_arg, dif_prev, make_diff_prev);
    }

    throw std::runtime_error(ef.str());
};

void diff_context_impl::disp_diff_def(std::ostream& os, const symbol& func_name, size_t n_args,
                    const symbol* args, size_t diff_arg, const expr& dif, bool make_dif)
{    
    os << "d/d"; disp(os, args[diff_arg], false);
    os << " ";
    disp_function(os, func_name, n_args, args);
    os << " -> ";

    if (make_dif == true)
        disp(os, dif, false);
    else
        os << "none";
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

//-------------------------------------------------------------------
//                        diff_context_global
//-------------------------------------------------------------------

static std::vector<diff_context_initializer*>&
get_initializers()
{
    static std::vector<diff_context_initializer*> g_initializers;

    return g_initializers;
}

diff_context_global::diff_context_global()
{
    initialize();
    m_impl->set_global();
}

void diff_context_global::initialize()
{
    std::vector<diff_context_initializer*>& init = get_initializers();

    for (auto pos : init)
        pos->initialize(*this);

    init = std::vector<diff_context_initializer*>();
}

void diff_context_global::register_initializer(diff_context_initializer* init)
{
    get_initializers().push_back(init);
}

//-------------------------------------------------------------------
//                        function_evaler_impl
//-------------------------------------------------------------------
function_evaler_impl::function_evaler_impl()
    :m_is_global(false)
{};

void function_evaler_impl::set_global()
{
    m_is_global = true;
}

bool function_evaler_impl::eval_function(const symbol& name, const value* subexpr, 
                    size_t n_size, value& res) const
{
    key_type key(name, n_size);
    auto pos = m_map_eval.find(key);

    if (pos == m_map_eval.end())
    {
        if (m_is_global == true)
            return false;

        function_evaler gl = global_function_evaler();
        return gl.eval_function(name, subexpr, n_size, res);
    };

    res = pos->second(n_size, subexpr);
    return true;
};

bool function_evaler_impl::eval_function(const symbol& name, const expr* subexpr, 
                    size_t n_size, expr& res) const
{
    bool all_values     = all_args_values(n_size, subexpr);

    if (all_values == true)
    {
        return eval_function_value(name, subexpr, n_size, res);
    }
    else
    {
        return eval_function_expr(name, subexpr, n_size, res);
    };
}

bool function_evaler_impl::eval_function_expr(const symbol& name, const expr* subexpr, 
                    size_t n_size, expr& res) const
{
    key_type key(name, n_size);
    auto pos = m_map_eval_partial.find(key);

    if (pos == m_map_eval_partial.end())
    {
        if (m_is_global == true)
            return false;

        function_evaler gl = global_function_evaler();
        return gl.eval_function(name, subexpr, n_size, res);
    };

    res = pos->second(n_size, subexpr);

    if (res.is_null() == true)
        return false;
    else
        return true;
};

bool function_evaler_impl::all_args_values(size_t n_args, const expr* subexpr)
{
    for (size_t i = 0; i < n_args; ++i)
    {
        if (subexpr[i].get_expr_handle()->isa<ast::scalar_rep>() == false)
            return false;
    }

    return true;
};

bool function_evaler_impl::eval_function_value(const symbol& name, const expr* subexpr, 
                    size_t n_size, expr& res) const
{
    key_type key(name, n_size);
    auto pos = m_map_eval.find(key);

    if (pos == m_map_eval.end())
    {
        if (m_is_global == true)
            return false;

        function_evaler gl = global_function_evaler();
        return gl.m_impl->eval_function_value(name, subexpr, n_size, res);
    };

    using value_pod     = pod_type<value>;
    int size_counter    = 0;
    value_pod::destructor_type d(&size_counter);
    stack_array<value_pod> buff(n_size, &d);    

    value* buff_ptr     = reinterpret_cast<value*>(buff.get());

    for(size_t i = 0; i < n_size; ++i)
    {
        // it is already checked, that all arguments are values
        value tmp       = subexpr[i].get_expr_handle()->static_cast_to<ast::scalar_rep>()->get_data();

        new(buff_ptr + size_counter) value(tmp);
        ++size_counter;
    };

    value v_res         = pos->second(n_size, buff_ptr);
    res                 = scalar(v_res);
    return true;
};

void function_evaler_impl::add_evaler(const symbol& func_name, size_t n_args,
                    const evaler_function& f)
{
    key_type key(func_name, n_args);
    auto pos    = m_map_eval.find(key);

    if (pos == m_map_eval.end())
    {
        m_map_eval.insert(pos, map_eval::value_type(key, f));
        return;
    }

    error_evaler_defined(func_name, n_args);
};

void function_evaler_impl::add_partial_evaler(const symbol& func_name, size_t n_args,
                    const evaler_function_partial& f)
{
    key_type key(func_name, n_args);
    auto pos    = m_map_eval_partial.find(key);

    if (pos == m_map_eval_partial.end())
    {
        m_map_eval_partial.insert(pos, map_eval_p::value_type(key, f));
        return;
    }

    error_partial_evaler_defined(func_name, n_args);
};

void function_evaler_impl::error_evaler_defined(const symbol& name, size_t n_size)
{
    error::error_formatter ef;
    ef.head() << "function evaler for symbol " << name.get_name() << " with " << n_size 
              << " arguments is already defined";

    throw std::runtime_error(ef.str());
}

void function_evaler_impl::error_partial_evaler_defined(const symbol& name, size_t n_size)
{
    error::error_formatter ef;
    ef.head() << "partial function evaler for symbol " << name.get_name() << " with " << n_size 
              << " arguments is already defined";

    throw std::runtime_error(ef.str());
}


//-------------------------------------------------------------------
//                        function_evaler_global
//-------------------------------------------------------------------

static std::vector<function_evaler_initializer*>&
get_function_initializers()
{
    static std::vector<function_evaler_initializer*> g_initializers;

    return g_initializers;
}

function_evaler_global::function_evaler_global()
{
    initialize();
    m_impl->set_global();
}

void function_evaler_global::initialize()
{
    std::vector<function_evaler_initializer*>& init = get_function_initializers();

    for (auto pos : init)
        pos->initialize(*this);

    init = std::vector<function_evaler_initializer*>();
}

void function_evaler_global::register_initializer(function_evaler_initializer* init)
{
    get_function_initializers().push_back(init);
}

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
    m_impl->m_set       = m_impl->m_set.set(sym.get_ptr()->get_indexed_symbol_code());
};

void subs_context::remove_symbol(const symbol& sym)
{
    remove_bind();

    m_impl->m_map.erase(sym);
    m_impl->m_set = m_impl->m_set.reset(sym.get_ptr()->get_indexed_symbol_code());
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

    if (m_impl->m_bind == nullptr)
    {
        expr ex     = m_impl->m_buffer[code];
        return ex;
    }
    else
    {
        return m_impl->m_bind[code];
    };
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

diff_context::diff_context()
    :m_impl(new sym_arrow::details::diff_context_impl())
{};

bool diff_context::diff(const symbol& func_name, size_t arg_num, const expr* args,
                    size_t n_args, expr& res)
{
    return m_impl->diff(func_name, arg_num, args, n_args, res);
};

void diff_context::add_diff_rule(const symbol& func_name, size_t n_args,
                    const symbol* args, size_t diff_arg, const expr& dif,
                    bool make_diff)
{
    return m_impl->add_diff_rule(func_name, n_args, args, diff_arg, dif, make_diff);
};

// global instance
details::diff_context_global* g_diff_context = nullptr;

const diff_context& sym_arrow::global_diff_context()
{
    if (!g_diff_context)
    {
        //force initialization
        details::initialize_global_contexts();
    }

    return *g_diff_context;
};

void diff_context_initializer::register_initializer(diff_context_initializer* init)
{
    if (g_diff_context != nullptr)
    {
        init->initialize(*g_diff_context);
        return;
    }

    details::diff_context_global::register_initializer(init);
}

//-------------------------------------------------------------------
//                        function_evaler
//-------------------------------------------------------------------

function_evaler::function_evaler()
    :m_impl(new sym_arrow::details::function_evaler_impl())
{};

bool function_evaler::eval_function(const symbol& name, const value* subexpr, 
                    size_t n_size, value& res) const
{
    return m_impl->eval_function(name, subexpr, n_size, res);
};

bool function_evaler::eval_function(const symbol& name, const expr* subexpr, 
                    size_t n_size, expr& res) const
{
    return m_impl->eval_function(name, subexpr, n_size, res);
};

void function_evaler::add_evaler(const symbol& func_name, size_t n_args,
                    const evaler_function& f)
{
    return m_impl->add_evaler(func_name, n_args, f);
}

void function_evaler::add_partial_evaler(const symbol& func_name, size_t n_args,
                    const partial_evaler_function& f)
{
    return m_impl->add_partial_evaler(func_name, n_args, f);
}

// global instance
details::function_evaler_global* g_func_evaler = nullptr;

const function_evaler& sym_arrow::global_function_evaler()
{
    if (!g_func_evaler)
    {
        //force initialization
        details::initialize_global_contexts();
    }
    
    return *g_func_evaler;
};

void function_evaler_initializer::register_initializer(function_evaler_initializer* init)
{
    if (g_func_evaler != nullptr)
    {
        init->initialize(*g_func_evaler);
        return;
    }

    details::function_evaler_global::register_initializer(init);
}

};

namespace sym_arrow { namespace details
{

//-------------------------------------------------------------------
//                        initialize contexts
//-------------------------------------------------------------------

void details::initialize_global_contexts()
{
    g_func_evaler   = new details::function_evaler_global();
    g_diff_context  = new details::diff_context_global();
}

void details::close_global_contexts()
{
    delete g_diff_context;
    delete g_func_evaler;    
}

}};