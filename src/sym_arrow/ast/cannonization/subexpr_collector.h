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
#include "sym_arrow/ast/builder/build_item.h"
#include "sym_arrow/ast/cannonization/subexpr_ordering.h"
#include "dbs/dbs.h"
#include "sym_arrow/utils/temp_value.h"

#include <map>

namespace sym_arrow { namespace ast
{

namespace sd = sym_arrow ::details;

enum class reduce_type
{
    ipow,
    rpow,
    exp, 
    ipow_horner,
};

// count number of subterms (ipow, rpow, exp subexpressions)
class subs_counter
{
    public:
        size_t      n_ipow;
        size_t      n_rpow;
        size_t      n_exp;

    public:
        subs_counter();

        void        add_ipow(size_t n_elems);
        void        add_rpow(size_t n_elems);
        void        add_exp();
};

struct ipow_item
{
    expr_handle     m_expr;
    size_t          m_add_pos;
    size_t          m_mult_pos;
    int             m_pow;
    bool            m_can_deep_simpl;

    ipow_item()
    {};

    // this is mult subexpression
    ipow_item(size_t add_pos, size_t mult_pos, int it_pow, expr_handle it_e,
              bool can_deep_simpl)
        :m_add_pos(add_pos), m_mult_pos(mult_pos), m_pow(it_pow)
        , m_expr(it_e), m_can_deep_simpl(can_deep_simpl)
    {};

    // this is atom
    ipow_item(size_t add_pos, int it_pow, expr_handle it_e)
        :m_add_pos(add_pos), m_mult_pos(size_t(-1)), m_pow(it_pow)
        , m_expr(it_e), m_can_deep_simpl(false)
    {};

    bool is_atom() const
    {
        return m_mult_pos == size_t(-1);
    };

    size_t get_mult_pos() const
    {
        return m_mult_pos;
    }
};

struct rpow_item
{
    expr_handle     m_expr;
    size_t          m_add_pos;
    size_t          m_mult_pos;
    value           m_pow;
    bool            m_can_deep_simpl;

    rpow_item(size_t add_pos, size_t mult_pos, const value& it_pow, expr_handle it_e,
              bool can_deep_simpl)
        : m_add_pos(add_pos), m_mult_pos(mult_pos) , m_pow(it_pow)
        , m_expr(it_e), m_can_deep_simpl(can_deep_simpl)
    {};

    bool is_atom() const
    {
        return false;
    };

    size_t get_mult_pos() const
    {
        return m_mult_pos;
    }
};

struct exp_item
{
    expr_handle     m_expr;
    size_t          m_add_pos;
    bool            m_can_deep_simpl;

    exp_item(size_t add_pos, expr_handle it_e, bool can_deep_simpl)
        :m_add_pos(add_pos), m_expr(it_e), m_can_deep_simpl(can_deep_simpl)
    {};

    bool is_atom() const
    {
        return false;
    };

    size_t get_mult_pos() const
    {
        return 0;
    }
};

struct ipow_comparer
{
    bool operator()(const ipow_item& a, const ipow_item& b) const;
};

struct rpow_comparer
{
    bool operator()(const rpow_item& a, const rpow_item& b) const;
};

struct exp_comparer
{
    bool operator()(const exp_item& a, const exp_item& b) const;
};

class factorizations
{
    private:
        using subs_vec  = std::vector<mult_subexpr>;

        subs_vec&       m_vec;

    public:
        factorizations(subs_vec& subs);

        size_t          size() const;
        void            add(const mult_subexpr& item);
};

// collect simple statistics about factorization group
class factor_group_stats
{
    private:
        using dbs       = dbs_lib::dbs;

    private:
        size_t          n_symbols;
        dbs             m_symbol_set;
        expr_handle     m_base;

    public:
        factor_group_stats();

        // constuctor from number of symbols in factorization
        // group; union of symbol sets; and smallest base
        factor_group_stats(size_t symbols, const dbs& syms, expr_handle eh_best);

        // ordering of two statistics, based on lexicographic
        // ordering (number symbols x symbol set x smallest base)
        bool        operator<(const factor_group_stats& other) const;
};

// collect common subexpressions in add expression
class subexpr_collector
{
    private:
        using add_item_handle   = build_item_handle<value>;
        using sub_type_vector   = std::vector<mult_subexpr>;
        using dbs               = dbs_lib::dbs;
        using temp_value        = sd::temp_value<value, value::is_pod>;

    private:
        temp_value*     m_temp_vals;

    public:
        // return true if some of items was factored; 
        // if true is returned, then factored items are removed 
        // from the array ih (i.e. expr handle is set to nullptr);
        bool            make(size_t n, add_item_handle* ih, expr& res, value& scal,
                            temp_value& temp_vals);

    private:
        // count number of subterms (ipow, rpow, exp subexpressions)
        void            calc_size(const add_item_handle& it, subs_counter& count);

        // collect mult_rep atoms
        void            collect_mult_ipow(const add_item_handle& it, size_t pos, 
                            ipow_item* arr, size_t& arr_pos);
        void            collect_mult_rpow(const add_item_handle& it, size_t pos, 
                            rpow_item* arr, size_t& arr_pos);
        void            collect_mult_epow(const add_item_handle& it, size_t pos, 
                            exp_item* arr, size_t& arr_pos);

        // return position of first item in arr with base different, than
        // arr[pos]
        template<class Array_item>
        size_t          find_next_base(const Array_item* arr, size_t arr_size, size_t pos);

        // return number of subexpr in array arr of length arr_size that 
        // has can_deep_simpl flag
        template<class Array_item>
        size_t          check_can_deep_flag(const Array_item* arr, size_t arr_size);

        // collect all possible ipow subexpr simplifications
        void            select_subexpr_ipow(const ipow_item* arr, size_t arr_size, 
                            factorizations& fac, bool& has_equal);

        // collect all possible exp subexpr simplifications
        void            select_subexpr_epow(const exp_item* arr, size_t arr_size, 
                            factorizations& fac);

        // collect all possible rpow subexpr simplifications
        void            select_subexpr_rpow(const rpow_item* arr, size_t arr_size,
                            factorizations& fac);

        // select best subexpression to factorize; it is assumed
        // that sub_types contains only equally good items with 
        // respect to mult_subexpr::compare; best factorizations are
        // at positions 0 - (group_size - 1)
        void            select_subexpr(sub_type_vector& sub_types, ipow_item* ipow_arr, 
                            rpow_item* rpow_arr, exp_item* exp_arr, size_t& group_size);

        // sort subexpression to factorize according to add_pos;
        // it is assumed, that best factorization is already selected
        void            sort_subexpr(sub_type_vector& sub_types, ipow_item* ipow_arr, 
                            rpow_item* rpow_arr, exp_item* exp_arr, size_t group_size);

        // remove all factorization groups shorter, than the longest;
        // return number of remaining factorization groups
        size_t          select_longest_group(sub_type_vector& sub_types);

        // select arbitrarly one of remaining group
        void            select_ambiguous_group(sub_type_vector& sub_types);

        // measure complexity of a factorization group
        factor_group_stats
                        measure_group(const mult_subexpr* ptr, size_t size);

        // identify factorizations, an can be proceed together
        void            collect_factorizations(sub_type_vector& sub_types,
                            ipow_item* ipow_arr, rpow_item* rpow_arr, exp_item* exp_arr);
        bool            can_link_factorizations(const mult_subexpr& f1, 
                            const mult_subexpr& f2, ipow_item* ipow_arr, 
                            rpow_item* rpow_arr, exp_item* exp_arr);

        // return positions of given factor in add_rep subexpressions
        dbs             get_factor_positions(const mult_subexpr& f1, 
                            ipow_item* ipow_arr, rpow_item* rpow_arr, exp_item* exp_arr);

        // factorize common subexpressions of given type; result is expressed 
        // as scal * mult
        void            factor_subexpr(add_item_handle* ih, ipow_item* ipow_arr, 
                            rpow_item* rpow_arr, exp_item* exp_arr, sub_type_vector& sub_types,
                            size_t group_size, expr& res, value& scal);

        void            finalize_sum(build_item<value>* it, size_t size, const value& add,
                            expr& res, value& scal);

        // get size parameters of final mult expression
        void            measure_final_mult(size_t& isize, size_t& rsize, size_t& n_exp,
                            const expr& sum, value& scal, const mult_rep*& mh,
                            size_t group_size, const mult_subexpr* factors);

        // collect exp terms; should be called, then there are two exp terms
        void            collect_exp(const expr& sum, size_t group_size, 
                            const mult_subexpr* factors, expr& exp_term);

        // collect final mult expr subexpressions
        void            collect_final_mult(size_t isize, size_t& ipow_pos, size_t& rpow_pos, 
                            build_item_handle<int>* ih, build_item_handle<value>* rh, 
                            rpow_item* rpow_arr, const expr& sum, size_t group_size, 
                            const mult_subexpr* factors);

        // remove mult expr subexpressions according to factorization factor
        void            reduce_mult(size_t pos_mult, build_item_handle<int>* ih,  
                            build_item_handle<value>* rh, bool& has_exp, 
                            const mult_subexpr& factor);

        // build mult rep after factorization
        expr            build_reduced_mult(size_t isize, size_t rsize, build_item_handle<int>* ih, 
                            build_item_handle<value>* rh, bool has_exp);

        // get position in add subexpr array of given factorization
        size_t          get_add_pos(const mult_subexpr& factor, size_t pos_arr_off,
                            ipow_item* ipow_arr, rpow_item* rpow_arr, exp_item* exp_arr);

        // get position in mult subexpr array of given factorization
        size_t          get_mult_pos(const mult_subexpr& factor, size_t pos_arr_off,
                            ipow_item* ipow_arr, rpow_item* rpow_arr, exp_item* exp_arr);

        // add factorized term to mult expression
        void            add_factor_to_mult(size_t isize, size_t& ipow_size, size_t& rpow_size, 
                            build_item_handle<int>* ih, build_item_handle<value>* rh, 
                            const rpow_item* rpow_arr, const mult_subexpr& factor);
        void            add_factor_to_mult(size_t isize, size_t& ipow_size, size_t& rpow_size, 
                            build_item_handle<int>* ih, build_item_handle<value>* rh, 
                            const mult_rep* mh);

        // return true if horner factorization can be performed
        bool            can_horner(expr_handle h);

        // return number of negative powers in sequnce of subexpr with the same base
        size_t          number_negative_int(const ipow_item* arr, size_t n_elems);

        // order sequence of subexpr with the same base and the same power
        // sequences containing more subexpr allowing for deep simplifications will have
        // preference over any other sequences; then longer sequences are preferred;
        // if two sequences have the same lengh then smaller power is preferred if 
        // pref_small_pow = true, and higher powers otherwise; it is assumed, that powers
        // in arr are sorted increasingly; return position of the first element in the
        // best group
        size_t          longest_equal_sequence_int(const ipow_item* arr, size_t n_elems, 
                            bool pref_small_pow, size_t& length, int& max_pow, size_t& n_deep);

        // order sequence of subexpr with the same base and the same power
        // sequences containing more subexpr allowing for deep simplifications will have
        // preference over any other sequences; then longer sequences are preferred;
        // if two sequences have the same lengh then smaller power is preferred if 
        // pref_small_pow = true, and higher powers otherwise; it is assumed, that powers
        // in arr are sorted increasingly; return position of the first element in the
        // best group
        size_t          longest_equal_sequence_real(const rpow_item* arr, size_t n_elems, 
                            bool pref_small_pow, size_t& max_length, size_t& n_deep);
};

};};
