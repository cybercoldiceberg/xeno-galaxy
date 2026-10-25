#include "pong_net.h" 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <fcntl.h>
#include <errno.h> 

int pong_net_init(NetContext *ctx, int port) { 
	ctx->sockfd = socket(AF_INET6, SOCK_DGRAM, 0); 
	if (ctx->sockfd < 0) { 
		perror("socket"); 
		return -1; 
	} 
	int flags = fcntl(ctx->sockfd, F_GETFL, 0); 
	if (flags < 0 || fcntl(ctx->sockfd, F_SETFL, flags | O_NONBLOCK) < 0) { 
		perror("fcntl nonblocking");
		close(ctx->sockfd); 
		return -1;
	} 
	struct sockaddr_in6 addr; 
	memset(&addr, 0, sizeof(addr)); 
	addr.sin6_family = AF_INET6; 
	addr.sin6_addr = in6addr_any;
	addr.sin6_port = htons(port); 

	if (bind(ctx->sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) { 
		perror("bind"); 
		close(ctx->sockfd); 
		return -1; 
	} 
	ctx->is_bound = 1;
	ctx->peer_len = sizeof(ctx->peer_addr); 
	return 0;
} 
int pong_net_set_peer(NetContext *ctx, const char *ip_str, int port) { 
	memset(&ctx->peer_addr, 0, sizeof(ctx->peer_addr)); 
	ctx->peer_addr.sin6_family = AF_INET6;
	ctx->peer_addr.sin6_port = htons(port); 
	if (inet_pton(AF_INET6, ip_str, &ctx->peer_addr.sin6_addr) <= 0) { 
		fprintf(stderr, "Invalid IP address format: %s\n", ip_str); 
        return -1;
    } 
    ctx->peer_len = sizeof(ctx->peer_addr); 
    return 0;
}

int pong_net_send(NetContext *ctx, const NetPacket *packet) { 
	ssize_t sent = sendto(ctx->sockfd, packet, sizeof(NetPacket), 0, 
							(struct sockaddr *)&ctx->peer_addr, ctx->peer_len);
    if (sent < 0) {
    	if (errno == EWOULDBLOCK || errno == EAGAIN) { 
    		return 0; 
    	} 
    	perror("sendto");
    	return -1; 
    } 
    return 0;
}
int pong_net_recv(NetContext *ctx, NetPacket *packet) { 
	struct sockaddr_in6 src_addr;
	socklen_t src_len = sizeof(src_addr); 

	ssize_t received = recvfrom(ctx->sockfd, packet, sizeof(NetPacket), 0, 
								(struct sockaddr *)&src_addr, &src_len); 
	if (received < 0) { 
		if (errno == EWOULDBLOCK || errno == EAGAIN) { 
			return 0;
		} 
		perror("recvfrom"); 
		return -1;
	}
	return (int)received;
} 
void pong_net_close(NetContext *ctx) { 
	if (ctx->sockfd >= 0) { 
		close(ctx->sockfd); 
		ctx->sockfd = -1; 
	} 
}	
