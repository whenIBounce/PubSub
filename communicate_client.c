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

#define MYPORT "8083"
#define MYPORT2 "2333"
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

int create_client_with_IP_and_Port(char **Client_IP, int* Client_Port, int Hardcoded_port_num){
     /* Create a socket */
    int sockfd;
    struct sockaddr_in cli_addr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return 0;
    }

    memset(&cli_addr, '0', sizeof(cli_addr));

    /* Hardcoded IP and Port for every client*/
    cli_addr.sin_family = AF_INET;
    cli_addr.sin_port = htons(Hardcoded_port_num);
    cli_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //same IP for diff clients

    /* Bind the socket to a specific port */
    if (bind(sockfd, (const struct sockaddr *) &cli_addr, sizeof(cli_addr)) < 0) {
        printf("\nBind failed\n");
        return 0;
    }

    /* Set IP and Port */
    struct sockaddr_in tmp_addr;
    socklen_t len = sizeof(tmp_addr);
    getsockname(sockfd, (struct sockaddr *) &tmp_addr, &len);
    *Client_IP = inet_ntoa(tmp_addr.sin_addr);
    *Client_Port = ntohs(tmp_addr.sin_port);

    printf("Client IP: %s, Port: %d\n", *Client_IP, *Client_Port);

    return sockfd;
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
        printf("Received subsribed article from server: %s\n", buf);
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
            printf("Server is down\n");
            clnt_destroy(clnt);
            exit(1);
        }else{
            printf("Server still running\n");
        }
    }
}

void communicate_prog_1(char *host) {
    CLIENT *clnt;

    char *client_IP;
    int client_Port; 

    int sockfd = create_client_with_IP_and_Port(&client_IP, &client_Port, 1234);

    bool_t *result_1;
    bool_t  *result_2;



	bool_t  *result_3;


    bool_t  *result_4;


    // /* Create a socket */
    // int sockfd;
    // struct sockaddr_in cli_addr;

    // if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    //     printf("\n Socket creation error \n");
    //     return;
    // }

    // memset(&cli_addr, '0', sizeof(cli_addr));

    // /* Hardcoded IP and Port for every client*/
    // cli_addr.sin_family = AF_INET;
    // cli_addr.sin_port = htons(atoi(MYPORT));
    // cli_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // /* Bind the socket to a specific port */
    // if (bind(sockfd, (const struct sockaddr *) &cli_addr, sizeof(cli_addr)) < 0) {
    //     printf("\nBind failed\n");
    //     return;
    // }

    // /* Set IP and Port */
    // struct sockaddr_in tmp_addr;
    // socklen_t len = sizeof(tmp_addr);
    // getsockname(sockfd, (struct sockaddr *) &tmp_addr, &len);
    // client_IP = inet_ntoa(tmp_addr.sin_addr);
    // client_Port = ntohs(tmp_addr.sin_port);

    // printf("Client IP: %s, Port: %d\n", client_IP, client_Port);



    /* Send RPC request to the server */
    clnt = rpc_setup(host);
    result_1 = join_1(client_IP, client_Port, clnt);
    if (result_1 == (bool_t *) NULL) {
        clnt_perror (clnt, "call failed");
    }

    //set up another client
    CLIENT *clnt2;
    clnt2 = clnt_create(host, COMMUNICATE_PROG, COMMUNICATE_VERSION, "udp");
    if (clnt2 == NULL) {
        clnt_pcreateerror(host);
        exit(1);
    }

        char *client_IP2;
    int client_Port2; 

     /* Create a socket */
    int sockfd2;
    struct sockaddr_in cli2_addr;

    if ((sockfd2 = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return;
    }

    memset(&cli2_addr, '0', sizeof(cli2_addr));

    /* Hardcoded IP and Port for every client*/
    cli2_addr.sin_family = AF_INET;
    cli2_addr.sin_port = htons(atoi(MYPORT2));
    cli2_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    /* Bind the socket to a specific port */
    if (bind(sockfd2, (const struct sockaddr *) &cli2_addr, sizeof(cli2_addr)) < 0) {
        printf("\nBind failed\n");
        return;
    }

    /* Set IP and Port */
    struct sockaddr_in tmp_addr2;
    socklen_t len2 = sizeof(tmp_addr2);
    getsockname(sockfd2, (struct sockaddr *) &tmp_addr2, &len2);
    client_IP2 = inet_ntoa(tmp_addr2.sin_addr);
    client_Port2 = ntohs(tmp_addr2.sin_port);

    printf("Client2 IP: %s, Port: %d\n", client_IP2, client_Port2);

        bool_t *join_res;
        join_res = join_1(client_IP2, client_Port2, clnt2);
    if (join_res == (bool_t *) NULL) {
        clnt_perror (clnt, "call failed");
    }

    ////////////////////////////////



    bool_t  *a;
    bool_t  *b;
    bool_t  *c;
    bool_t  *d;
    bool_t  *e;
    char * str1 = "Sports; org; ;";
    char * str2 = "Politics;ori; ;";
    char * str3 = ";ori;org ;";
    char * str4 = ";ori; ;contents";
    char * str5 = "gaga;ss;ss;ss";
    char * str6 = ";;;whatabout";
    char * str7 = "Entertainment;Carrie's Eating Club;CSBSJU;NoEntertainmentInCSBSJU";

    a = subscribe_1(client_IP, client_Port, str1, clnt);
    b = subscribe_1(client_IP, client_Port, str2, clnt);
    c = subscribe_1(client_IP, client_Port, str3, clnt);
    d = subscribe_1(client_IP, client_Port, str4, clnt);
    e = subscribe_1(client_IP, client_Port, str5, clnt);

    //client1 unsub
    bool_t *cli1_unsub1;
    bool_t *cli1_unsub2;
	cli1_unsub1 = unsubscribe_1(client_IP, client_Port, str2, clnt);
	cli1_unsub2 = unsubscribe_1(client_IP, client_Port, str2, clnt);



    //client2 publich
    bool_t  *cli2_publish1;
    bool_t  *cli2_publish2;
    char * matched_str = "Sports; org; ;soccer game";
    char * unmatched_str = "Politics;;;Trump did what";
    cli2_publish1 = publish_1(matched_str, client_IP2, client_Port2,clnt2);
    cli2_publish2 = publish_1(unmatched_str, client_IP2, client_Port2, clnt2);

    bool_t  *p1;
    bool_t  *p2;
    bool_t  *p3;
    bool_t  *p4;
    bool_t  *p5;
    bool_t  *p6;
    bool_t  *p7;
    p1 = publish_1(str1, client_IP, client_Port,clnt);
    p2 = publish_1(str2, client_IP, client_Port, clnt);
    p3 = publish_1(str3, client_IP, client_Port, clnt);
    p4 = publish_1(str4, client_IP, client_Port, clnt);
    p5 = publish_1(str5, client_IP, client_Port, clnt);
    p6 = publish_1(str6, client_IP, client_Port, clnt);
    p7 = publish_1(str7, client_IP, client_Port, clnt);

    /* Create a thread to receive UDP messages from the server */
    pthread_t udp_thread;
    if (pthread_create(&udp_thread, NULL, receive_udp_message, &sockfd)) {
        printf("\n Error creating UDP message receive thread\n");
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