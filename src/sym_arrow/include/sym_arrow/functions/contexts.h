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

#include "sym_arrow/nodes/expr.h"
#include "dbs/dbs.h"

#include <map>
#include <memory>

#pragma warning(push)
#pragma warning(disable:4251)    //needs to have dll-interface

namespace sym_arrow
{

// define mapping from symbols to values and evaluate
// a function
class SYM_ARROW_EXPORT data_provider
{
    public:
        virtual ~data_provider() {};

        // return value associated to a symbol sh
        virtual value   get_value(const symbol& sh) const = 0;

        // evaluate a function with n_size arguments evaluated
        // to values stored in the array subexpr
        virtual value   eval_function(const symbol& name, const value* subexpr, 
                            size_t n_size) const = 0;
};

// visitor of substitutions stored in subs_context
class SYM_ARROW_EXPORT substitution_vis
{
    public:
        // destructor
        virtual ~substitution_vis(){};

        // report number of substitutions; size is as returned by
        // subs_context::size(); bind_length is as returned by
        // subs_context::bind_array_length()
        virtual void    report_size(size_t size, size_t bind_length) = 0;

        // report substitution sym -> ex with assigned bind code = 'code'
        // ex can be empty when subs_context::add_symbol without substitution
        // was called
        virtual void    report_subs(const symbol& sym, const expr& ex,
                            size_t code) = 0;
};

// define substitutions from symbols to expressions
class SYM_ARROW_EXPORT subs_context
{
    private:
        using impl_type = std::shared_ptr<details::subs_context_impl>;
        using dbs       = dbs_lib::dbs;

    private:
        impl_type       m_impl;

    public:
        // create empty mapping
        subs_context();

        // add new symbol-expression substitution to this set
        void            add_symbol(const symbol& sym, const expr& ex);

        // add new symbol substitution to this set; expression can be
        // set later by add_bind function; return position in expression
        // vector supplied to set_bind function associated with this 
        // substitution; return code is just number of substitution added
        // up to now
        size_t          add_symbol(const symbol& sym);

        // remove existing symbol-expression substitution
        void            remove_symbol(const symbol& sym);

        // set delayed expressions; it is expected, that ex[c] is not empty
        // for every code c returned by add_symbol up to now; expressions are
        // not copied, therefore array ex must be valid when subs function is
        // called; bind array is cleared when this substitution set is modified,
        // therefore this function should be called just before call to subs 
        // function
        void            set_bind(size_t size, const expr* ex);

        // remove previously set expression array by set_bind function
        void            remove_bind();

        // get bitset of codes of all symbols with defined substitutions
        const dbs&      get_symbol_set() const;

        // return number of substitutions added to this set
        size_t          size() const;        

        // return expected size of array supplied to set_bind function; this size
        // is equal to number substitutions added by add_symbol functions
        // (remove_symbol calls are ignored)
        size_t          bind_array_length() const;

        // get substitution for the symbol sh; return empty expression
        // if substitution is not defined
        expr            subs(const symbol& sh) const;

        // visit stored substitution
        void            visit_substitutions(substitution_vis& info) const;

        // display content of this set
        void            disp(std::ostream& os) const;        

    private:
        void            add_symbol(const symbol& sym, size_t code);
};

// define differentiation rules for functions
class SYM_ARROW_EXPORT diff_context
{
    private:
        using impl_type = std::shared_ptr<details::diff_context_impl>;

    private:
        impl_type       m_impl;

    public:
        // create empty mapping
        diff_context();

        // construct partial derivative of a function 'func_name' with respect
        // to i-th argument (i = arg_num, index is 0-based), i.e. and substitute
        // function arguments with n_args expressions given in the array args;
        // return empty expression if appropriate diff rule is not defined
        expr            diff(const symbol& func_name, size_t arg_num, const expr* args,
                            size_t n_args);

        // add differentiation rule d/dx_i f[x0, ..., xn] -> dif[x0, ..., xn]
        void            add_diff_rule(const symbol& func_name, size_t n_args,
                            const symbol* args, size_t diff_arg, const expr& dif);
};

// return global diff context
SYM_ARROW_EXPORT 
const diff_context&     global_diff_context();

};

#pragma warning(pop)