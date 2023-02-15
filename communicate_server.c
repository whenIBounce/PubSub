/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

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


#define MAX_CLIENTS 100
#define MAX_SUBSCRIPTIONS 10
#define MAXSTRING 120

struct ClientInfo {
    char IP[16];
    int Port;
    int numSubscriptions;  // track the number of subscriptions for the client
    char subscriptions[MAX_SUBSCRIPTIONS][MAXSTRING];  // array of subscriptions for the client
};

struct ClientInfo clients[MAX_CLIENTS];
int numClients = 0;

//send UDP message to the specified client
//later change to message
void send_message(char* IP, int Port, char* message) {
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // set to AF_INET to use IPv4
    hints.ai_socktype = SOCK_DGRAM;

    char port_str[6];
    sprintf(port_str, "%d", Port);

    if ((rv = getaddrinfo(IP, port_str, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return;
    }

    // loop through all the results and make a socket
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "talker: failed to create socket\n");
        return;
    }

    if ((numbytes = sendto(sockfd, message, strlen(message), 0,
             p->ai_addr, p->ai_addrlen)) == -1) {
        perror("talker: sendto");
        exit(1);
    }

    freeaddrinfo(servinfo);

    printf("talker: sent %d bytes to %s\n", numbytes, IP);
    close(sockfd);
}


/* Check if the client has already joined */
bool_t clientJoined(char *IP, int Port) {
    for (int i = 0; i < numClients; i++) {
        if (strcmp(clients[i].IP, IP) == 0 && clients[i].Port == Port) {
            return 1;
        }
    }
    return 0;
}

bool_t isArticleValidForSubscription(char *article) {

    char type[MAXSTRING], originator[MAXSTRING], org[MAXSTRING], contents[MAXSTRING];

    memset(type, '\0', MAXSTRING);
    memset(originator, '\0', MAXSTRING);
    memset(org, '\0', MAXSTRING);
    memset(contents, '\0', MAXSTRING);

    int fields = sscanf(article, "%[^;];%[^;];%[^;];%[^;]", type, originator, org, contents);

    // printf("Type: %s  Originator: %s  Org: %s   Contents: %s\n", type, originator, org, contents);

    if (fields != 4 || strlen(article) > MAXSTRING) {
        // Invalid article string
        return 0;
    }

    if (strlen(contents) != 0) {
        // Contents field is not empty
        return 0;
    }

    if (strlen(type) == 0 && strlen(originator) == 0 && strlen(org) == 0) {
        // None of the first three fields are present
        return 0;
    }

    if (strcmp(type, "Sports") != 0 &&
        strcmp(type, "Lifestyle") != 0 &&
        strcmp(type, "Entertainment") != 0 &&
        strcmp(type, "Business") != 0 &&
        strcmp(type, "Technology") != 0 &&
        strcmp(type, "Science") != 0 &&
        strcmp(type, "Politics") != 0 &&
        strcmp(type, "Health") != 0) {
        // Invalid type field
        printf("Invalid type");
        return 0;
    }

    return 1;
}


bool_t isArticleValidForPublication(char *article) {
    char type[MAXSTRING], originator[MAXSTRING], org[MAXSTRING], contents[MAXSTRING];

    memset(type, '\0', MAXSTRING);
    memset(originator, '\0', MAXSTRING);
    memset(org, '\0', MAXSTRING);
    memset(contents, '\0', MAXSTRING);

    int fields = sscanf(article, "%[^;];%[^;];%[^;];%[^;]", type, originator, org, contents);

    if (fields != 4 || strlen(article) > MAXSTRING) {
        // Invalid article string
        return 0;
    }

    if (strlen(contents) == 0) {
        // Contents field is empty
        return 0;
    }

    if (strlen(type) == 0 && strlen(originator) == 0 && strlen(org) == 0) {
        // None of the first three fields are present
        return 0;
    }

    if (strcmp(type, "Sports") != 0 &&
        strcmp(type, "Lifestyle") != 0 &&
        strcmp(type, "Entertainment") != 0 &&
        strcmp(type, "Business") != 0 &&
        strcmp(type, "Technology") != 0 &&
        strcmp(type, "Science") != 0 &&
        strcmp(type, "Politics") != 0 &&
        strcmp(type, "Health") != 0) {
        // Invalid type field
        return 0;
    }

    return 1;
}

/* Check if the article is already subscribed */
bool_t alreadySubscribed(int clientIndex, char *Article) {
    for (int i = 0; i < clients[clientIndex].numSubscriptions; i++) {
        char subscribedType[MAXSTRING], subscribedOriginator[MAXSTRING], subscribedOrg[MAXSTRING];
        sscanf(clients[clientIndex].subscriptions[i], "%[^;];%[^;];%[^;];", subscribedType, subscribedOriginator, subscribedOrg);
        char articleType[MAXSTRING], articleOriginator[MAXSTRING], articleOrg[MAXSTRING];
        sscanf(Article, "%[^;];%[^;];%[^;];", articleType, articleOriginator, articleOrg);
        if (strcmp(subscribedType, articleType) == 0 && strcmp(subscribedOriginator, articleOriginator) == 0 && strcmp(subscribedOrg, articleOrg) == 0) {
            return 1;
        }
    }
    return 0;
}

/* Subscribe the article */
bool_t subscribeArticle(int clientIndex, char *Article) {
    if (clients[clientIndex].numSubscriptions < MAX_SUBSCRIPTIONS) {
        strcpy(clients[clientIndex].subscriptions[clients[clientIndex].numSubscriptions], Article);
        clients[clientIndex].numSubscriptions++;
        printf("Client subscribed to article '%s' successfully\n", Article);
        return 1;
    } else {
        return 0;
    }
}


bool_t *
join_1_svc(char *IP, int Port,  struct svc_req *rqstp)
{
	static bool_t  result;

	/* Check if the client has already joined */
    for (int i = 0; i < numClients; i++) {
        if (strcmp(clients[i].IP, IP) == 0 && clients[i].Port == Port) {
            result = 0;
			printf("Join Failed, Client with IP: %s and Port: %d has already joined the server\n", IP, Port);
            return &result;
        }
    }
	
	/* Add the new client to the list of clients */
    if (numClients < MAX_CLIENTS) {
        strcpy(clients[numClients].IP, IP);
        clients[numClients].Port = Port;
        numClients++;
        result = 1;
    } else {
        result = 0;
    }

    // send_message(IP, Port, "hello wulala"); //test send_message in join

	printf("Client joined with IP: %s and Port: %d\n", IP, Port);

	return &result;
}


bool_t *
leave_1_svc(char *IP, int Port, struct svc_req *rqstp)
{
    static bool_t result;

    /* Find the client in the list of clients */
    int clientIndex = -1;
    for (int i = 0; i < numClients; i++) {
        if (strcmp(clients[i].IP, IP) == 0 && clients[i].Port == Port) {
            clientIndex = i;
            break;
        }
    }

    if (clientIndex == -1) {
        result = 0;
        printf("Leave Failed, Client with IP: %s and Port: %d has not joined the server\n", IP, Port);
        return &result;
    }

    /* Remove the client from the list of clients */
    for (int i = clientIndex; i < numClients - 1; i++) {
        clients[i] = clients[i + 1];
    }
    numClients--;

    result = 1;
    printf("Client with IP: %s and Port: %d has left the server\n", IP, Port);

    return &result;
}

bool_t *subscribe_1_svc(char *IP, int Port, char *Article, struct svc_req *rqstp) {
    static bool_t result;

    if (!clientJoined(IP, Port)) {
        result = 0;
        printf("Subscribe Failed, Client with IP: %s and Port: %d has not joined the server\n", IP, Port);
        return &result;
    }

    int clientIndex = -1;
    for (int i = 0; i < numClients; i++) {
        if (strcmp(clients[i].IP, IP) == 0 && clients[i].Port == Port) {
            clientIndex = i;
            break;
        }
    }

    if(!isArticleValidForSubscription(Article)) {
        result = 0;
        printf("Subscribe Failed, Article '%s' is not valid for subscription.\n", Article);
        return &result;
    }

    if (alreadySubscribed(clientIndex, Article)) {
        result = 0;
        printf("Subscribe Failed, Article '%s' is already subscribed by client with IP: %s and Port: %d\n", Article, IP, Port);
        return &result;
    }

    if (subscribeArticle(clientIndex, Article)) {
        result = 1;
    } else {
        result = 0;
    }

    return &result;
}



bool_t 
*unsubscribe_1_svc(char *IP, int Port, char *Article, struct svc_req *rqstp) {
    static bool_t result;

    if (!clientJoined(IP, Port)) {
        result = 0;
        printf("Unsubscribe Failed, Client with IP: %s and Port: %d has not joined the server\n", IP, Port);
        return &result;
    }

    int clientIndex = -1;
    for (int i = 0; i < numClients; i++) {
        if (strcmp(clients[i].IP, IP) == 0 && clients[i].Port == Port) {
            clientIndex = i;
            break;
        }
    }

    if(!isArticleValidForSubscription(Article)) {
        result = 0;
        printf("Unsubscribe Failed, Article '%s' is not valid for subscription.\n", Article);
        return &result;
    }

    bool_t unsubscribed = 0;
    for (int i = 0; i < clients[clientIndex].numSubscriptions; i++) {
        char subscribedType[MAXSTRING], subscribedOriginator[MAXSTRING], subscribedOrg[MAXSTRING];
        sscanf(clients[clientIndex].subscriptions[i], "%[^;];%[^;];%[^;];", subscribedType, subscribedOriginator, subscribedOrg);
        char articleType[MAXSTRING], articleOriginator[MAXSTRING], articleOrg[MAXSTRING];
        sscanf(Article, "%[^;];%[^;];%[^;];", articleType, articleOriginator, articleOrg);
        if (strcmp(subscribedType, articleType) == 0 && strcmp(subscribedOriginator, articleOriginator) == 0 && strcmp(subscribedOrg, articleOrg) == 0) {
            // Remove the subscription
            for (int j = i; j < clients[clientIndex].numSubscriptions - 1; j++) {
                strcpy(clients[clientIndex].subscriptions[j], clients[clientIndex].subscriptions[j + 1]);
            }
            clients[clientIndex].numSubscriptions--;
            unsubscribed = 1;
            break;
        }
    }

    if (unsubscribed) {
        result = 1;
        printf("Client unsubscribed to article '%s' successfully\n", Article);
    } else {
        result = 0;
        printf("Unsubscribe Failed, Client with IP: %s and Port: %d has not subscribed to article '%s'\n", IP, Port, Article);
    }

    return &result;
}

bool_t *
publish_1_svc(char *Article, char *IP, int Port, struct svc_req *rqstp) {
    static bool_t result;

    // Check if the article is valid for publishing
    if (!isArticleValidForPublication(Article)) {
        result = 0;
        printf("Publish Failed, Article '%s' is not valid for publishing.\n", Article);
        return &result;
    }

    // Find all clients who have subscribed to the publishing article
    for (int i = 0; i < numClients; i++) {
        if (clients[i].numSubscriptions == 0) {
            // Skip clients without any subscriptions
            continue;
        }

        char articleType[MAXSTRING], articleOriginator[MAXSTRING], articleOrg[MAXSTRING], contents[MAXSTRING];
        sscanf(Article, "%[^;];%[^;];%[^;];%s", articleType, articleOriginator, articleOrg, contents);

        for (int j = 0; j < clients[i].numSubscriptions; j++) {
            char subscribedType[MAXSTRING], subscribedOriginator[MAXSTRING], subscribedOrg[MAXSTRING];
            sscanf(clients[i].subscriptions[j], "%[^;];%[^;];%[^;];", subscribedType, subscribedOriginator, subscribedOrg);

            // Check if the client has subscribed to the article being published
            if (strcmp(subscribedType, articleType) == 0 &&
                (subscribedOriginator[0] == '\0' || strcmp(subscribedOriginator, articleOriginator) == 0) &&
                (subscribedOrg[0] == '\0' || strcmp(subscribedOrg, articleOrg) == 0)) {
                // Send the matching article to the client
                send_message(clients[i].IP, clients[i].Port, Article);
                break;
            }
        }
    }

    result = 1;
    printf("Article Published: %s\n", Article);

    return &result;
}





bool_t *
ping_1_svc(struct svc_req *rqstp)
{
	static bool_t  result;

	/*
	 * insert server code heres
	 */
	printf("ping function called\n");
	return &result;
}
