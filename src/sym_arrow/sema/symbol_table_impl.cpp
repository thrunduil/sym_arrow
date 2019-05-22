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

#include "sym_arrow/sema/symbol_table_impl.h"
#include "sym_arrow/error/sema_error.h"
#include "dag/details/leak_detector.h"

namespace sym_arrow { namespace details
{

//----------------------------------------------------------------------
//                        symbol data types
//----------------------------------------------------------------------

def_data::def_data(symbol_kind kind)
    :m_kind(kind)
{};

def_data::~def_data()
{};

def_data_set::def_data_set(const set& s)
    :m_set(s), def_data(symbol_kind::set)
{};

def_data_set::~def_data_set()
{};

def_data_type::def_data_type()
    :def_data(symbol_kind::type)
{};

def_data_type::~def_data_type()
{};

def_data_symbol::def_data_symbol(const std::vector<identifier>& args, const identifier& t)
    :def_data(symbol_kind::indexed_symbol), m_args(args), m_type(t)
{};

def_data_symbol::~def_data_symbol()
{};

symbol_map_entry::symbol_map_entry(const identifier& sym, const symbol_data_maker& data)
    :m_sym(sym), m_data(data())
{
    m_code = sym.get_base_symbol_code();
}

symbol_map_entry::~symbol_map_entry()
{
    delete m_data;
}

void symbol_map_entry::close()
{
    delete m_data;
    m_sym   = identifier();
    m_data  = nullptr;

    #if SYM_DAG_DEBUG_MEMORY
        sym_dag::details::leak_detector::report_free(this);
    #endif  
}

//----------------------------------------------------------------------
//                        symbol_map
//----------------------------------------------------------------------

symbol_map::symbol_map()
    :m_pool(sizeof(entry))
{};

symbol_map::~symbol_map()
{
    m_code_sym_map.close();
    m_pool.purge_memory();
};

const symbol_map::entry* symbol_map::define(const identifier& sym, const symbol_data_maker& data)
{
    size_t code     = sym.get_base_symbol_code();
    auto pos        = m_code_sym_map.get(code);

    if (pos.empty() == false)
        return pos.get();

    entry* ptr      = (entry*)m_pool.malloc(); 
    new(ptr) entry(sym, data);

    pos.assign(ptr);

    return nullptr;
}

const symbol_map::entry* symbol_map::get_entry(const identifier& sym) const
{
    size_t code     = sym.get_base_symbol_code();
    auto pos        = m_code_sym_map.get(code);

    if (pos.empty() == true)
        return nullptr;
    else
        return pos.get();
}

bool symbol_map::get_set(const identifier& sym, set& s) const
{
    size_t code     = sym.get_base_symbol_code();
    auto pos        = m_code_sym_map.get(code);

    if (pos.empty() == true)
        return false;

    const def_data* def  = pos.get()->get_data();

    if (def == nullptr)
        return false;

    if (def->get_kind() != symbol_kind::set)
        return false;

    s = dynamic_cast<const def_data_set*>(def)->get();
    return true;
}

//----------------------------------------------------------------------
//                        sym_table_impl
//----------------------------------------------------------------------
sym_table_impl* sym_table_impl::m_impl = nullptr;

sym_table_impl::sym_table_impl()
{};

sym_table_impl::~sym_table_impl()
{};

sym_table_impl* sym_table_impl::get()
{
    if (m_impl == nullptr)
        m_impl = new sym_table_impl();

    return m_impl;
}

void sym_table_impl::close()
{
    delete m_impl;
    m_impl = nullptr;
};

void sym_table_impl::define_set(const identifier& sym, const set& s)
{
    symbol_data_maker f = [&](){return new def_data_set(s);};
    const entry* ptr    = m_sym_map.define(sym, f);

    //1. set name cannot be already defined

    if (ptr)
    {      
        // symbol is already defined
        bool is_set = (ptr->get_kind() == symbol_kind::set);

        if (is_set == true)
            error::sema_error().set_already_defined(sym);
        else
            error::sema_error().symbol_already_defined(sym);

        return;
    };
};

void sym_table_impl::define_type(const identifier& sym)
{    
    symbol_data_maker f = [](){return new def_data_type();};

    const entry* ptr    = m_sym_map.define(sym, f);

    //1. type name cannot be already defined

    if (ptr)
    {      
        // symbol is already defined
        bool is_type = (ptr->get_kind() == symbol_kind::type);

        if (is_type == true)
            error::sema_error().type_already_defined(sym);
        else
            error::sema_error().symbol_already_defined(sym);

        return;
    };
}

void sym_table_impl::define_symbol(const identifier& sym, 
                            const std::vector<identifier>& args, const identifier& t)
{
    if (t.is_null() == false)
    {
        // type name must be defined

        bool is_def = this->is_defined_type(t);

        if (is_def == false)
            error::sema_error().type_not_defined(t);        
    }

    for (const auto& it : args)
    {
        // set name must be defined

        bool is_def = this->is_defined_set(it);

        if (is_def == false)
            error::sema_error().set_not_defined(it); 
    };

    symbol_data_maker f = [&](){return new def_data_symbol(args, t);};
    const entry* ptr    = m_sym_map.define(sym, f);

    //1. symbol name cannot be already defined

    if (ptr)
    {      
        // symbol is already defined
        // TODO: more informative error
        error::sema_error().symbol_already_defined(sym);

        return;
    };    
}

bool sym_table_impl::is_defined_set(const identifier& sym) const
{
    const entry* ptr    = m_sym_map.get_entry(sym);

    if (!ptr)
        return false;
    else
        return (ptr->get_kind() == symbol_kind::set);
};

bool sym_table_impl::is_defined_type(const identifier& sym) const
{
    const entry* ptr    = m_sym_map.get_entry(sym);

    if (!ptr)
        return false;
    else
        return (ptr->get_kind() == symbol_kind::type);
};

bool sym_table_impl::get_set_definition(const identifier& sym, set& def) const
{
    return m_sym_map.get_set(sym, def);
}

}};
