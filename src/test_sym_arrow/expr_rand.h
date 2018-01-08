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

#include "sym_arrow/sym_arrow.h"
#include "error_value.h"

#include <vector>
#include <map>

namespace sym_arrow { namespace testing
{

struct expr_value_dif
{
    private:
        expr                m_base;
        expr                m_dif;
        error_value         m_base_val;
        error_value         m_dif_val;

    public:
        expr_value_dif();
        expr_value_dif(const expr& base, const error_value& val_base,
                       const expr& dif, const error_value& val_dif);

        const expr&         get_base() const;
        const expr&         get_dif() const;
        const error_value&  get_base_value() const;
        const error_value&  get_dif_value() const;
        bool                is_precise(double ulp_limit) const;
};

struct rand_state
{
    private:
        using expr_value    = std::pair<expr,error_value>;
        using symbol_value  = std::pair<symbol,error_value>;
        using expr_vec      = std::vector<expr_value>;                
        using expr_dif_vec  = std::vector<expr_value_dif>;
        using symbol_vec    = std::vector<symbol_value>;
        using symbol_map    = std::map<symbol, value>;

    private:
        int                 n_sym;
        int                 n_depth;
        int                 n_scalars;
        expr_vec            m_vec;
        expr_dif_vec        m_vec_dif;
        symbol_vec          m_symbols;        
        expr_vec            m_scalars;
        symbol_map          m_symbol_map;
        symbol_vec          m_functions_names;
        bool                m_disp;
        bool                m_test_value;

    private:
        rand_state& operator=(const rand_state&) = delete;                

        void            init_symbols();
        void            init_functions();
        expr_value      rand_scalar();        
        value           rand_value_special();
        int             rand_int();
        expr_value      rand_symbol();
        expr_value      rand_atom();
        expr_value      rand_existing();        
        bool            add(const expr_value&);

        expr_value_dif  rand_atom_dif(const symbol& sym);
        expr_value_dif  rand_scalar_dif();
        expr_value_dif  rand_symbol_dif(const symbol& sym);
        expr_value_dif  rand_existing_dif();

        bool            add_dif(const expr_value_dif&, const symbol& sym);
        bool            test_existing_dif(const expr_value_dif&, const symbol& sym);

        expr_value_dif  sum_dif(const expr_value_dif& x1, const expr_value_dif& x2);
        expr_value_dif  minus_dif(const expr_value_dif& x1, const expr_value_dif& x2);
        expr_value_dif  mult_dif(const expr_value_dif& x1, const expr_value_dif& x2);
        expr_value_dif  div_dif(const expr_value_dif& x1, const expr_value_dif& x2);
        expr_value_dif  pow_int_dif(const expr_value_dif& x1, int pow);
        expr_value_dif  pow_real_dif(const expr_value_dif& x1, const expr_value_dif& x2);
        expr_value_dif  uminus_dif(const expr_value_dif& x1);
        expr_value_dif  exp_dif(const expr_value_dif& x1);
        expr_value_dif  log_dif(const expr_value_dif& x1);
        expr_value_dif  abs_dif(const expr_value_dif& x1);

        bool            test_input_exp(const error_value& val) const;
        bool            test_input_pow(const error_value& val1, const error_value& val2) const;
        bool            test_input_pow(const error_value& val1, int val2) const;
        bool            test_input_mult(const error_value& val1, const error_value& val2) const;
        bool            test_input_div(const error_value& val1, const error_value& val2) const;

    public:
        rand_state(int n_sym, int max_depth, bool disp, bool test_value);        
        ~rand_state();

        rand_state(const rand_state& r);

        expr_value      rand_expr(int depth, bool show = false);
        expr_value_dif  rand_expr_dif(int depth, const symbol& sym, bool show = false);
        symbol          rand_sym();

        value           get_symbol_value(const symbol& sh) const;
        rand_state      set_symbol_value(const symbol& sh, value v) const;

        void            disp(std::ostream& os) const;

        void            set_test_value(bool tv);
        static bool&    get_debug_flag();

        static bool     values_equal(const error_value& v1, const value& v2, 
                            double& ulp_error);

    private:        
        void            disp(std::ostream& os, const expr_value& ex);
        void            disp(std::ostream& os, const expr_value_dif& ex);
        value           rand_value();                                
        static double   ulp_limit();
        static double   ulp_limit_prec();
};

using expr_value    = std::pair<expr,error_value>;

double      rand();
double      randn();
int         irand();
expr_value  expr_rand(int n_sym, int max_depth, bool d, bool test_value);
void        init_genrand(unsigned long s);

class rand_data_provider : public sym_arrow::data_provider
{
    private:
        const rand_state*   m_rand;

    public:
        rand_data_provider(const rand_state* rs);

        virtual value   get_value(const symbol& sh) const override;
        virtual value   eval_function(const symbol&, const value*, size_t nargs) const override;

        void            set_value(const symbol& sh, value v);
        void            disp(std::ostream& os) const;
};

};};
