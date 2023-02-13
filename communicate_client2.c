#include "communicate.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <unistd.h>

#define MYPORT "8084"
#define MAXBUFLEN 100

CLIENT *rpc_setup(char *host) {
    CLIENT *clnt;

    clnt = clnt_create(host, COMMUNICATE_PROG, COMMUNICATE_VERSION, "udp");
    if (clnt == NULL) {
        clnt_pcreateerror(host);
        exit(1);
    }

    return clnt;
}


void* receive_udp_message(void* arg) {
    int sockfd = *(int*)arg;
    char buf[MAXBUFLEN];
    struct sockaddr_storage sender_addr;
    socklen_t addr_len = sizeof(sender_addr);

    while (1) {
        int numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0, (struct sockaddr *)&sender_addr, &addr_len);
        if (numbytes == -1) {
            perror("recvfrom");
            exit(1);
        }
        buf[numbytes] = '\0';
        printf("Received UDP message from server: %s\n", buf);
    }

    pthread_exit(NULL);
}

void *check_server_status(void* arg)
{
    CLIENT *clnt = (CLIENT *)arg;
    while(1)
    {
        sleep(2);
        if (ping_1(clnt) == (bool_t *)NULL)
        {
            printf("server is down\n");
            clnt_destroy(clnt);
            exit(1);
        }
    }
}


void communicate_prog_1(char *host) {
    CLIENT *clnt;

    bool_t *result_1;
    char *join_1_IP;
    int join_1_Port;

    bool_t  *result_2;
	char *leave_1_IP;
	int leave_1_Port;


	bool_t  *result_3;
	char *subscribe_1_IP;
	int subscribe_1_Port;
	char *subscribe_1_Article = "Health;oo;haha;";

    bool_t  *result_4;
	char *unsubscribe_1_IP;
	int unsubscribe_1_Port;
	char *unsubscribe_1_Article = "Health;oo;haha;";

    bool_t  *result_5;
	char *publish_1_Article = "Health;oo;haha;healthystuff";
	char *publish_1_IP;
	int publish_1_Port;

    /* Create a socket */
    int sockfd;
    struct sockaddr_in cli_addr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return;
    }

    memset(&cli_addr, '0', sizeof(cli_addr));

    /* Hardcoded IP and Port for every client*/
    cli_addr.sin_family = AF_INET;
    cli_addr.sin_port = htons(atoi(MYPORT));
    cli_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    /* Bind the socket to a specific port */
    if (bind(sockfd, (const struct sockaddr *) &cli_addr, sizeof(cli_addr)) < 0) {
        printf("\nBind failed\n");
        return;
    }

    /* Set IP and Port */
    struct sockaddr_in tmp_addr;
    socklen_t len = sizeof(tmp_addr);
    getsockname(sockfd, (struct sockaddr *) &tmp_addr, &len);
    join_1_IP = inet_ntoa(tmp_addr.sin_addr);
    join_1_Port = ntohs(tmp_addr.sin_port);
    leave_1_IP = join_1_IP;
    leave_1_Port = join_1_Port;
    subscribe_1_IP = join_1_IP;
    subscribe_1_Port = join_1_Port;
    unsubscribe_1_IP = join_1_IP;
    unsubscribe_1_Port = join_1_Port;
    publish_1_IP = join_1_IP;
    publish_1_Port = join_1_Port;

    printf("Client IP: %s, Port: %d\n", join_1_IP, join_1_Port);

    /* Send RPC request to the server */
    clnt = rpc_setup(host);
    result_1 = join_1(join_1_IP, join_1_Port, clnt);
    if (result_1 == (bool_t *) NULL) {
        clnt_perror (clnt, "join call failed");
    }
    
	result_5 = publish_1(publish_1_Article, publish_1_IP, publish_1_Port, clnt);
	if (result_5 == (bool_t *) NULL) {
		clnt_perror (clnt, "publish call failed");
	}


    /* Create a thread to receive UDP messages from the server */
    pthread_t thread;
    if (pthread_create(&thread, NULL, receive_udp_message, &sockfd)) {
        printf("\n Error creating receive thread\n");
        return;
    }

    /* Create a thread to periodically ping the server */
    pthread_t ping_thread;
    if (pthread_create(&ping_thread, NULL, check_server_status, clnt) < 0) {
        printf("\n Error creating ping thread\n");
        return;
    } 

    clnt_destroy (clnt);
}

int main(int argc, char *argv[]) {
    char *host;
    host = argv[1];

    communicate_prog_1(host);

    exit(0);
}