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

    *************************************************************************
    NOTE to contributors. This file comprises the principal public contract
    for ZeroMQ API users (along with zmq_utils.h). Any change to this file
    supplied in a stable release SHOULD not break existing applications.
    In practice this means that the value of constants must not change, and
    that old values may not be reused for new constants.
    *************************************************************************
*/

#ifndef __NET_H_INCLUDED__
#define __NET_H_INCLUDED__

/*  Version macros for compile-time API version detection                     */
#define NET_VERSION_MAJOR 4
#define NET_VERSION_MINOR 0
#define NET_VERSION_PATCH 5

#define NET_MAKE_VERSION(major, minor, patch) \
    ((major) * 10000 + (minor) * 100 + (patch))
#define NET_VERSION \
    NET_MAKE_VERSION(NET_VERSION_MAJOR, NET_VERSION_MINOR, NET_VERSION_PATCH)

#ifdef __cplusplus
extern "C" {
#endif

#if !defined _WIN32_WCE
#include <errno.h>
#endif
#include <stddef.h>
#include <stdio.h>
#if defined _WIN32
#include <winsock2.h>
#endif

/*  Handle DSO symbol visibility                                             */
#if defined _WIN32
#   if defined NET_STATIC
#       define NET_EXPORT
#   elif defined DLL_EXPORT
#       define NET_EXPORT __declspec(dllexport)
#   else
#       define NET_EXPORT __declspec(dllimport)
#   endif
#else
#   if defined __SUNPRO_C  || defined __SUNPRO_CC
#       define NET_EXPORT __global
#   elif (defined __GNUC__ && __GNUC__ >= 4) || defined __INTEL_COMPILER
#       define NET_EXPORT __attribute__ ((visibility("default")))
#   else
#       define NET_EXPORT
#   endif
#endif

/*  Define integer types needed for event interface                          */
#if defined NET_HAVE_SOLARIS || defined NET_HAVE_OPENVMS
#   include <inttypes.h>
#elif defined _MSC_VER && _MSC_VER < 1600
#   ifndef int32_t
typedef __int32 int32_t;
#   endif
#   ifndef uint16_t
typedef unsigned __int16 uint16_t;
#   endif
#   ifndef uint8_t
typedef unsigned __int8 uint8_t;
#   endif
#else
#   include <stdint.h>
#endif


/******************************************************************************/
/*  DDS errors.                                                               */
/******************************************************************************/

/*  A number random enough not to collide with different errno ranges on      */
/*  different OSes. The assumption is that error_t is at least 32-bit type.   */
#define NET_HAUSNUMERO 156384712

/*  On Windows platform some of the standard POSIX errnos are not defined.    */
#ifndef ENOTSUP
#define ENOTSUP (NET_HAUSNUMERO + 1)
#endif
#ifndef EPROTONOSUPPORT
#define EPROTONOSUPPORT (NET_HAUSNUMERO + 2)
#endif
#ifndef ENOBUFS
#define ENOBUFS (NET_HAUSNUMERO + 3)
#endif
#ifndef ENETDOWN
#define ENETDOWN (NET_HAUSNUMERO + 4)
#endif
#ifndef EADDRINUSE
#define EADDRINUSE (NET_HAUSNUMERO + 5)
#endif
#ifndef EADDRNOTAVAIL
#define EADDRNOTAVAIL (NET_HAUSNUMERO + 6)
#endif
#ifndef ECONNREFUSED
#define ECONNREFUSED (NET_HAUSNUMERO + 7)
#endif
#ifndef EINPROGRESS
#define EINPROGRESS (NET_HAUSNUMERO + 8)
#endif
#ifndef ENOTSOCK
#define ENOTSOCK (NET_HAUSNUMERO + 9)
#endif
#ifndef EMSGSIZE
#define EMSGSIZE (NET_HAUSNUMERO + 10)
#endif
#ifndef EAFNOSUPPORT
#define EAFNOSUPPORT (NET_HAUSNUMERO + 11)
#endif
#ifndef ENETUNREACH
#define ENETUNREACH (NET_HAUSNUMERO + 12)
#endif
#ifndef ECONNABORTED
#define ECONNABORTED (NET_HAUSNUMERO + 13)
#endif
#ifndef ECONNRESET
#define ECONNRESET (NET_HAUSNUMERO + 14)
#endif
#ifndef ENOTCONN
#define ENOTCONN (NET_HAUSNUMERO + 15)
#endif
#ifndef ETIMEDOUT
#define ETIMEDOUT (NET_HAUSNUMERO + 16)
#endif
#ifndef EHOSTUNREACH
#define EHOSTUNREACH (NET_HAUSNUMERO + 17)
#endif
#ifndef ENETRESET
#define ENETRESET (NET_HAUSNUMERO + 18)
#endif

/*  Native DDS error codes.                                                   */
#define EFSM (NET_HAUSNUMERO + 51)
#define ENOCOMPATPROTO (NET_HAUSNUMERO + 52)
#define ETERM (NET_HAUSNUMERO + 53)
#define EMTHREAD (NET_HAUSNUMERO + 54)

/*  New API                                                                   */
/*  Context options                                                           */
#define NET_IO_THREADS  1
#define NET_MAX_SOCKETS 2

/*  Default for new contexts                                                  */
#define NET_IO_THREADS_DFLT  4
#define NET_MAX_SOCKETS_DFLT 1023

#define NET_MAX_ASYNCHRONISM_DATA (8192	+ 128)
#define NET_SOCKET_BUFFER         (8192	+ 128)

/*  Send/recv options.                                                        */
#define NET_DONTWAIT 1
#define NET_SNDMORE 2

/*  Security mechanisms                                                       */
#define NET_NULL 0
#define NET_PLAIN 1
#define NET_CURVE 2

/*  Deprecated options and aliases                                            */
#define NET_IPV4ONLY                31
#define NET_DELAY_ATTACH_ON_CONNECT NET_IMMEDIATE
#define NET_NOBLOCK                 NET_DONTWAIT
#define NET_FAIL_UNROUTABLE         NET_ROUTER_MANDATORY
#define NET_ROUTER_BEHAVIOR         NET_ROUTER_MANDATORY

/*  Socket options.                                                           */
#define NET_AFFINITY 4
#define NET_IDENTITY 5
#define NET_SUBSCRIBE 6
#define NET_UNSUBSCRIBE 7
#define NET_RATE 8
#define NET_RECOVERY_IVL 9
#define NET_SNDBUF 11
#define NET_RCVBUF 12
#define NET_RCVMORE 13
#define NET_FD 14
#define NET_EVENTS 15
#define NET_TYPE 16
#define NET_LINGER 17
#define NET_RECONNECT_IVL 18
#define NET_BACKLOG 19
#define NET_RECONNECT_IVL_MAX 21
#define NET_MAXMSGSIZE 22
#define NET_SNDHWM 23
#define NET_RCVHWM 24
#define NET_MULTICAST_HOPS 25
#define NET_RCVTIMEO 27
#define NET_SNDTIMEO 28
#define NET_LAST_ENDPOINT 32
#define NET_ROUTER_MANDATORY 33
#define NET_TCP_KEEPALIVE 34
#define NET_TCP_KEEPALIVE_CNT 35
#define NET_TCP_KEEPALIVE_IDLE 36
#define NET_TCP_KEEPALIVE_INTVL 37
#define NET_TCP_ACCEPT_FILTER 38
#define NET_IMMEDIATE 39
#define NET_XPUB_VERBOSE 40
#define NET_ROUTER_RAW 41
#define NET_IPV6 42
#define NET_MECHANISM 43
#define NET_PLAIN_SERVER 44
#define NET_PLAIN_USERNAME 45
#define NET_PLAIN_PASSWORD 46
#define NET_CURVE_SERVER 47
#define NET_CURVE_PUBLICKEY 48
#define NET_CURVE_SECRETKEY 49
#define NET_CURVE_SERVERKEY 50
#define NET_PROBE_ROUTER 51
#define NET_REQ_CORRELATE 52
#define NET_REQ_RELAXED 53
#define NET_CONFLATE 54
#define NET_ZAP_DOMAIN 55
#define BUFSIZE   1500

#undef  NET_EXPORT

#ifdef __cplusplus
}
#endif

#endif

