/*
*  This file is a part of sym_arrow library.
*
*  Copyright (c) Pawe³ Kowal 2017 - 2021
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

#include "dbs/dbs.h"
#include "sym_arrow/nodes/expr.h"
#include "sym_arrow/utils/pool_hash_map.h"
#include "sym_arrow/ast/builder/vlist.h"
#include "sym_arrow/ast/expr_cache.h"

namespace sym_arrow { namespace details
{

class symbol_wrap
{
    public:
        using value_type    = symbol;
        using stack_type    = ast::vlist_stack;

    private:
        symbol              m_symbol;

    public:
        symbol_wrap(const symbol& s);

        const symbol&       get() const { return m_symbol; };
        void                release(stack_type& st);
};

// class storing diff results
class diff_hash_data_base : protected ast::vlist_base<symbol_wrap, diff_hash_data_base>
{
    private:
        using base_type     = ast::vlist_base<symbol_wrap, diff_hash_data_base>;
        using stack_type    = ast::expr_base::stack_type;

        friend base_type;

    public:
        // create uninitialized object
        diff_hash_data_base();

        diff_hash_data_base(const symbol& s);

        void                add(const symbol& s);

        bool                is_empty() const;
        size_t              current_size() const;
        bool                has_previous() const;
        const symbol&       elem(size_t pos) const;

        const diff_hash_data_base*
                            get_previous() const;        

        void                release(stack_type& st);

    public:
        void                init_with_default_values(){};
};

class diff_hash_data
{
    private:
        using stack_type    = ast::expr_base::stack_type;

    private:
        ast::expr_ptr       m_diff;

    public:
        // create uninitialized object
        diff_hash_data();

        // initialize with differential
        diff_hash_data(const expr& dif);

        expr                get_diff() const;
        bool                is_empty() const;

        void                release(stack_type& st);
};

struct expr_sym
{
    ast::expr_handle    m_handle;
    ast::symbol_handle  m_sym;
    size_t              m_hash;

    expr_sym(ast::expr_handle eh, ast::symbol_handle sh)
        : m_handle(eh), m_sym(sh)
    {
        m_hash          = eval_hash();
    }

    size_t eval_hash() const
    {
        // TODO: check this hashing
        size_t seed = (size_t)m_sym + (size_t)m_handle;
        //boost::hash_combine(seed, m_code);
        return seed;
    }
};

struct expr_sym_hash_equal
{
    static size_t hash_value(const expr_sym& h)
    {
        return h.m_hash;
    }

    static bool equal(const expr_sym& h1, const expr_sym& h2)
    {
        return h1.m_handle == h2.m_handle && h1.m_sym == h2.m_sym;
    }
};

// hasing diff result
class diff_hash : public sym_dag::node_cache
{
    private:
        static const size_t cache_size      = 10000;
        static const size_t num_rounds      = 32;

        static const size_t min_refcount    = 2;

    private:
        using expr_cache    = ast::expr_cache<cache_size, num_rounds>;
        using expr_handle   = ast::expr_handle;
        using stack_type    = ast::expr_base::stack_type;
        using hash_map_root = utils::pool_hash_map<ast::expr_handle, 
                                diff_hash_data_base, utils::expr_hash_equal>;        
        using hash_map_dif  = utils::pool_hash_map<expr_sym, diff_hash_data, 
                                expr_sym_hash_equal>;

        using ebs           = std::tuple<ast::expr_ptr, expr>;
        using diff_map      = std::map<ebs,expr>;
        using value_type    = diff_map::value_type;

    private:
        hash_map_root       m_hash_map_root;
        hash_map_dif        m_hash_map_dif;
        expr_cache          m_cache;

    private:
        diff_hash();

        diff_hash(const diff_hash&) = delete;
        diff_hash& operator=(const diff_hash&) = delete;

        friend sym_dag::global_objects;

    public:
        ~diff_hash();

        static diff_hash&   get();

        expr                find(expr_handle h, const symbol& s);
        void                add(expr_handle h, const symbol& s, const expr& dif);

    public:
        void                unregister(ast::expr_handle h, stack_type& st);
        void                unregister_cache(ast::expr_handle h, stack_type& st);
        virtual void        clear() override;
};

};};
