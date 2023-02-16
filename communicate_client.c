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
#include <assert.h>
#include <stdbool.h>

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

    CLIENT *clnt1;
    CLIENT *clnt2;
    CLIENT *clnt3;

    char *cli1_IP;
    char *cli2_IP;
    char *cli3_IP;
    int cli1_Port; 
    int cli2_Port; 
    int cli3_Port; 

    int sockfd1 = create_client_with_IP_and_Port(&cli1_IP, &cli1_Port, 1234);
    int sockfd2 = create_client_with_IP_and_Port(&cli2_IP, &cli2_Port,7777);
    int sockfd3 = create_client_with_IP_and_Port(&cli3_IP, &cli3_Port, 6366);
    clnt1 = rpc_setup(host);
    clnt2 = rpc_setup(host);
    clnt3 = rpc_setup(host);

    //test three clients join
    bool_t * cli1_join;
    cli1_join = join_1(cli1_IP, cli1_Port, clnt1);
    if (cli1_join == (bool_t *) NULL) {
        clnt_perror (clnt1, "call failed");
    }
    

    bool_t *cli2_join;
    cli2_join = join_1(cli2_IP, cli2_Port, clnt2);
    if (cli2_join == (bool_t *) NULL) {
        clnt_perror (clnt2, "call failed");
    }

    bool_t *cli3_join;
    cli3_join = join_1(cli3_IP, cli3_Port, clnt3);
    if (cli3_join == (bool_t *) NULL) {
        clnt_perror (clnt3, "call failed");
    }


    //test subscribe
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

    a = subscribe_1(cli1_IP, cli1_Port, str1, clnt1);
    b = subscribe_1(cli1_IP, cli1_Port, str2, clnt1);
    c = subscribe_1(cli1_IP, cli1_Port, str3, clnt1);
    d = subscribe_1(cli1_IP, cli1_Port, str4, clnt1);
    e = subscribe_1(cli1_IP, cli1_Port, str5, clnt1);

    assert(a != NULL);
    if (*a) {
        assert(*a == 1); 
    } 
    assert(b != NULL);
    if (*b) {
        assert(*b == 1); 
    }
    assert(c != NULL);
    if (*c) {
        assert(*c == 1); 
    }
    assert(d != NULL);
    if (*d) {
        assert(*d == 0); 
    }
    assert(e != NULL);
    if (*e) {
        assert(*a == 0); 
    }

    //test unsubscibe
    bool_t *cli1_unsub1;
    bool_t *cli1_unsub2;
	cli1_unsub1 = unsubscribe_1(cli1_IP, cli1_Port, str2, clnt1);
	cli1_unsub2 = unsubscribe_1(cli1_IP, cli1_Port, str2, clnt1);

    assert(cli1_unsub1 != NULL);
    if (*cli1_unsub1) {
        assert(*cli1_unsub1 == 1); 
    } 
    assert(cli1_unsub2 != NULL);
    if (*cli1_unsub2) {
        assert(*cli1_unsub2 == 0); 
    }

    //test publish
    bool_t  *cli2_publish1;
    bool_t  *cli2_publish2;
    bool_t  *cli3_publish3;
    char * matched_str = "Sports; org; ;soccer game";
    char * another_matched_str = ";ori;org ;blahblah";
    char * unmatched_str = "Politics;;;Trump did what";
    cli2_publish1 = publish_1(matched_str, cli2_IP, cli2_Port,clnt2); //clnt1 receive
    cli2_publish2 = publish_1(unmatched_str, cli2_IP, cli2_Port, clnt2);
    cli2_publish2 = publish_1(another_matched_str, cli2_IP, cli2_Port, clnt2); //clnt1 receive


    bool_t  *p1;
    bool_t  *p7;
    char * empty_str = ";;;";
    p7 = publish_1(str7, cli2_IP, cli2_Port, clnt2);

    assert(p7 != NULL);
    if (*p7) {
        assert(*p7 == 1); 
    }

    //test leave
    bool_t  *cli1_leave_success;
    bool_t  *cli1_leave_failed;
    cli1_leave_success = leave_1(cli2_IP, cli2_Port, clnt2);
    cli1_leave_failed = leave_1(cli2_IP, cli2_Port, clnt2);

    assert(cli1_leave_success != NULL);
    if (*cli1_leave_success) {
        assert(*cli1_leave_success == 1); 
    } 
    assert(cli1_leave_failed != NULL);
    if (*cli1_leave_failed) {
        assert(*cli1_leave_failed == 0); 
    }
	
    /* Create a thread to receive UDP messages from the server */
    pthread_t udp_thread;
    if (pthread_create(&udp_thread, NULL, receive_udp_message, &sockfd1)) {
        printf("\n Error creating UDP message receive thread\n");
        return;
    }

    /* Create a thread to periodically ping the server */
    pthread_t ping_thread;
    if (pthread_create(&ping_thread, NULL, check_server_status, clnt1) < 0) {
        printf("\n Error creating ping thread\n");
        return;
    } 

    clnt_destroy (clnt1);
    clnt_destroy (clnt2);
    clnt_destroy (clnt3);
}

int main(int argc, char *argv[]) {
    char *host;
    host = argv[1];

    communicate_prog_1(host);

    exit(0);
}