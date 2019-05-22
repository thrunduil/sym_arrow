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

#pragma once

#include "sym_arrow/nodes/expr.h"
#include "dbs/dbs.h"
#include "sym_arrow/functions/contexts.h"
#include "sym_arrow/functions/global_context_initializers.h"

namespace sym_arrow { namespace details
{

//-------------------------------------------------------------------
//                        subs_context
//-------------------------------------------------------------------

class subs_context_impl
{
    public:
        using expr_vec      = std::vector<expr>;
        using sym_map       = std::map<symbol, size_t>;
        using dbs           = dbs_lib::dbs;

    public:
        subs_context_impl();

    private:
        dbs                 m_set;
        sym_map             m_map;
        expr_vec            m_buffer;
        const expr*         m_bind;

    public:
        void                add_symbol(const symbol& sym, const expr& ex);
        void                add_symbol(const symbol& sym, size_t code);
        size_t              add_symbol(const symbol& sym);

        void                remove_bind();
        void                set_bind(size_t size, const expr* ex);
        void                remove_symbol(const symbol& sym);

        const dbs_lib::dbs& get_symbol_set() const;
        size_t              size() const;
        size_t              bind_array_length() const;

        void                disp(std::ostream& os) const;
        expr                subs(const symbol& sh) const;
        void                visit_substitutions(substitution_vis& info) const;

    private:                   
        void                check_bind() const;
        void                check_substitution(const symbol& sym, const expr& ex) const;
};    

//-------------------------------------------------------------------
//                        diff_context
//-------------------------------------------------------------------

class diff_rule
{
    private:
        expr            m_expr;
        subs_context    m_subs;
        bool            m_make_dif;

    public:
        diff_rule(size_t n_args, const symbol* args, const expr& dif, bool make_dif);

        const expr&     get_diff_result(bool& make_dif) const;
        size_t          number_args() const;
        void            get_function_args(std::vector<symbol>& rule_args) const;
        bool            make_subs(size_t n_args, const expr* args, expr& res);
};

class diff_context_impl
{
    private:
                        // function_name x number args x arg number
        using key       = std::tuple<identifier, size_t, size_t>;
        using value     = diff_rule;

        using diff_map  = std::map<key, value>;

    private:
        diff_map        m_diff_map;
        bool            m_is_global;

    public:
        diff_context_impl();

        bool            diff(const identifier& func_name, size_t arg_num, const expr* args,
                            size_t n_args, expr& res);
        void            add_diff_rule(const identifier& func_name, size_t n_args,
                            const symbol* args, size_t diff_arg, const expr& dif,
                            bool make_diff);

        // mark this context as global
        void            set_global();

    private:
        void            error_rule_defined(const identifier& func_name, size_t n_args,
                            const symbol* args, size_t diff_arg, const expr& dif, bool make_diff,
                            const diff_rule& prev_rule);
        void            disp_diff_def(std::ostream& os, const identifier& func_name, size_t n_args,
                            const symbol* args, size_t diff_arg, const expr& dif, bool make_diff);
        void            disp_function(std::ostream& os, const identifier& func_name, size_t n_args, 
                            const symbol* args);
};

class diff_context_global : public diff_context
{
    public:
        // initialize global diff context
        diff_context_global();

        static void     register_initializer(diff_context_initializer* init);

    private:
        void            initialize();    
};

//-------------------------------------------------------------------
//                        function_evaler_impl
//-------------------------------------------------------------------
class function_evaler_impl
{
    public:
        // type of function evaler
        using evaler_function   = std::function<value (size_t n_args, const value* args)>;
        using evaler_function_partial
                                = std::function<expr (size_t n_args, const expr* args)>;

    private:
        using key_type  = std::pair<identifier, size_t>;
        using map_eval  = std::map<key_type, evaler_function>;
        using map_eval_p= std::map<key_type, evaler_function_partial>;

    private:
        map_eval        m_map_eval;
        map_eval_p      m_map_eval_partial;
        bool            m_is_global;

    public:
        // create empty mapping from functions to evaluators
        function_evaler_impl();

        // mark this context as global
        void            set_global();

        // evaluate a function with n_size arguments evaluated
        // to values stored in the array subexpr; return true 
        // if function was evaluated
        bool            eval_function(const identifier& name, const value* subexpr, 
                            size_t n_size, value& res) const;

        // evaluate a function with n_size arguments stored in the
        // array subexpr; return true if function was evaluated,
        bool            eval_function(const identifier& name, const expr* subexpr, 
                            size_t n_size, expr& res) const;

        // register an evaler for function with name func_name and
        // n_args arguments
        void            add_evaler(const identifier& func_name, size_t n_args,
                            const evaler_function& f);

        void            add_partial_evaler(const identifier& func_name, size_t n_args,
                            const evaler_function_partial& f);

    private:
        static bool     all_args_values(size_t n_size, const expr* subexpr);

        bool            eval_function_value(const identifier& name, const expr* subexpr, 
                            size_t n_size, expr& res) const;
        bool            eval_function_expr(const identifier& name, const expr* subexpr, 
                            size_t n_size, expr& res) const;

        void            error_evaler_defined(const identifier& name, size_t n_size);
        void            error_partial_evaler_defined(const identifier& name, size_t n_size);
};

class function_evaler_global : public function_evaler
{
    public:
        // initialize global diff context
        function_evaler_global();

        static void     register_initializer(function_evaler_initializer* init);

    private:
        void            initialize();    
};

void initialize_global_contexts();
void close_global_contexts();

}};
