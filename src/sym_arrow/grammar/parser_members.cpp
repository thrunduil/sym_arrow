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

#include "grammar/output/parser_sym_arrow.hpp"
#include "sym_arrow/nodes/expr.h"
#include "sym_arrow/nodes/set.h"
#include "sym_arrow/sema/symbol_table.h"
#include "sym_arrow/ast/ast.h"

#pragma warning(push)
#pragma warning(disable:4702)
#include <boost/lexical_cast.hpp>
#pragma warning(pop)

void parser_sym_arrow::init(std::ostream* os_)
{
    os = os_;
};

void parser_sym_arrow::reportError(const std::string& str)
{
    throw std::runtime_error(str);
};

void parser_sym_arrow::reportError(const antlr::RecognitionException& ex)
{
    std::ostringstream os2;
    os2 << ex.getMessage() << ", line: " << ex.getLine() << ":" << ex.getColumn();
    throw std::runtime_error(os2.str());
};

void parser_sym_arrow::reportWarning(const std::string& str)
{
    if (os)
        *os << "warning: " << str << "\n";
};

int parser_sym_arrow::get_int(const std::string& s)
{
    int val = to_int(s);
    return val;
};

expr parser_sym_arrow::make_power(expr&& x, expr&& p)
{
    static const double tol = 100 * std::numeric_limits<double>::epsilon();

    if (p.get_ptr()->isa<ast::scalar_rep>() == true)
    {
        value val       = p.get_ptr()->static_cast_to<ast::scalar_rep>()->get_data();
        bool is_int     = val.is_int();

        if (is_int == true)
        {
            return power_int(std::move(x), val.convert_int());
        }
        else
        {
            return power_real(std::move(x), std::move(p));
        };
    }
    else
    {
        return power_real(std::move(x), std::move(p));
    }
};

expr parser_sym_arrow::get_number(const std::string& s)
{
    #if SYM_ARROW_VALUE_TYPE == SYM_ARROW_VALUE_MP
        value val   = value::make_from_string(s);
        return expr(val);
    #else
        double val;
        to_number(s, false, val);

        return expr(val);
    #endif
};

void parser_sym_arrow::to_number(const std::string& value_str0, bool is_complex, double& ret)
{
    size_t str_size              = (value_str0.size() == 0) ? 0 : value_str0.size() - 1;
    const std::string& value_str = (is_complex == false) ? value_str0 : value_str0.substr(0,str_size);

    size_t radix;
    long exp;
    get_precission(value_str,radix,exp);

    size_t prec_d = std::numeric_limits<double>::digits10;
    if (radix > prec_d)
    {
        std::string msg = "too many significant digits, precision is lost";
        reportWarning(msg);
    };

    int max_exp    = std::numeric_limits<double>::max_exponent10;
    int min_exp    = std::numeric_limits<double>::min_exponent10;

    if (exp + static_cast<long>(radix) <= max_exp && exp + static_cast<long>(radix) - 1 >= min_exp)
    {
        double value_d = boost::lexical_cast<double>(value_str);
        ret = value_d;
        return;
    }
    else
    {
        std::string msg = "numeric value too large";
        reportError(msg);
        ret = 0;
        return;
    };
};

void parser_sym_arrow::get_precission(const std::string& str,size_t& radix,long& exp)
{
    radix           = 0;
    exp             = 0;
    int mult        = 1;
    int exp_mult    = 1;
    long e_exp      = 0;

    using iterator  = std::string::const_iterator;
    iterator pos    = str.begin();

    enum STATE {S_INIT,S_RADIX,S_DOT,S_INIT_DOT,S_EXP};
    STATE state = S_INIT;
    while(pos !=str.end())
    {
        switch (*pos)
        {
            case '0':
            {
                switch (state)
                {
                    case S_INIT:
                    {
                        break;
                    }
                    case S_RADIX:
                    {
                        radix++;
                        break;
                    }
                    case S_DOT:
                    {
                        radix++;
                        exp--;
                        break;
                    }
                    case S_INIT_DOT:
                    {
                        exp--;
                        break;
                    }
                    case S_EXP:
                    {
                        e_exp    = e_exp*10;
                        break;
                    }
                }
                break;
            }
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            {
                switch (state)
                {
                    case S_INIT:
                    {
                        radix++;
                        state = S_RADIX;
                        break;
                    }
                    case S_RADIX:
                    {
                        radix++;
                        break;
                    }
                    case S_DOT:
                    {
                        radix++;
                        exp--;
                        break;
                    }
                    case S_INIT_DOT:
                    {
                        radix++;
                        exp--;
                        state = S_DOT;
                        break;
                    }
                    case S_EXP:
                    {
                        e_exp    = e_exp*10 + (*pos)-'0';
                        break;
                    }
                }
                break;
            }
            case '.':
            {
                switch (state)
                {
                    case S_INIT:
                    {
                        state = S_INIT_DOT;
                        break;
                    }
                    case S_RADIX:
                    {
                        state = S_DOT;
                        break;
                    }
                }
                break;
            }
            case 'e':
            case 'E':
            {
                if (state == S_INIT || state == S_INIT_DOT)
                {
                    return;
                };
                state = S_EXP;
                break;
            }
            case '+':
            {
                break;
            }
            case '-':
            {
                switch (state)
                {
                    case S_INIT:
                    {
                        mult = -1;
                        break;
                    }
                    case S_EXP:
                    {
                        exp_mult = -1;
                        break;
                    }
                }
                break;
            }
            case 'i':
            {
                break;
            }
        };
        pos++;
    };
    exp = exp + exp_mult*e_exp;
};

int parser_sym_arrow::to_int(const std::string& value_str)
{
    using iterator      = std::string::const_iterator;
    size_t length       = value_str.size();
    size_t max_digits   = std::numeric_limits<int>::digits10;
    
    if (length<=max_digits)
    {
        int value       = 0;
        iterator pos    = value_str.begin();

        while (pos!=value_str.end())
        {
            value       = value * 10 + (*pos) - '0';
            pos++;
        };

        return value;
    }
    else
    {
        std::string msg = "integer value too large";
        throw std::runtime_error(msg);
    };
};

expr parser_sym_arrow::make_function(const identifier& sym, const std::vector<expr>& args)
{
    if (sym.get_name() == std::string("exp") && args.size() == 1)
        return sym_arrow::exp(args[0]);

    if (sym.get_name() == std::string("log") && args.size() == 1)
        return sym_arrow::log(args[0]);

    return sym_arrow::function(sym, args);
};

symbol parser_sym_arrow::make_indexed(const identifier& sym, const std::vector<expr>& args)
{
    std::initializer_list<expr> il(args.data(), args.data() + args.size());

    return sym.indexed(il);
};

symbol parser_sym_arrow::make_symbol(const identifier& sym)
{
    return symbol::from_identifier(sym);
};

index parser_sym_arrow::make_indexer(const identifier& sym, const identifier& set)
{
    return index(sym, set);
};

set parser_sym_arrow::make_set(const std::vector<identifier>& args)
{
    return sym_arrow::set(args);
}

void parser_sym_arrow::def_set(const identifier& sym, const set& ex)
{
    sym_table::get().define_set(sym, ex);
}

void parser_sym_arrow::def_sym(const identifier& sym, const std::vector<identifier>& args,
                            const identifier& type)
{
    sym_table::get().define_symbol(sym, args, type);
}

void parser_sym_arrow::def_type(const identifier& sym)
{
    sym_table::get().define_type(sym);
}
