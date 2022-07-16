//Name: Quentin Jennings
//UCID: 30089570
//Section: L01 (Carey)

//creates and allocates address to memory
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include <regex.h>

#define BUFFER_SIZE 8192 //the size of our request/response msgs in bytes (so this is 8 kilobytes)
#define HOSTNAME_SIZE 256 //size of hostname array, 256 is plenty
#define PORT "4421" //port number

//global variables
char request_msg[BUFFER_SIZE], response_msg[BUFFER_SIZE], hostname[HOSTNAME_SIZE]; //buffers and hostname
int status, listen_socket, client_socket, server_socket; //socket and status variables
struct addrinfo base, *pclient, *pserver; //a nice structure that deals with address info stuff for us
ssize_t num_bytes_s, num_bytes_r; //stores the number of bytes
regex_t regex;
regmatch_t match[1];

//function declarations
void initialize_address();
void get_hostname();
void listen_stuff();
int client_stuff();
void server_stuff();

void get_hostname();
void happy_filter();
void clown_filter();
const char* clown_randomizer();

//main program - calls other functions to do the work
int main(){ 
    srand(time(NULL)); //seeds randomizer for random clown generation
    regcomp(&regex, "happy", REG_ICASE); //compiles regex pattern for case-insensitive happy

    listen_stuff(); //sets up listen socket

    //main program loop
    while(1) {
        //clears our buffers after each request
        memset(request_msg, 0, BUFFER_SIZE);
        memset(response_msg, 0, BUFFER_SIZE);
        memset(hostname, 0, HOSTNAME_SIZE);

        printf("\n--------------------------------------------------------\nWaiting for client http request...\n");
        status = client_stuff(); //sets up client socket and recieves their request
        if(status == 1) { //skips the rest of the loop if we don't care abt the req
            continue;
        }
        printf("Client http request received!\n");
        printf("===================\n%s==================\n", request_msg);

        clown_filter(); //filters http requests for .jpg images
        get_hostname(); //grabs the hostname from request
        printf("Retrieved host: \"%s\"\n", hostname);

        server_stuff(); //sets up server socket using hostname and sends request to client

        printf("Request sent back to client, done!\n");

        close(client_socket); //closes client socket
        close(server_socket); //closes server socket
    }
}

void listen_stuff() {
    //--ADDRESS INITIALIZATION--
    memset(&base, 0, sizeof(base)); //im afraid to remove this at this point
    base.ai_family = AF_UNSPEC; //chooses ipv4 or ipv6 for us, no thinking needed
    base.ai_socktype = SOCK_STREAM; //TCP
    base.ai_flags = AI_PASSIVE; //listening

    //gets the info needed to create the listen socket
    //thank goodness i discovered getaddrinfo :]
    status = getaddrinfo(NULL, PORT, &base, &pclient);
    if(status) {
        printf("Error getting address info for proxy: getaddrinfo() call failed.");
        exit(1);
    }

    //--LISTEN SOCKET--
    listen_socket = socket(pclient->ai_family, pclient->ai_socktype, pclient->ai_protocol); //creates the listening socket
    if(listen_socket == -1) {
        printf("Error creating listen socket: socket() call failed.");
        exit(1);
    }

    status = bind(listen_socket, pclient->ai_addr, pclient->ai_addrlen); //binds the socket (the addrinfo has everything we need!)
    if(status == -1) {
        printf("Error binding listen socket: bind() call failed. Try waiting or changing the port.");
        close(listen_socket);
        exit(1);
    }

    status = listen(listen_socket, 16); //sets socket to listen
    if(status == -1) {
        printf("Error listening listen socket: listen() call failed.");
        close(listen_socket);
        exit(1);
    }
}

int client_stuff() {
    //--CLIENT REQUEST--
    client_socket = accept(listen_socket, NULL, NULL); //accepts a single incoming connection
    if(client_socket == -1) {
        printf("Error accepting connection on client socket: accept() call failed.");
        close(listen_socket);
        exit(1);
    }

    num_bytes_r = recv(client_socket, request_msg, BUFFER_SIZE, 0); //recieves the incoming http request message
    if(num_bytes_r == -1) {
        printf("Error recieving HTTP request : recv() call failed.");
        close(listen_socket);
        close(client_socket);
        exit(1);
    }
    else if(num_bytes_r == 0) { //if the request is empty
        printf("Warning: client sent request of 0 bytes, ignored.");
        close(client_socket);
        return 1;
    }
    if(strncmp(request_msg, "GET", 3) != 0) { //if the request isn't a GET, ignore it
        printf("Warning: non-GET request sent and ignored.");
        close(client_socket);
        return 1;
    }

    return 0;
}

void get_hostname() {
    //--GET HOST NAME--
    char* parsed_str = strstr(request_msg, "Host:"); //brings us to the host tag
    if(parsed_str) {
        sscanf(parsed_str, "%*s %s", hostname); //and retrieves the hostname from it
    }
    else {
        printf("Error: could not retrieve host name.");
        close(listen_socket);
        close(client_socket);
        exit(1);
    }
}

void server_stuff() {
    //--SERVER SOCKET--
    base.ai_flags = 0; //removes listening flag (server socket won't want it)
    
    status = getaddrinfo(hostname, "http", &base, &pserver); //gets all the address info we need, namely ip address
    if(status != 0) {
        printf("Error getting address info for server: getaddrinfo() call failed.");
        printf(" Host: %s", hostname);
        close(listen_socket);
        close(client_socket);
        exit(1);
    }
    
    server_socket = socket(pserver->ai_family, pserver->ai_socktype, pserver->ai_protocol); //creates the listening socket
    if(server_socket == -1) {
        printf("Error creating server socket: socket() call failed.");
        close(listen_socket);
        close(client_socket);
        exit(1);
    }

    status = connect(server_socket, pserver->ai_addr, pserver->ai_addrlen); //connects to server
    if(status != 0) {
        printf("Error connecting to server: connect() call failed.");
        close(listen_socket);
        close(client_socket);
        close(server_socket);
        exit(1);
    }

    //--SEND REQUEST TO SERVER--
    num_bytes_s = send(server_socket, request_msg, num_bytes_r, 0);
    if(num_bytes_s == -1) {
        close(listen_socket);
        close(client_socket);
        close(server_socket);
        printf("Error sending http request: send() call failed.");
        exit(1);
    }

    //--RECEIVE RESPONSE FROM SERVER
    while((num_bytes_r = recv(server_socket, response_msg, BUFFER_SIZE, 0)) > 0) {

        //--MODIFY RESPONSE--
        happy_filter(); //replaces happy with silly

        //--SEND RESPONSE TO CLIENT--
        num_bytes_s = send(client_socket, response_msg, num_bytes_r, 0); //sends a chunk of the message to the client
        if(num_bytes_s == -1) {
            printf("Error sending response to client: send() call failed.");
            close(listen_socket);
            close(server_socket);
            close(client_socket);
            exit(1);
        }

        //printf("Received bytes: %ld | Sent bytes: %ld\n", num_bytes_r, num_bytes_s);
        memset(response_msg, 0, BUFFER_SIZE); //clears response_msg
    }
    if(num_bytes_r == -1) {
        printf("Error recieving response from server: recv() call failed.");
        close(listen_socket);
        close(server_socket);
        close(client_socket);
        exit(1);
    }

    freeaddrinfo(pserver); //closes server address info
}

void happy_filter() {
    while((regexec(&regex, response_msg, 1, match, 0)) == 0) {
        char* ptr = response_msg + match->rm_so; //uses start offset to navigate to response
        *ptr += 's' - 'h'; //shifts each letter over
        ptr++;
        *ptr += 'i' - 'a';
        ptr++;
        *ptr += 'l' - 'p';
        ptr++;
        *ptr += 'l' - 'p';
        ptr++;
        *ptr += 'y' - 'y';
    }
}

void clown_filter() {
    if(strstr(request_msg, ".jpg")) { //if the request contains a .jpg image
        memset(request_msg, 0, BUFFER_SIZE); //clear the request so we can make a new, modified one
        strcpy(request_msg, clown_randomizer());
    }
}

const char* clown_randomizer() {
    int clown_num = (rand() % 2) + 1; //generates a random number between 1 and 2
    switch(clown_num) {
        case 1:
            return "GET http://pages.cpsc.ucalgary.ca/~carey/CPSC441/ass1/clown1.png HTTP/1.0\r\nHost: pages.cpsc.ucalgary.ca\r\n\r\n";
            break;
        case 2:
            return "GET http://pages.cpsc.ucalgary.ca/~carey/CPSC441/ass1/clown2.png HTTP/1.0\r\nHost: pages.cpsc.ucalgary.ca\r\n\r\n";
            break;
        default:
            printf("Error with clown_randomizer() randomization");
            exit(1);
    }
}