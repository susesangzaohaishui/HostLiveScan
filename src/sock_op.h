#ifndef SOCK_OP_H
#define SOCK_OP_H
#include <sys/types.h>
 #include <sys/socket.h>
 #include <netdb.h>
#include <strings.h>
#include <arpa/inet.h>
#include <sys/time.h>

struct addrinfo * host_serv(const char *host, const char *serv, int family, int socktype);
char * sock_ntop_host(const struct sockaddr *sa, socklen_t salen);
uint16_t in_cksum(uint16_t *addr, int len);
int Sendto(int fd, const void *ptr, size_t nbytes, int flags,
       const struct sockaddr *sa, socklen_t salen);
void
tv_sub(struct timeval *out, struct timeval *in);
#endif // SOCK_OP_H
