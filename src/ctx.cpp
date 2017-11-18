/*
    Copyright (c) 2007-2014 Contributors as noted in the AUTHORS file

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

#include "platform.hpp"
#ifdef NET_HAVE_WINDOWS
#include "windows.hpp"
#else
#include <unistd.h>
#endif

#include <new>
#include <string.h>

#include "ctx.hpp"
#include "io_thread.hpp"
#include "reaper.hpp"
#include "err.hpp"
#include "global_val.h"
#include "tinyxml.h"
#ifdef HAVE_LIBSODIUM
#include <sodium.h>
#endif

#define NET_CTX_TAG_VALUE_GOOD 0xabadcafe
#define NET_CTX_TAG_VALUE_BAD  0xdeadbeef


host::ctx_t::ctx_t () :
    tag (NET_CTX_TAG_VALUE_GOOD),
    starting (true),
    terminating (false),
    slot_count (0),
    slots (NULL),
    io_thread_count (NET_IO_THREADS_DFLT)
{
#ifdef HAVE_FORK
    pid = getpid();
#endif
}

bool host::ctx_t::check_tag ()
{
    return tag == NET_CTX_TAG_VALUE_GOOD;
}

host::ctx_t::~ctx_t ()
{

	reaper->stop();
	delete reaper;

    //  Ask I/O threads to terminate. If stop signal wasn't sent to I/O
    //  thread subsequent invocation of destructor would hang-up.
    for (io_threads_t::size_type i = 0; i != io_threads.size (); i++)
        io_threads [i]->stop ();

    //  Wait till I/O threads actually terminate.
    for (io_threads_t::size_type i = 0; i != io_threads.size (); i++)
        delete io_threads [i];

    //  Deallocate the array of mailboxes. No special work is
    //  needed as mailboxes themselves were deallocated with their
    //  corresponding io_thread/socket objects.

    free (slots);

    //  If we've done any Curve encryption, we may have a file handle
    //  to /dev/urandom open that needs to be cleaned up.
#ifdef HAVE_LIBSODIUM
    randombytes_close();
#endif

    //  Remove the tag, so that the object is considered dead.
    tag = NET_CTX_TAG_VALUE_BAD;
}

void host::ctx_t::start()
{
	if (unlikely (starting)) {
		starting = false;
		//  Initialise the array of mailboxes. Additional three slots are for
		//  zmq_ctx_term thread and reaper thread.
		int ios = io_thread_count;
		slot_count =  ios + 2;
		slots = (mailbox_t**) malloc (sizeof (mailbox_t*) * slot_count);
		alloc_assert (slots);

		//  Initialise the infrastructure for zmq_ctx_term thread.
		slots [term_tid] = &term_mailbox;

		//  Create the reaper thread.
		reaper = new (std::nothrow) reaper_t (this, reaper_tid);
		alloc_assert (reaper);
		slots [reaper_tid] = reaper->get_mailbox ();
		reaper->start ();

		//  Create I/O thread objects and launch them.
		for (int i = 2; i != ios + 2; i++) {
			io_thread_t *io_thread = new (std::nothrow) io_thread_t (this, i);
			alloc_assert (io_thread);
			io_threads.push_back (io_thread);
			slots [i] = io_thread->get_mailbox ();
			io_thread->start ();
		}
	}
}

int host::ctx_t::terminate ()
{
    //  Deallocate the resources.
    delete this;
    return 0;
}

int host::ctx_t::shutdown ()
{
    return 0;
}

void host::ctx_t::send_command (uint32_t tid_, const command_t &command_)
{
    slots [tid_]->send (command_);
}

host::io_thread_t *host::ctx_t::choose_io_thread (uint64_t affinity_)
{
    if (io_threads.empty ())
        return NULL;

    //  Find the I/O thread with minimum load.
    int min_load = -1;
    io_thread_t *selected_io_thread = NULL;
    for (io_threads_t::size_type i = 0; i != io_threads.size (); i++) {
        if (!affinity_ || (affinity_ & (uint64_t (1) << i))) {
            int load = io_threads [i]->get_load ();
            if (selected_io_thread == NULL || load < min_load) {
                min_load = load;
                selected_io_thread = io_threads [i];
            }
        }
    }
    return selected_io_thread;
}

host::object_t *host::ctx_t::get_reaper ()
{
	return reaper;
}
