/*
*  Morfa programming language.
*  This file is a part of sym_arrow library.
*
*  Copyright (c) Pawe³ Kowal 2017-2019
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
#include "sym_arrow/nodes/set.h"
#include "dbs/dbs.h"

namespace sym_arrow { namespace details
{

//----------------------------------------------------------------------
//                        symbol data types
//----------------------------------------------------------------------

enum class symbol_kind
{
    set, type, symbol
};

class def_data
{
    private:
        symbol_kind     m_kind;

    public:
        def_data(symbol_kind kind);
        virtual ~def_data();

        symbol_kind     get_kind() const    { return m_kind; }
};

class def_data_set : public def_data
{
    private:
        set             m_set;

    public:
        def_data_set(const set& s);
        ~def_data_set();

        const set&      get() const         { return m_set; }
};

class def_data_type : public def_data
{
    public:
        def_data_type();
        ~def_data_type();
};

class def_data_symbol : public def_data
{
    private:
        std::vector<identifier> m_args;
        identifier              m_type;
        bool                    m_is_const;

    public:
        def_data_symbol(const std::vector<identifier>& args, const identifier& t,
                bool is_const);

        ~def_data_symbol();

        void    get_def(std::vector<identifier>& args, identifier& t, bool& is_const) const;
};

//----------------------------------------------------------------------
//                        symbol map entry
//----------------------------------------------------------------------

class symbol_map_entry
{
    public:
        using symbol_data_maker = std::function<def_data* ()>;

    private:
        size_t          m_code;
        def_data*       m_data;
        identifier      m_sym;
        
    public:
        // take ownership of data pointer; memory is released, when this object
        // is destroyed
        symbol_map_entry(const identifier& sym, const symbol_data_maker& data);

        ~symbol_map_entry();

        symbol_map_entry(const symbol_map_entry&) = delete;
        symbol_map_entry& operator=(const symbol_map_entry&) = delete;

    public:
        size_t          hash_value() const      { return m_code; }
        symbol_kind     get_kind() const        { return m_data->get_kind(); }
        const def_data* get_data() const        { return m_data;}

        void            close();
};

struct symbol_map_entry_track
{
    using T         = symbol_map_entry;

    static void     free(T* ptr)            {ptr->close();};
    static void     copy(T* ptr)            {(void)ptr;};
    static void     assign(T* to, T* from)  {(void)from;(void)to;};};

//----------------------------------------------------------------------
//                        symbol map
//----------------------------------------------------------------------

class symbol_map
{
    public:
        using entry             = symbol_map_entry;
        using symbol_data_maker = symbol_map_entry::symbol_data_maker;

    private:
        using allocator         = sym_dag::details::symbolic_allocator<symbol_map>;
        using pool_allocator    = sym_dag::details::object_allocator<allocator>;      
        using entry_track       = symbol_map_entry_track;

        struct cs_h
        {
            size_t operator()(size_t c) const           { return c;};
            size_t operator()(const entry* c) const     { return c->hash_value();};
        };

        struct cs_e
        {
            bool operator()(const entry* c1, size_t, size_t c2) const 
                                                        { return c1->hash_value() == c2; };
        };

        using sym_map           = sym_dag::hash_table<entry, cs_h, cs_e, entry_track>;

    private:
        pool_allocator          m_pool;
        sym_map                 m_code_sym_map;

    public:
        symbol_map();
        ~symbol_map();

        // return null if sym is not defined, or a pointer to existing definition
        const entry*            define(const identifier& sym, const symbol_data_maker& data);

        // return null if sym is not defined, or a pointer to existing definition
        const entry*            get_entry(const identifier& sym) const;

        // return false if sym does not define a set
        bool                    get_set(const identifier& sym, set& s) const;

        // return false if sym does not define a symbol
        bool                    get_symbol(const identifier& sym, 
                                    std::vector<identifier>& args, identifier& t,
                                    bool& is_const) const;
};

//----------------------------------------------------------------------
//                        symbol table
//----------------------------------------------------------------------

// management of symbol definitions
// this is a singleton class
class sym_table_impl
{
    private:
        using entry             = symbol_map::entry;
        using symbol_data_maker = symbol_map::symbol_data_maker;

    private:
        static sym_table_impl*  m_impl;
        symbol_map              m_sym_map;

    private:
        sym_table_impl();

    public:
        ~sym_table_impl();

        sym_table_impl(const sym_table_impl&) = delete;
        sym_table_impl& operator=(const sym_table_impl&) = delete;

    public:
        // get global symbol table
        static sym_table_impl*  get();

        // release handle to this object; memory is released if all handles 
        // are released
        void                    close();

    public:
        // definition of a symbol sym as a set
        void                    define_set(const identifier& sym, const set& s);

        // definition of a new type t
        void                    define_type(const identifier& t);

        // define possibly indexed symbol sym<a1, ..., ak> of type t
        // sets a1, ..., ak must be already defined using define_set
        // function, if type name t is not initialized, then sym has
        // default type
        void                    define_symbol(const identifier& sym, 
                                    const std::vector<identifier>& args, const identifier& t,
                                    bool is_const);

    public:
        // return true is symbol sym defines a set
        bool                    is_defined_set(const identifier& sym) const;

        // return true is symbol sym defines a type
        bool                    is_defined_type(const identifier& sym) const;

        // return a set defined by the symbol sym; return true if sym defines a
        // set, otherwise false is returned
        bool                    get_set_definition(const identifier& sym, set& def) const;

        // return a symbol defined by the identifier sym; return true if sym defines a
        // symbol, otherwise false is returned; if true is returned, then args contains
        // index sets and t is defined type
        bool                    get_symbol_definition(const identifier& sym, 
                                    std::vector<identifier>& args, identifier& t, 
                                    bool& is_const) const;
};

}};
