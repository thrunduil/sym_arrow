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

#include "sym_arrow/config.h"
#include <string>

#ifdef __unix__
    #include <sys/timeb.h>
#endif

namespace sym_arrow { namespace details
{

struct SYM_ARROW_EXPORT timer_base
{
    bool            tic_started;
    int64_t         tic_int64;

    #ifdef __unix__
      struct timeb  tic_timeb;
    #endif

    void            init();
    void            tic();
    double          toc();

    std::string     tocstr();
    void            tocdisp();
};

}}

namespace sym_arrow 
{

// calculate elapsed time
class SYM_ARROW_EXPORT timer : private details::timer_base
{
    public:
        // create timer object; in order to start counting time call tic()
        timer()        { init();};

        // restart timer
        using details::timer_base::tic;
        
        // finish timer; return elapsed time is seconds from last call to tic()
        using details::timer_base::toc;

        // finish timer; return elapsed time as string is seconds from last
        // call to tic()
        using details::timer_base::tocstr;
        
        // finish timer; display elapsed time is seconds from last call to tic()
        // on global output stream
        using details::timer_base::tocdisp;
};

// start global timer
SYM_ARROW_EXPORT void           tic();

// stop global timer and return time elapsed from last tic in seconds
SYM_ARROW_EXPORT double         toc();

// stop global timer and return time elapsed from last tic in seconds
// represented as string
SYM_ARROW_EXPORT std::string    tocstr();

// stop global timer; calculatr time elapsed from last tic in seconds
// and print elapsed time on global_output_stream
SYM_ARROW_EXPORT void           tocdisp();

}
