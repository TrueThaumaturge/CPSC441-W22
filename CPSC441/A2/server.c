//File: server.c
//Name: Quentin Jennings
//UCID: 30089570
//Section: L01 (Carey)
#include "server.h"
#include "ports.h"
#include "vowelizer.h"

//global variables
int status, listen_socket, tcp_socket, udp_socket; //sockets/status
struct addrinfo base, *tcp, *udp; //addrinfos
ssize_t num_bytes_s, num_bytes_r; //bytes sent/received
char str[STR_SIZE], str_v[STR_SIZE], str_c[STR_SIZE]; //string buffers

//UDP client info
struct sockaddr_in clientaddr;

int main() {
    memset(&base, 0, sizeof(base));
    //gets address info for TCP
    base.ai_family = AF_INET; //ipv4
    base.ai_socktype = SOCK_STREAM; //TCP
    base.ai_flags = AI_PASSIVE; //listening

    //TCP addrinfo
    status = getaddrinfo(NULL, TCP_PORT, &base, &tcp);
    if(status) {
        printf("Error getting address info for TCP: getaddrinfo() call failed.");
        exit(1);
    }

    //TCP socket setup
    listen_socket = socket(tcp->ai_family, tcp->ai_socktype, tcp->ai_protocol); //creating listening socket
    if(tcp_socket == -1) {
        printf("Error creating server TCP socket: socket() call failed.");
        exit(1);
    }

    status = bind(listen_socket, tcp->ai_addr, tcp->ai_addrlen); //binds socket
    if(status) {
        printf("Error binding server TCP socket: bind() call failed");
        exit(1);
    }

    printf("TCP socket created.\n");

    status = listen(listen_socket, 10); //sets socket to listen
    if(status) {
        printf("Error: listen() call failed");
    }

    //gets address info for UDP
    //base.ai_flags = 0; //not listening
    base.ai_socktype = SOCK_DGRAM; //udp
    status = getaddrinfo(NULL, UDP_PORT, &base, &udp); //sets up info for udp port
    if(status) {
        printf("Error getting address info for TCP: getaddrinfo() call failed.");
        exit(1);
    }

    //UDP socket setup
    udp_socket = socket(udp->ai_family, udp->ai_socktype, udp->ai_protocol); //creates udp socket
    if(udp_socket == -1) {
        printf("Error creating server UDP socket: socket() call failed.");
        exit(1);
    }

    status = bind(udp_socket, udp->ai_addr, udp->ai_addrlen); //binds socket
    if(status) {
        printf("Error binding server UDP socket: bind() call failed");
        exit(1);
    }

    printf("UDP socket created, entering server loop.\n");

    server_loop(); //enters the main program loop that runs indefinitely
}

void server_loop() {
    //server loop - repeats indefinitely
    while(1) {
        printf("\nWaiting for client connection...\n");
        //waits to accept a connection
        tcp_socket = accept(listen_socket, NULL, NULL);
        if(tcp_socket == -1) {
            printf("Error accepting TCP connection: accept() call failed");
            exit(1);
        }
        printf("Client connected, sending dummy response through TCP.\n");

        //sends a dummy message to client to let them know they're being served
        num_bytes_s = send(tcp_socket, "aaaaa", 5, 0);
        if(num_bytes_s == -1) {
            printf("Error sending dummy bytes: send() call failed");
            exit(1);
        }

        //grabs clients info with dummy UDP message
        socklen_t clientaddr_len = sizeof(clientaddr);
        num_bytes_r = recvfrom(udp_socket, str, 1, 0, (struct sockaddr*)&clientaddr, &clientaddr_len);
        if(num_bytes_r == -1) {
            printf("Error obtaining UDP client info: recvfrom() call failed");
            exit(1);
        }
        printf("Dummy UDP received from client, client info obtained.\n");
        memset(&str, 0, STR_SIZE); //paranoia again

        int exitloop = 0;
        //client session loop - repeats until client exits
        while(exitloop == 0) {
            //clears string buffers
            memset(&str, 0, STR_SIZE);
            memset(&str_v, 0, STR_SIZE);
            memset(&str_c, 0, STR_SIZE);
            printf("Waiting for client to choose an option...\n");

            int option;
            //receives option response from client
            num_bytes_r = recv(tcp_socket, &option, sizeof(option), 0);
            if(num_bytes_r == -1) {
                printf("Error receiving client option: accept() call failed");
                exit(1);
            }
            printf("Client selected option: ");

            //chooses operation based on client selection
            switch(option) {
                case 1: //simple split
                    printf("Simple Split\n");
                    split_string(0);
                    break;
                case 2: //adv split
                    printf("Adv. Split\n");
                    split_string(1);
                    break;
                case 3: //simple merge
                    printf("Simple Merge\n");
                    merge_strings(0);
                    break;
                case 4: //adv merge
                    printf("Adv. Merge\n");
                    merge_strings(1);
                    break;
                case 5: //exit
                    printf("Exit\n");
                    exitloop = 1;
                    break;
                default: //oops!
                    printf("Error: invalid option received, this shouldn't happen.");
                    exit(1);
            }
        }
        close(tcp_socket); //closes connection socket when done
        printf("Closed connection with client.\n");
    }
}

void split_string(int advflag) {
    //receives string from client
    num_bytes_r = recv(tcp_socket, str, STR_SIZE, 0);
    if(num_bytes_r == -1) {
        printf("Error: recv() TCP str.");
        exit(1);
    }
    printf("received string from client: [%s]\n", str);

    //splits string in one of 2 ways, depending on client option
    if(advflag == 0) split_simple(str, str_v, str_c);
    else split_advanced(str, str_v, str_c);
    printf("Sending over UDP: [%s]\nSending over TCP: [%s]\n", str_v, str_c);

    //sends TCP back
    num_bytes_s = send(tcp_socket, str_c, STR_SIZE, 0);
    if(num_bytes_s == -1) {
        printf("Error: send() TCP str_c.");
        exit(1);
    }
    printf("Sent %ld bytes through TCP and ", num_bytes_s);

    //sends UDP back
    socklen_t clientaddr_len = sizeof(clientaddr);
    num_bytes_s = sendto(udp_socket, str_v, STR_SIZE, 0, (struct sockaddr*)&clientaddr, clientaddr_len);
    if(num_bytes_s == -1) {
        perror("Error: sendto() UDP str_v");
        exit(1);
    }
    printf("%ld bytes through UDP.\n", num_bytes_s);
}

void merge_strings(int advflag) {
    //receives TCP string
    num_bytes_r = recv(tcp_socket, str_c, STR_SIZE, 0);
    if(num_bytes_r == -1) {
        printf("Error: recv() TCP str.");
        exit(1);
    }
    printf("received from TCP: [%s]\n", str_c);

    //receives UDP string
    socklen_t clientaddr_len = sizeof(clientaddr);

    num_bytes_r = recvfrom(udp_socket, str_v, STR_SIZE, 0, (struct sockaddr*)&clientaddr, &clientaddr_len);
    if(num_bytes_r == -1) {
        printf("Error: recv() UDP str_v.");
        exit(1);
    }
    printf("received from UDP: [%s]\n", str_v);

    //merges strings in one of 2 ways, depending on client option
    if(advflag == 0) merge_simple(str, str_v, str_c);
    else merge_advanced(str, str_v, str_c);
    printf("Sending merged string: [%s]\n", str);

    //sends back through TCP
    num_bytes_s = send(tcp_socket, str, STR_SIZE, 0);
    if(num_bytes_s == -1) {
        printf("Error: send() TCP str.");
        exit(1);
    }
    printf("Sent %ld bytes through TCP.\n", num_bytes_s);
}