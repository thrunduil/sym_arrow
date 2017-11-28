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

private:
    std::ostream*           os;

private:
    expr                    make_power(expr&& x, expr&& p);
    expr                    get_number(const std::string& s);
    int                     get_int(const std::string& s);

    expr                    make_function(const symbol& sym, const std::vector<expr>& args);
    void                    to_number(const std::string& value_str0, bool is_complex, double& ret);
    void                    get_precission(const std::string& str,size_t& radix,long& exp);
    int                     to_int(const std::string& value_str);

private:
    void                    reportError(const antlr::RecognitionException& ex);
    void                    reportError(const std::string& s);
    void                    reportWarning(const std::string& s);

public:
    void                    init(std::ostream* os);