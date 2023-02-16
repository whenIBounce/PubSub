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
    // printf("wulalalalalalalalalaalaalalalalalala\n");
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
            printf("Server is down\n");
            clnt_destroy(clnt);
            exit(1);
        }
    }
}

void communicate_prog_1(char *host) {
    CLIENT *clnt;

    char *client_IP;
    int client_Port; 

    bool_t *result_1;
    bool_t  *result_2;
    bool_t  *result_3;
    bool_t  *result_4;
    bool_t  *result_5;
    bool_t  *result_6;


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
    client_IP = inet_ntoa(tmp_addr.sin_addr);
    client_Port = ntohs(tmp_addr.sin_port);

    printf("Client IP: %s, Port: %d\n", client_IP, client_Port);


    /* Send RPC request to the server */
    clnt = rpc_setup(host);
    result_1 = join_1(client_IP, client_Port, clnt);
    if (result_1 == (bool_t *) NULL) {
        clnt_perror (clnt, "call failed");
    }

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

    
    // char * str1 = "Sports; org; ;";
    // char * str2 = "Politics;ori; ;";
    // char * str3 = ";ori;org ;";
    // char * str4 = ";ori; ;contents";
    // char * str5 = "gaga;ss;ss;ss";
    // char * str6 = ";;;whatabout";
    // char * str7 = "Entertainment;Carrie's Eating Club;CSBSJU;NoEntertainmentInCSBSJU";

    char article[MAXSTRING];
    char command[MAXSTRING];
    while(1){
        scanf("%s %[^\n]s", command,article);
    if (!strcmp(command, "subscribe")) {
        result_3 = subscribe_1(client_IP, client_Port, article, clnt);
        if (result_3 == (bool_t *) NULL) {
        clnt_perror (clnt, "call failed");
    }
    } else if (!strcmp(command, "unsubscribe")) {
        result_4 = unsubscribe_1(client_IP, client_Port, article, clnt);
        if (result_4 == (bool_t *) NULL) {
        clnt_perror (clnt, "call failed");
    }
    } else if (!strcmp(command, "leave")) {
        result_2 = leave_1(client_IP, client_Port, clnt);
    } else if (!strcmp(command, "publish")) {
        result_5 = publish_1(article, client_IP, client_Port, clnt);
        if (result_5 == (bool_t *) NULL) {
        clnt_perror (clnt, "call failed");
    }
    } else if (!strcmp(command, "ping")) {
        result_6 = ping_1(clnt);
        if (result_6 == (bool_t *) NULL) {
        clnt_perror (clnt, "call failed");
    }
    } else {
        printf("Invalid command: %s\n", command);
        return 1;
    }
    memset(command,'\0', sizeof(char)*MAXSTRING); //clean up input buffer every time
    memset(article,'\0', sizeof(char)*MAXSTRING);
    }

    

    if((pthread_join(ping_thread, NULL)) != 0){
      printf("ERROR : Fail to join ping thread.\n");
    }

    if((pthread_join(udp_thread, NULL)) != 0){
      printf("ERROR : Fail to join receive thread.\n");
    }

    clnt_destroy (clnt);
}

int main(int argc, char *argv[]) {
    char *host;
    host = argv[1];

    communicate_prog_1(host);

    exit(0);
}