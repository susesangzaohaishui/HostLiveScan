#include "sock_op.h"
#include "debug.h"
#include <libio.h>
#include <stdio.h>
#include <sys/un.h>

struct addrinfo *
host_serv(const char *host, const char *serv, int family, int socktype)
{
    int				n;
    struct addrinfo	hints, *res;

    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_flags = AI_CANONNAME;	/* always return canonical name */
    hints.ai_family = family;		/* AF_UNSPEC, AF_INET, AF_INET6, etc. */
    hints.ai_socktype = socktype;	/* 0, SOCK_STREAM, SOCK_DGRAM, etc. */

    if ( (n = getaddrinfo(host, serv, &hints, &res)) != 0)
        return(NULL);

    return(res);	/* return pointer to first on linked list */
}

char *
sock_ntop_host(const struct sockaddr *sa, socklen_t salen)
{
    static char str[128];		/* Unix domain is largest */

    switch (sa->sa_family) {
    case AF_INET: {
        struct sockaddr_in	*sin = (struct sockaddr_in *) sa;

        if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
            return(NULL);
        return(str);
    }

#ifdef	IPV6
    case AF_INET6: {
        struct sockaddr_in6	*sin6 = (struct sockaddr_in6 *) sa;

        if (inet_ntop(AF_INET6, &sin6->sin6_addr, str, sizeof(str)) == NULL)
            return(NULL);
        return(str);
    }
#endif

#ifdef	AF_UNIX
    case AF_UNIX: {
        struct sockaddr_un	*unp = (struct sockaddr_un *) sa;

            /* OK to have no pathname bound to the socket: happens on
               every connect() unless client calls bind() first. */
        if (unp->sun_path[0] == 0)
            strcpy(str, "(no pathname bound)");
        else
            snprintf(str, sizeof(str), "%s", unp->sun_path);
        return(str);
    }
#endif

#ifdef	HAVE_SOCKADDR_DL_STRUCT
    case AF_LINK: {
        struct sockaddr_dl	*sdl = (struct sockaddr_dl *) sa;

        if (sdl->sdl_nlen > 0)
            snprintf(str, sizeof(str), "%*s",
                     sdl->sdl_nlen, &sdl->sdl_data[0]);
        else
            snprintf(str, sizeof(str), "AF_LINK, index=%d", sdl->sdl_index);
        return(str);
    }
#endif
    default:
        snprintf(str, sizeof(str), "sock_ntop_host: unknown AF_xxx: %d, len %d",
                 sa->sa_family, salen);
        return(str);
    }
    return (NULL);
}

uint16_t
in_cksum(uint16_t *addr, int len)
{
    int				nleft = len;
    uint32_t		sum = 0;
    uint16_t		*w = addr;
    uint16_t		answer = 0;

    /*
     * Our algorithm is simple, using a 32 bit accumulator (sum), we add
     * sequential 16 bit words to it, and at the end, fold back all the
     * carry bits from the top 16 bits into the lower 16 bits.
     */
    while (nleft > 1)  {
        sum += *w++;
        nleft -= 2;
    }

        /* 4mop up an odd byte, if necessary */
    if (nleft == 1) {
        *(unsigned char *)(&answer) = *(unsigned char *)w ;
        sum += answer;
    }

        /* 4add back carry outs from top 16 bits to low 16 bits */
    sum = (sum >> 16) + (sum & 0xffff);	/* add hi 16 to low 16 */
    sum += (sum >> 16);			/* add carry */
    answer = ~sum;				/* truncate to 16 bits */
    return(answer);
}

int
Sendto(int fd, const void *ptr, size_t nbytes, int flags,
       const struct sockaddr *sa, socklen_t salen)
{
    int res;
    if ((res = sendto(fd, ptr, nbytes, flags, sa, salen)) != (ssize_t)nbytes)
       debug_test("sendto error");
    return res;
}

void
tv_sub(struct timeval *out, struct timeval *in)
{
    if ( (out->tv_usec -= in->tv_usec) < 0) {	/* out -= in */
        --out->tv_sec;
        out->tv_usec += 1000000;
    }
    out->tv_sec -= in->tv_sec;
}
