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

#ifndef __NET_POLLER_HPP_INCLUDED__
#define __NET_POLLER_HPP_INCLUDED__

#include "platform.hpp"

#if defined NET_FORCE_SELECT
#define NET_USE_SELECT
#include "select.hpp"
#elif defined NET_FORCE_POLL
#define NET_USE_POLL
#include "poll.hpp"
#elif defined NET_FORCE_EPOLL
#define NET_USE_EPOLL
#include "epoll.hpp"
#elif defined NET_FORCE_DEVPOLL
#define NET_USE_DEVPOLL
#include "devpoll.hpp"
#elif defined NET_FORCE_KQUEUE
#define NET_USE_KQUEUE
#include "kqueue.hpp"
#elif defined NET_HAVE_LINUX
#define NET_USE_EPOLL
#include "epoll.hpp"
#elif defined NET_HAVE_WINDOWS
#define NET_USE_SELECT
#include "select.hpp"
#elif defined NET_HAVE_FREEBSD
#define NET_USE_KQUEUE
#include "kqueue.hpp"
#elif defined NET_HAVE_OPENBSD
#define NET_USE_KQUEUE
#include "kqueue.hpp"
#elif defined NET_HAVE_NETBSD
#define NET_USE_KQUEUE
#include "kqueue.hpp"
#elif defined NET_HAVE_SOLARIS
#define NET_USE_DEVPOLL
#include "devpoll.hpp"
#elif defined NET_HAVE_OSX
#define NET_USE_KQUEUE
#include "kqueue.hpp"
#elif defined NET_HAVE_QNXNTO
#define NET_USE_POLL
#include "poll.hpp"
#elif defined NET_HAVE_AIX
#define NET_USE_POLL
#include "poll.hpp"
#elif defined NET_HAVE_HPUX
#define NET_USE_DEVPOLL
#include "devpoll.hpp"
#elif defined NET_HAVE_OPENVMS
#define NET_USE_SELECT
#include "select.hpp"
#elif defined NET_HAVE_CYGWIN
#define NET_USE_SELECT
#include "select.hpp"
#else
#error Unsupported platform
#endif

#endif
