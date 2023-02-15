# PubSub

## How to run:

Open a terminal window:

1. ```make -f Makefile.communicate```

2. ```gcc -o communicate_client2 communicate_client2.c communicate_clnt.c communicate_xdr.c -pthread```

3. ```./communicate_server```

Then open another terminal window:
```./communicate_client localhost```

Then open another terminal window:
```./communicate_client2 localhost```

## Design document describing each component.

### Server

The server uses RPC and UDP sockets to handle client-server communication. The server maintains an array of clients, where each client is represented by its IP address, port number, and a list of article types it has subscribed to. The server checks the validity of the article strings received from the client, and publishes the article to all subscribed clients, or to a specific client.

```
struct ClientInfo {
    char IP[16];
    int Port;
    int numSubscriptions;  // track the number of subscriptions for the client
    char subscriptions[MAX_SUBSCRIPTIONS][MAXSTRING];  // array of subscriptions for the client
};
```

The system uses the following RPC functions:

1. `bool_t *join_1_svc(char *IP, int Port, struct svc_req *rqstp)`: The server adds the client to the list of clients if it has not already joined the system.

2. `bool_t *leave_1_svc(char *IP, int Port, struct svc_req *rqstp)`: The server removes the client from the list of clients if it has joined the system.

3. `bool_t *subscribe_1_svc(char *IP, int Port, char *Article, struct svc_req *rqstp)`: The server checks 
    1. if the client has joined, 
    2. then the validity of the article string, 
    3. the client has not already subscribed to it. 
    4. adds the article type to the client's subscription list if it is valid and the client has not already subscribed to it.

4. `bool_t *unsubscribe_1_svc(char *IP, int Port, char *Article, struct svc_req *rqstp)`: The server checks the validity of the article string, and removes the article type from the client's subscription list if it is valid and the client has subscribed to it.

5. `bool_t *publish_all_1_svc(char *IP, int Port, char *Article, struct svc_req *rqstp)`: The server publishes an article to all subscribed clients. The server checks 
    1. the validity of the article string, 
    2. and sends the article to all clients who have subscribed to the article type.

6. `bool_t *publish_1_svc(char *IP, int Port, char *Article, char *ClientIP, int ClientPort, struct svc_req *rqstp)`: The server publishes an article to a specific client. The server checks
    1. if the client has joined, 
    2. then the validity of the article string, 
    3. the client has already subscribed to it. 
    Then it will, 
    1. get each subscibed client info
    2. send the article to each subscribed clients. 

The code includes several helper functions that are used in the RPC functions:

1. `send_message(char* IP, int Port, char* message)` - This function sends a UDP message to a specified client.

2. `bool_t clientJoined(char *IP, int Port)` - This function checks if a client with a specified IP and Port has already joined the server.

3. `bool_t isArticleValidForSubscription(char *article)` - This function checks if a given article is valid for subscription. An article is valid for subscription if it has a valid type, originator, and organization, and the contents field is empty.

4. `bool_t isArticleValidForPublication(char *article)` - This function checks if a given article is valid for publication. An article is valid for publication if it has a valid type, originator, and organization, and the contents field is non-empty.

5. `bool_t alreadySubscribed(int clientIndex, char *Article)` - This function checks if a specified article is already subscribed by a given client.

6. `bool_t subscribeArticle(int clientIndex, char *Article)` - This function subscribes a given article for a specified client.

7. `bool_t unsubscribeArticle(int clientIndex, char *Article)` - This function unsubscribes a given article for a specified client.

### Client

The client implementation for a news subscription service uses Remote Procedure Calls (RPC).

The code creates a UDP socket and binds it to a specific port. The IP address and port are **hardcoded** for every client. It then sends RPC requests to the server to join, subscribe, unsubscribe, publich, leave, etc. 

The client also creates two threads. The first thread listens for UDP messages from the server and prints them out. The second thread periodically pings the server to check its status. If the server is down, the client exits the program.

The main function takes the IP address of the server as an argument and calls the communicate_prog_1 function, which sets up the client and sends the necessary RPC requests to the server.