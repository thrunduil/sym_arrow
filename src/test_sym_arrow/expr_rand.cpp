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

#include "expr_rand.h"
#include <sstream>
#include "sym_arrow/sym_arrow.h"
#include "rand.h"

#include <iomanip>

namespace sym_arrow { namespace testing
{

rand_data_provider::rand_data_provider(const rand_state* rs)
    : m_rand(rs)
{};

value rand_data_provider::get_value(const symbol& sh) const 
{ 
    return m_rand->get_symbol_value(sh); 
};

value rand_data_provider::eval_function(const symbol&, const value*, size_t) const
{ 
    return value::make_zero(); 
};

void rand_data_provider::set_value(const symbol& sh, value v)
{ 
    m_rand->set_symbol_value(sh, v); 
};

void rand_data_provider::disp(std::ostream& os) const
{ 
    m_rand->disp(os); 
};

expr_value_dif::expr_value_dif()
    :m_base(scalar::make_zero()), m_dif(scalar::make_zero())
    , m_base_val(value::make_zero()), m_dif_val(value::make_zero())
{};

expr_value_dif::expr_value_dif(const expr& base, const error_value& val_base,
                const expr& dif, const error_value& val_dif)
    :m_base(base), m_dif(dif), m_base_val(val_base), m_dif_val(val_dif)
{};

const expr& expr_value_dif::get_base() const
{
    return m_base;
}

const expr& expr_value_dif::get_dif() const
{
    return m_dif;
}

const error_value& expr_value_dif::get_base_value() const
{
    return m_base_val;
}

const error_value& expr_value_dif::get_dif_value() const
{
    return m_dif_val;
}

bool expr_value_dif::is_precise(double ulp_limit) const
{
    bool v1 = m_base_val.is_precise(ulp_limit);
    bool v2 = m_dif_val.is_precise(ulp_limit);

    return v1 && v2;
}


rand_state::rand_state(const rand_state& r)
    : n_sym(r.n_sym), n_scalars(r.n_scalars), n_depth(r.n_depth)
    , m_disp(r.m_disp), m_test_value(r.m_test_value), m_vec(r.m_vec)
    , m_symbols(r.m_symbols), m_scalars(r.m_scalars), m_symbol_map(r.m_symbol_map)
    , m_functions_names(r.m_functions_names)
{
    // make a copy (default object is const)
}

rand_state::rand_state(int sym, int max_depth, bool disp, bool test_value)
    : m_disp(disp), m_test_value(test_value)
{
    m_vec.reserve(100000);

    n_sym       = (sym > 1) ? sym : 1;
    n_scalars   = 20;
    n_depth     = (max_depth > 0) ? max_depth : 0;

    m_vec.push_back(expr_value(scalar::make_one(), value::make_one()));
    
    init_symbols();
    init_functions();
};

rand_state::~rand_state()
{};

void rand_state::init_symbols()
{
    for (int i = 1; i <= n_sym; ++i)
    {
        std::ostringstream os;
        os << "x" << i;
        symbol s(os.str().c_str());

        value v = rand_value();
        
        m_symbols.push_back(symbol_value(s, v));
        m_symbol_map.insert(symbol_map::value_type(s, v));
    };

    for (int i = 1; i <= n_scalars; ++i)
    {
        value tmp = rand_value();
        scalar sc(tmp);

        expr_value sc_val(sc,tmp);
        m_scalars.push_back(sc_val);
    };
};

void rand_state::init_functions()
{
    /*
    symbol s(c,"sin");
    m_functions_names.push_back(s);
    symbol s1(c,"cos");
    m_functions_names.push_back(s1);
    symbol s2(c,"tan");
    m_functions_names.push_back(s2);
    symbol s3(c,"ctan");
    m_functions_names.push_back(s3);
    */
};

double rand_state::ulp_limit()
{
    return 1000.0;
};

double rand_state::ulp_limit_prec()
{
    return 100.0;
};

bool rand_state::add(const expr_value& ex)
{
    bool add_item    = rand() < .1 && ex.second.is_precise(ulp_limit_prec());

    bool ret        = true;

    if (m_test_value)
    {
        error_value v1  = ex.second;
        expr tmp        = ex.first;
        value v2        = eval(tmp, rand_data_provider(this));
        double ulp_err;

        if (values_equal(v1,v2, ulp_err) == false)
        {
            if (get_debug_flag() == true)
            {
                disp(std::cout, ex);
                sym_arrow::disp(std::cout, tmp);
                expr tmp2   = ex.first;
                sym_arrow::eval(tmp2, rand_data_provider(this));
            };

            ret        = false;
        };
    };

    if (add_item && ret == true)
    {
        m_vec.push_back(ex);
    };

    return ret;
};

bool rand_state::test_existing_dif(const expr_value_dif& ex, const symbol& sym)
{
    bool ret            = true;

    if (m_test_value)
    {
        error_value v1  = ex.get_base_value();
        expr tmp        = ex.get_base();
        value v2        = eval(tmp, rand_data_provider(this));
        double ulp_err;

        if (values_equal(v1,v2, ulp_err) == false)
        {
            if (get_debug_flag() == true)
            {
                disp(std::cout, ex);
                sym_arrow::disp(std::cout, tmp);
                expr tmp2   = ex.get_base();
                sym_arrow::eval(tmp2, rand_data_provider(this));
            };

            ret        = false;
        };

        error_value vd1 = ex.get_dif_value();
        expr tmp_d      = ex.get_dif();
        value vd2       = eval(tmp_d, rand_data_provider(this));

        if (values_equal(vd1, vd2, ulp_err) == false)
        {
            if (get_debug_flag() == true)
            {
                disp(std::cout, ex);
                
                std::cout << std::setprecision(17);
                sym_arrow::disp_nocannonize(std::cout, ex.get_dif());
                sym_arrow::disp(std::cout, ex.get_dif());

                expr tmp2   = ex.get_dif();
                sym_arrow::eval(tmp2, rand_data_provider(this));
            };

            ret        = false;
        };

        expr tmp_d2     = diff(ex.get_base(), sym);
        value vd3       = eval(tmp_d2, rand_data_provider(this));

        if (values_equal(vd1, vd3, ulp_err) == false)
        {
            if (get_debug_flag() == true)
            {
                disp(std::cout, ex);

                sym_arrow::disp_nocannonize(std::cout, ex.get_base());
                sym_arrow::disp(std::cout, ex.get_base());
                sym_arrow::disp(std::cout, tmp_d2);

                sym_arrow::eval(tmp_d2, rand_data_provider(this));
            };

            ret        = false;
        };
    };

    return ret;
}

bool rand_state::add_dif(const expr_value_dif& ex, const symbol& sym)
{
    bool add_item    = rand() < .1 && ex.is_precise(ulp_limit_prec());
    bool ret        = test_existing_dif(ex, sym);

    if (add_item && ret == true)
    {
        m_vec_dif.push_back(ex);
    };

    return ret;
};


rand_state::expr_value rand_state::rand_existing()
{
    if (m_vec.size() == 0)
        return rand_atom();

    int t   = irand();
    t       = (t < 0)? -t:t;
    t       = t % m_vec.size();
    return m_vec[t];
};
 
expr_value_dif rand_state::rand_existing_dif()
{
    if (m_vec_dif.size() == 0)
        return rand_scalar_dif();

    int t   = irand();
    t       = (t < 0)? -t:t;
    t       = t % m_vec_dif.size();
    return m_vec_dif[t];
};
 

rand_state::expr_value rand_state::rand_atom()
{
    int t   = irand();
    t       = (t < 0)? -t : t;

    if (t % 2 == 0)
    {
        expr_value s = rand_scalar();
        return s;
    }
    else
    {
        return rand_symbol();
    };
};

expr_value_dif rand_state::rand_atom_dif(const symbol& sym)
{
    int t   = irand();
    t       = (t < 0)? -t : t;

    if (t % 2 == 0)
    {
        expr_value_dif s = rand_scalar_dif();
        return s;
    }
    else
    {
        return rand_symbol_dif(sym);
    };
};

value rand_state::rand_value()
{
    int t   = irand();
    t       = (t < 0)? -t : t;

    if (t % 5 == 0)
        return rand_value_special();

    double tmp = randn();
    return value::make_value(tmp);
};

rand_state::expr_value rand_state::rand_scalar()
{
    int t   = irand();
    t       = (t < 0)? -t : t;

    return m_scalars[t % n_scalars];
};

expr_value_dif rand_state::rand_scalar_dif()
{
    int t   = irand();
    t       = (t < 0)? -t : t;

    expr_value ex = m_scalars[t % n_scalars];
    return expr_value_dif(ex.first, ex.second, scalar::make_zero(), 
                          error_value(value::make_zero()));
};

int rand_state::rand_int()
{
    return (int)(6*rand()) - 3;
};

value rand_state::rand_value_special()
{
    int t   = irand();
    t       = (t < 0)? -t : t;

    switch (t % 5)
    {
        case 0:     return value::make_zero();
        case 1:     return value::make_one();
        case 2:     return value::make_minus_one();
        case 3:     return value::make_value(2.0);
        case 4:     return value::make_value(-2.0);
        default:
        {
            assertion(0,"unknown case");
            throw;
        }
    };
};

rand_state::expr_value rand_state::rand_symbol()
{
    int t   = irand();
    t       = (t < 0)? -t : t;
    return m_symbols[t % n_sym];
};

expr_value_dif rand_state::rand_symbol_dif(const symbol& sym)
{
    int t   = irand();
    t       = (t < 0)? -t : t;

    symbol_value ex = m_symbols[t % n_sym];

    if (ex.first == sym)
    {
        return expr_value_dif(ex.first, ex.second, scalar::make_one(), 
                          error_value(value::make_one()));
    }
    else
    {
        return expr_value_dif(ex.first, ex.second, scalar::make_zero(), 
                          error_value(value::make_zero()));
    };
};

value rand_state::get_symbol_value(const symbol& sh) const
{
    auto it = m_symbol_map.find(sh);

    if (it == m_symbol_map.end())
    {
        std::ostringstream msg;
        sym_arrow::disp_nocannonize(msg, sh, false);

        msg << " has no assigned value";
        throw std::runtime_error(msg.str());
    };

    return it->second;
};

rand_state rand_state::set_symbol_value(const symbol& sh, value v) const
{
    rand_state r(*this);

    auto it = r.m_symbol_map.find(sh);

    if (it == r.m_symbol_map.end())
    {
        std::ostringstream msg;
        sym_arrow::disp_nocannonize(msg, sh, false);

        msg << " has no assigned value";
        throw std::runtime_error(msg.str());
    };

    it->second = v;
    return r;
};

symbol rand_state::rand_sym()
{
    int t   = irand();
    t       = (t < 0)? -t : t;

    std::ostringstream os;
    os << "x" << t % n_sym;
    return symbol(os.str().c_str());
}

rand_state::expr_value rand_state::rand_expr(int depth, bool show)
{
    static int counter = 0;
    ++counter;

    if (depth >= n_depth)
        return rand_atom();

    int t  = irand();
    t       = (t < 0)? -t : t;

    switch(t % 11)
    {
        case 0: //scalar
        {
            if (show)
                std::cout << "\nscalar\n";

            return rand_scalar();
        }
        case 1: //symbol
        {
            if (show)
                std::cout << "\nsymbol\n";

            return rand_symbol();
        }
        case 2: //existing
        {
            expr_value x1 = rand_existing();

            if (show)
            {
                std::cout << "\nexisting\n";
                disp(std::cout, x1);
            };

            return x1;
        }
        case 3: //plus
        {
            expr_value x1 = rand_expr(depth + 1, show);
            expr_value x2 = rand_expr(depth + 1, show);

            if (show)
            {
                std::cout << "\n" << "plus 1: ";
                disp(std::cout, x1);

                std::cout << "plus 2: ";
                disp(std::cout, x2);
            };

            expr_value ret(std::move(x1.first) + std::move(x2.first),x1.second + x2.second);
            
            if (show)
            {
                std::cout << "plus ret: ";
                disp(std::cout, ret);
            }

            bool valid = add(ret);

            if (ret.second.is_precise(ulp_limit_prec()) == false)
                return rand_expr(depth + 1, show);

            (void)valid;
            return ret;
        }
        case 4: //minus
        {            
            expr_value x1 = rand_expr(depth+1,show);
            expr_value x2 = rand_expr(depth+1,show);

            if (show)
            {
                std::cout << "\n" << "minus 1: ";
                disp(std::cout, x1);

                std::cout << "minus 2: ";
                disp(std::cout, x2);
            }

            expr_value ret(std::move(x1.first) - std::move(x2.first), x1.second - x2.second);

            if (show)
            {
                std::cout << "minus ret: ";
                disp(std::cout, ret);
            }

            bool valid  = add(ret);

            if (ret.second.is_precise(ulp_limit_prec()) == false)
                return rand_expr(depth + 1, show);

            (void)valid;
            return ret;
        }
        case 5: //mult
        {
            expr_value x1 = rand_expr(depth + 1, show);
            expr_value x2 = rand_expr(depth + 1, show);

            if (show)
            {
                std::cout << "\n" << "mult 1: ";
                disp(std::cout, x1);

                std::cout << "mult 2: ";
                disp(std::cout, x2);
            }

            if (test_input_mult(x1.second, x2.second) == false)
                return rand_expr(depth + 1, show);

            expr_value ret(std::move(x1.first) * std::move(x2.first), x1.second * x2.second);

            if (show)
            {
                std::cout << "mult ret: ";
                disp(std::cout, ret);
            }

            bool valid  = add(ret);

            if (ret.second.is_precise(ulp_limit_prec()) == false)
                return rand_expr(depth + 1,show);

            (void)valid;
            return ret;
        }
        case 6: //div
        {
            expr_value x1 = rand_expr(depth + 1,show);
            expr_value x2 = rand_expr(depth + 1,show);

            if (show)
            {
                std::cout << "\n" << "div 1: ";
                disp(std::cout, x1);

                std::cout << "div 2: ";
                disp(std::cout, x2);
            }

            if (test_input_div(x1.second, x2.second) == false)
                return rand_expr(depth + 1, show);

            error_value div_v = x1.second / x2.second;
            expr_value ret(std::move(x1.first) / std::move(x2.first), div_v);

            if (show)
            {
                std::cout << "div ret: ";
                disp(std::cout, ret);
            }

            bool valid  = add(ret);

            if (ret.second.is_precise(ulp_limit_prec()) == false)
                return rand_expr(depth + 1,show);

            (void)valid;
            return ret;
        }
        case 7: //pow
        {
            expr_value x1    = rand_expr(depth + 1,show);

            if (show)
            {
                std::cout << "\n" << "pow 1: ";
                disp(std::cout, x1);
            }
            
            int t2  = irand();
            t2      = (t2 < 0)? -t2 : t2;

            switch (t2 % 3)
            {
                case 0:
                {                    
                    int x2            = rand_int();

                    if (show)
                    {
                        std::cout << "\npow0 2: ";
                        std::cout << x2;
                        std::cout << "\n";
                    }
                    
                    if (test_input_pow(x1.second, x2) == false)
                        return rand_expr(depth + 1,show);

                    expr_value ret(power_int(std::move(x1.first), x2), power_int(x1.second, x2));

                    if (show)
                    {
                        std::cout << "pow0 ret: ";
                        disp(std::cout, ret);
                    }

                    bool valid  = add(ret);

                    if (ret.second.is_precise(ulp_limit_prec()) == false)
                        return rand_expr(depth + 1,show);

                    (void)valid;
                    return ret;
                }
                case 1:                   
                {
                    expr_value x2   = this->rand_scalar();

                    if (show)
                    {
                        std::cout << "\n" << "pow1 2: ";
                        disp(std::cout, x2);
                    }

                    if (test_input_pow(x1.second, x2.second) == false)
                        return rand_expr(depth + 1, show);

                    expr_value ret(power_real(std::move(x1.first),std::move(x2.first)),
                                   power_real(x1.second,x2.second));

                    if (show)
                    {
                        std::cout << "pow1 ret: ";
                        disp(std::cout, ret);
                    }

                    bool valid  = add(ret);

                    if (ret.second.is_precise(ulp_limit_prec()) == false)
                        return rand_expr(depth+1,show);

                    (void)valid;
                    return ret;
                }
                case 2:
                {
                    expr_value x2 = rand_expr(depth + 1, show);

                    if (show)
                    {
                        std::cout << "\n" << "pow2 2: ";
                        disp(std::cout, x2);
                    }

                    if (test_input_pow(x1.second, x2.second) == false)
                        return rand_expr(depth + 1, show);

                    expr_value ret(power_real(std::move(x1.first),std::move(x2.first)),
                                   power_real(x1.second,x2.second));

                    if (show)
                    {
                        std::cout << "pow2 ret: ";
                        disp(std::cout, ret);
                    }

                    bool valid  = add(ret);

                    if (ret.second.is_precise(ulp_limit_prec()) == false)
                        return rand_expr(depth + 1, show);

                    (void)valid;
                    return ret;
                }
            };
        }
        case 8: //uminus
        {
            expr_value x1 = rand_expr(depth+1,show);
            expr_value ret(-std::move(x1.first), -x1.second);

            if (show)
            {
                std::cout << "\n" << "uminus: ";
                disp(std::cout, ret);
            }

            bool valid  = add(ret);

            if (ret.second.is_precise(ulp_limit_prec()) == false)
                return rand_expr(depth + 1, show);

            (void)valid;
            return ret;
        }
        case 9: // exp
        {
            expr_value x1 = rand_expr(depth + 1, show);

            if (test_input_exp(x1.second) == false)
                return rand_expr(depth + 1, show);

            if (show)
            {
                std::cout << "\n" << "exp 1: ";
                disp(std::cout, x1);
            }

            expr_value ret(exp(std::move(x1.first)), exp(x1.second));

            if (show)
            {
                std::cout << "exp ret: ";
                disp(std::cout, ret);
            }

            bool valid  = add(ret);

            (void)valid;
            return ret;
        }
        case 10: // log
        {
            expr_value x1 = rand_expr(depth + 1, show);

            if (show)
            {
                std::cout << "\n" << "log 1: ";
                disp(std::cout, x1);
            }

            expr_value ret(log(std::move(x1.first)), log(x1.second));

            if (show)
            {
                std::cout << "log ret: ";
                disp(std::cout, ret);
            }

            bool valid  = add(ret);

            if (ret.second.is_precise(ulp_limit_prec()) == false)
                return rand_expr(depth + 1, show);

            (void)valid;
            return ret;
        }

        default:
        {
            assertion(0,"unknown case");
            throw;
        }
    };
};

//
expr_value_dif rand_state::rand_expr_dif(int depth, const symbol& sym, bool show)
{
    static int counter = 0;
    ++counter;

    if (depth >= n_depth)
        return rand_atom_dif(sym);

    int t  = irand();
    t       = (t < 0)? -t : t;

    switch(t % 11)
    {
        case 0: //scalar
        {
            if (show)
                std::cout << "\nscalar\n";

            return rand_scalar_dif();
        }
        case 1: //symbol
        {
            if (show)
                std::cout << "\nsymbol\n";

            return rand_symbol_dif(sym);
        }
        case 2: //existing
        {
            expr_value_dif x1 = rand_existing_dif();

            bool valid = test_existing_dif(x1, sym);
            (void)valid;

            if (show)
            {
                std::cout << "\nexisting\n";
                disp(std::cout, x1);
            };

            return x1;
        }
        case 3: //plus
        {
            expr_value_dif x1 = rand_expr_dif(depth + 1, sym, show);
            expr_value_dif x2 = rand_expr_dif(depth + 1, sym, show);

            if (show)
            {
                std::cout << "\n" << "plus 1: ";
                disp(std::cout, x1);

                std::cout << "plus 2: ";
                disp(std::cout, x2);
            };

            expr_value_dif ret = sum_dif(x1, x2);
            
            if (show)
            {
                std::cout << "plus ret: ";
                disp(std::cout, ret);
            }

            bool valid = add_dif(ret, sym);

            if (ret.is_precise(ulp_limit_prec()) == false)
                return rand_expr_dif(depth + 1, sym, show);

            (void)valid;
            return ret;
        }
        case 4: //minus
        {            
            expr_value_dif x1 = rand_expr_dif(depth+1, sym, show);
            expr_value_dif x2 = rand_expr_dif(depth+1, sym, show);

            if (show)
            {
                std::cout << "\n" << "minus 1: ";
                disp(std::cout, x1);

                std::cout << "minus 2: ";
                disp(std::cout, x2);
            }

            expr_value_dif ret = minus_dif(x1, x2);

            if (show)
            {
                std::cout << "minus ret: ";
                disp(std::cout, ret);
            }

            bool valid  = add_dif(ret, sym);

            if (ret.is_precise(ulp_limit_prec()) == false)
                return rand_expr_dif(depth + 1, sym, show);

            (void)valid;
            return ret;
        }
        case 5: //mult
        {
            expr_value_dif x1 = rand_expr_dif(depth + 1, sym, show);
            expr_value_dif x2 = rand_expr_dif(depth + 1, sym, show);

            if (show)
            {
                std::cout << "\n" << "mult 1: ";
                disp(std::cout, x1);

                std::cout << "mult 2: ";
                disp(std::cout, x2);
            }

            expr_value_dif ret = mult_dif(x1, x2);

            if (show)
            {
                std::cout << "mult ret: ";
                disp(std::cout, ret);
            }

            bool valid  = add_dif(ret, sym);

            if (ret.is_precise(ulp_limit_prec()) == false)
                return rand_expr_dif(depth + 1, sym, show);

            (void)valid;
            return ret;
        }
        case 6: //div
        {
            expr_value_dif x1 = rand_expr_dif(depth + 1, sym, show);
            expr_value_dif x2 = rand_expr_dif(depth + 1, sym, show);

            if (show)
            {
                std::cout << "\n" << "div 1: ";
                disp(std::cout, x1);

                std::cout << "div 2: ";
                disp(std::cout, x2);
            }

            expr_value_dif ret = div_dif(x1, x2);

            if (show)
            {
                std::cout << "div ret: ";
                disp(std::cout, ret);
            }

            bool valid  = add_dif(ret, sym);

            if (ret.is_precise(ulp_limit_prec()) == false)
                return rand_expr_dif(depth + 1, sym, show);

            (void)valid;
            return ret;
        }
        case 7: //pow
        {
            expr_value_dif x1    = rand_expr_dif(depth + 1, sym, show);

            if (show)
            {
                std::cout << "\n" << "pow 1: ";
                disp(std::cout, x1);
            }
            
            int t2  = irand();
            t2      = (t2 < 0)? -t2 : t2;

            switch (t2 % 3)
            {
                case 0:
                {                    
                    int x2            = rand_int();

                    if (show)
                    {
                        std::cout << "\npow0 2: ";
                        std::cout << x2;
                        std::cout << "\n";
                    }
                    
                    expr_value_dif ret  = pow_int_dif(x1, x2);

                    if (show)
                    {
                        std::cout << "pow0 ret: ";
                        disp(std::cout, ret);
                    }

                    bool valid  = add_dif(ret, sym);

                    if (ret.is_precise(ulp_limit_prec()) == false)
                        return rand_expr_dif(depth + 1, sym, show);

                    (void)valid;
                    return ret;
                }
                case 1:                   
                {
                    expr_value_dif x2   = this->rand_scalar_dif();

                    if (show)
                    {
                        std::cout << "\n" << "pow1 2: ";
                        disp(std::cout, x2);
                    }

                    expr_value_dif ret = pow_real_dif(x1, x2);

                    if (show)
                    {
                        std::cout << "pow1 ret: ";
                        disp(std::cout, ret);
                    }

                    bool valid  = add_dif(ret, sym);

                    if (ret.is_precise(ulp_limit_prec()) == false)
                        return rand_expr_dif(depth+1, sym, show);

                    (void)valid;
                    return ret;
                }
                case 2:
                {
                    expr_value_dif x2 = rand_expr_dif(depth + 1, sym, show);

                    if (show)
                    {
                        std::cout << "\n" << "pow2 2: ";
                        disp(std::cout, x2);
                    }

                    expr_value_dif ret = pow_real_dif(x1, x2);

                    if (show)
                    {
                        std::cout << "pow2 ret: ";
                        disp(std::cout, ret);
                    }

                    bool valid  = add_dif(ret, sym);

                    if (ret.is_precise(ulp_limit_prec()) == false)
                        return rand_expr_dif(depth + 1, sym, show);

                    (void)valid;
                    return ret;
                }
            };
        }
        case 8: //uminus
        {
            expr_value_dif x1   = rand_expr_dif(depth+1, sym, show);
            expr_value_dif ret  = uminus_dif(x1);

            if (show)
            {
                std::cout << "\n" << "uminus: ";
                disp(std::cout, ret);
            }

            bool valid  = add_dif(ret, sym);

            if (ret.is_precise(ulp_limit_prec()) == false)
                return rand_expr_dif(depth + 1, sym, show);

            (void)valid;
            return ret;
        }
        case 9: // exp
        {
            expr_value_dif x1 = rand_expr_dif(depth + 1, sym, show);

            if (show)
            {
                std::cout << "\n" << "exp 1: ";
                disp(std::cout, x1);
            }

            expr_value_dif ret  = exp_dif(x1);

            if (show)
            {
                std::cout << "exp ret: ";
                disp(std::cout, ret);
            }

            bool valid  = add_dif(ret, sym);

            (void)valid;
            return ret;
        }
        case 10: // log
        {
            expr_value_dif x1 = rand_expr_dif(depth + 1, sym, show);

            if (show)
            {
                std::cout << "\n" << "log 1: ";
                disp(std::cout, x1);
            }

            expr_value_dif ret  = log_dif(x1);

            if (show)
            {
                std::cout << "log ret: ";
                disp(std::cout, ret);
            }

            bool valid  = add_dif(ret, sym);

            if (ret.is_precise(ulp_limit_prec()) == false)
                return rand_expr_dif(depth + 1, sym, show);

            (void)valid;
            return ret;
        }

        default:
        {
            assertion(0,"unknown case");
            throw;
        }
    };
};

bool rand_state::test_input_exp(const error_value& val) const
{
    if (val.is_finite() == false)
        return false;

    return true;
};

bool rand_state::test_input_pow(const error_value& val1, const error_value& val2) const
{
    if (val1.is_finite() == false)
        return false;

    if (val2.is_finite() == false)
        return false;

    return true;
};

bool rand_state::test_input_pow(const error_value& val1, int val2) const
{
    if (val1.is_finite() == false)
        return false;

    (void)val2;

    return true;
};

bool rand_state::test_input_mult(const error_value& val1, const error_value& val2) const
{
    if (val1.is_finite() == false)
        return false;

    if (val2.is_finite() == false)
        return false;

    return true;
}

bool rand_state::test_input_div(const error_value& val1, const error_value& val2) const
{
    if (val1.is_finite() == false)
        return false;

    if (val2.is_finite() == false)
        return false;

    return true;
}

bool rand_state::values_equal(const error_value& v1, const value& v2, 
                              double& ulp_err)
{
    ulp_err = std::numeric_limits<double>::infinity();

    if (v1.is_value_finite() != v2.is_finite())
        return false;
  
    return v1.is_equal(v2, ulp_limit(), ulp_err);
};

void rand_state::set_test_value(bool tv)
{
    m_test_value = tv;
};

void rand_state::disp(std::ostream& os) const
{
    for (auto iter = m_symbol_map.begin(); iter != m_symbol_map.end(); ++iter) 
    {
        sym_arrow::disp_nocannonize(os, iter->first);
        os << ": ";
        sym_arrow::disp(os, iter->second);
        os << "\n";
    }
};

void rand_state::disp(std::ostream& os, const expr_value& ex)
{
    sym_arrow::disp_nocannonize(os, ex.first, false);
    os << " : ";
    testing::disp(os, ex.second, true);
};

void rand_state::disp(std::ostream& os, const expr_value_dif& ex)
{
    os << "base: ";
    sym_arrow::disp_nocannonize(os, ex.get_base(), false);
    os << " :: ";
    testing::disp(os, ex.get_base_value(), false);
    os << "\n";

    os << "dif: ";
    sym_arrow::disp_nocannonize(os, ex.get_dif(), false);
    os << " :: ";
    testing::disp(os, ex.get_dif_value(), false);
    os << "\n";

    os << "\n";
};

double testing::rand()
{
    return testing::genrand_real1();
};

double testing::randn()
{
    return testing::gen_norm();
};

int testing::irand()
{
    return testing::genrand_int32();
};

bool& rand_state::get_debug_flag()
{
    static bool flag = false;
    return flag;
}

expr_value testing::expr_rand(int n_sym, int max_depth, bool d, bool test_val)
{
    rand_state rs(n_sym,max_depth,d,test_val);
    return rs.rand_expr(0);
};

expr_value_dif rand_state::sum_dif(const expr_value_dif& x1, const expr_value_dif& x2)
{
    expr new_base               = x1.get_base() + x2.get_base();
    error_value new_base_val    = x1.get_base_value() + x2.get_base_value();
    expr new_dif                = x1.get_dif() + x2.get_dif();
    error_value new_dif_val     = x1.get_dif_value() + x2.get_dif_value();

    return expr_value_dif(new_base, new_base_val, new_dif, new_dif_val);
};

expr_value_dif rand_state::minus_dif(const expr_value_dif& x1, const expr_value_dif& x2)
{
    expr new_base               = x1.get_base() - x2.get_base();
    error_value new_base_val    = x1.get_base_value() - x2.get_base_value();
    expr new_dif                = x1.get_dif() - x2.get_dif();
    error_value new_dif_val     = x1.get_dif_value() - x2.get_dif_value();

    return expr_value_dif(new_base, new_base_val, new_dif, new_dif_val);
};

expr_value_dif rand_state::mult_dif(const expr_value_dif& x1, const expr_value_dif& x2)
{
    expr new_base               = x1.get_base() * x2.get_base();
    error_value new_base_val    = x1.get_base_value() * x2.get_base_value();

    expr new_dif                = x1.get_dif() * x2.get_base() + x1.get_base() * x2.get_dif();
    error_value new_dif_val     = x1.get_dif_value() * x2.get_base_value() 
                                + x1.get_base_value() * x2.get_dif_value();

    return expr_value_dif(new_base, new_base_val, new_dif, new_dif_val);
}

expr_value_dif rand_state::div_dif(const expr_value_dif& x1, const expr_value_dif& x2)
{
    expr new_base               = x1.get_base() / x2.get_base();
    error_value new_base_val    = x1.get_base_value() / x2.get_base_value();

    //(f/g)' = f'/g - f/(g^2) * g'
    expr new_dif                = x1.get_dif() / x2.get_base() 
                                - x1.get_base() * power_int(x2.get_base(), -2) * x2.get_dif();
    error_value new_dif_val     = x1.get_dif_value() / x2.get_base_value() 
                                - x1.get_base_value() * power_int(x2.get_base_value(), -2) 
                                        * x2.get_dif_value();

    return expr_value_dif(new_base, new_base_val, new_dif, new_dif_val);
}

expr_value_dif rand_state::uminus_dif(const expr_value_dif& x1)
{
    expr new_base               = -x1.get_base();
    error_value new_base_val    = -x1.get_base_value();
    expr new_dif                = -x1.get_dif();
    error_value new_dif_val     = -x1.get_dif_value();

    return expr_value_dif(new_base, new_base_val, new_dif, new_dif_val);
};

expr_value_dif rand_state::pow_int_dif(const expr_value_dif& x1, int pow)
{
    expr new_base               = power_int(x1.get_base(), pow);
    error_value new_base_val    = power_int(x1.get_base_value(), pow);

    //(f^k)' = k * f^k-1 * f'
    expr new_dif                = value::make_value(pow) * power_int(x1.get_base(), pow - 1)
                                    * x1.get_dif();
    error_value new_dif_val     = value::make_value(pow) * power_int(x1.get_base_value(), pow - 1) 
                                    * x1.get_dif_value();

    return expr_value_dif(new_base, new_base_val, new_dif, new_dif_val);
};

expr_value_dif rand_state::pow_real_dif(const expr_value_dif& x1, const expr_value_dif& x2)
{
    expr new_base               = power_real(x1.get_base(), x2.get_base());
    error_value new_base_val    = power_real(x1.get_base_value(), x2.get_base_value());

    //(|f|^g)' = exp(log(|f|) * g)' = |f|^g * (log(|f|)' * g + log(|f|) * g')
    expr_value_dif log1         = log_dif(x1);

    expr new_dif                = new_base * (log1.get_dif() * x2.get_base() 
                                    + log1.get_base() * x2.get_dif());
    error_value new_dif_val     = new_base_val * (log1.get_dif_value() * x2.get_base_value() 
                                                    + log1.get_base_value() * x2.get_dif_value());

    return expr_value_dif(new_base, new_base_val, new_dif, new_dif_val);
};

expr_value_dif rand_state::exp_dif(const expr_value_dif& x1)
{
    expr new_base               = exp(x1.get_base());
    error_value new_base_val    = exp(x1.get_base_value());

    if (x1.get_dif_value().get_value().is_zero() == true 
        && x1.get_dif_value().get_error() == 0.0)
    {
        return expr_value_dif(new_base, new_base_val, x1.get_dif(), x1.get_dif_value());
    };

    expr new_dif                = exp(x1.get_base()) * x1.get_dif();
    error_value new_dif_val     = exp(x1.get_base_value()) * x1.get_dif_value();

    return expr_value_dif(new_base, new_base_val, new_dif, new_dif_val);
};

expr_value_dif rand_state::log_dif(const expr_value_dif& x1)
{
    expr new_base               = log(x1.get_base());
    error_value new_base_val    = log(x1.get_base_value());

    // log(|f|)' = 1/|f| * |f|'
    expr_value_dif x_abs        = abs_dif(x1);
    expr new_dif                = x_abs.get_dif() / x_abs.get_base();
    error_value new_dif_val     = x_abs.get_dif_value() / x_abs.get_base_value();

    return expr_value_dif(new_base, new_base_val, new_dif, new_dif_val);
};

expr_value_dif rand_state::abs_dif(const expr_value_dif& x1)
{
    expr new_base               = abs(x1.get_base());
    error_value new_base_val    = abs(x1.get_base_value());

    // |f|' = |f|/f * f'
    expr new_dif                = new_base / x1.get_base() * x1.get_dif();
    error_value new_dif_val     = new_base_val / x1.get_base_value() * x1.get_dif_value();

    return expr_value_dif(new_base, new_base_val, new_dif, new_dif_val);
};

};};
