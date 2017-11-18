/*
    Copyright (c) 2007-2013 Contributors as noted in the AUTHORS file

    This file is part of 0MQ.

    0MQ is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    0MQ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __NET_CONFIG_HPP_INCLUDED__
#define __NET_CONFIG_HPP_INCLUDED__

namespace host
{

    //  Compile-time settings.

    enum
    {
        //  Commands in pipe per allocation event.
        command_pipe_granularity = 256,


        //  Maximum number of events the I/O thread can process in one go.
        max_io_events = 256,

        //  Maximal delay to process command in API thread (in CPU ticks).
        //  3,000,000 ticks equals to 1 - 2 milliseconds on current CPUs.
        //  Note that delay is only applied when there is continuous stream of
        //  messages to process. If not so, commands are processed immediately.
        max_command_delay = 3000000,

        //  Low-precision clock precision in CPU ticks. 1ms. Value of 1000000
        //  should be OK for CPU frequencies above 1GHz. If should work
        //  reasonably well for CPU frequencies above 500MHz. For lower CPU
        //  frequencies you may consider lowering this value to get best
        //  possible latencies.
        clock_precision = 1000000,

        //  On some OSes the signaler has to be emulated using a TCP
        //  connection. In such cases following port is used.
        signaler_port = 5905
    };

}

#endif
