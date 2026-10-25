#ifndef PONG_NET_H
#define PONG_NET_H

#include <stdint.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
typedef struct { 
	float paddle_y;    // Current vertical position of the paddle
	uint32_t sequence;
} NetPacket;

typedef struct { 
	int sockfd; 
	struct sockaddr_in6 peer_addr; 
	socklen_t peer_len;
	int is_bound;
} NetContext;

int pong_net_init(NetContext *ctx, int port); 
int pong_net_set_peer(NetContext *ctx, const char *ip_str, int port); 
int pong_net_send(NetContext *ctx, const NetPacket *packet); 
int pong_net_recv(NetContext *ctx, NetPacket *packet); 
void pong_net_close(NetContext *ctx);

#endif 
