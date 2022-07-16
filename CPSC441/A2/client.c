//File: 
//Name: Quentin Jennings
//UCID: 30089570
//Section: L01 (Carey)
#include "client.h"
#include "ports.h"

//global variables
int status, tcp_socket, udp_socket;
struct addrinfo base, *tcp, *udp;
ssize_t num_bytes_s, num_bytes_r;
char str[STR_SIZE], str_v[STR_SIZE], str_c[STR_SIZE];

int main(){
    //sets up base for addrinfos
    memset(&base, 0, sizeof(base));
    base.ai_family = AF_INET; //ipv4
    base.ai_socktype = SOCK_STREAM; //TCP

    //creates tcp addrinfo
    status = getaddrinfo(IP_ADDR, TCP_PORT, &base, &tcp);
    if(status) {
        printf("Error getting address info for TCP: getaddrinfo() call failed.");
        exit(1);
    }

    //creates tcp socket
    tcp_socket = socket(tcp->ai_family, tcp->ai_socktype, tcp->ai_protocol);
    if(tcp_socket == -1) {
        printf("Error creating TCP socket: socket() call failed.");
        exit(1);
    }

    //tcp socket connects to server
    status = connect(tcp_socket, tcp->ai_addr, tcp->ai_addrlen);
    if(status) {
        printf("Error connecting to server TCP: connect() call failed.");
        exit(1);
    }
    printf("TCP socket created and connected.\n");

    //creates UDP addrinfo
    base.ai_socktype = SOCK_DGRAM; //UDP
    status = getaddrinfo(IP_ADDR, UDP_PORT, &base, &udp);
    if(status) {
        printf("Error getting address info for UDP: getaddrinfo() call failed.");
        exit(1);
    }

    //creates udp socket
    udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if(udp_socket == -1) {
        printf("Error creating UDP socket: socket() call failed.");
        exit(1);
    }

    //connects udp socket (so we can use recv() and send())
    status = connect(udp_socket, udp->ai_addr, udp->ai_addrlen);
    if(status) {
        printf("Error connecting UDP socket: connect() call failed.");
        exit(1);
    }
    printf("UDP socket created and connected. Sending dummy UDP to server.\n");

    //sends a dummy message through udp to give the server our address info
    num_bytes_s = send(udp_socket, "a", 1, 0);
    if(num_bytes_s == -1) {
        perror("Error sending dummy UDP socket: send() call failed.");
        exit(1);
    }
    printf("Waiting for response from server...\n");

    //receives dummy response from server to know it's being served
    //client stalls here until it receives, effectively forming a queue
    num_bytes_r = recv(tcp_socket, str, 5, 0);
    if(num_bytes_r == -1) {
        printf("Error recieving dummy response: recv() call failed.");
        exit(1);
    }
    printf("Connected to server!\n\n");
    memset(&str, 0, STR_SIZE); //paranoia

    menu_loop(); //runs the menu loop
    
    //closes everything when client is done
    close(tcp_socket);
    freeaddrinfo(tcp);
    freeaddrinfo(udp);
    printf("Disconnected and exiting program.\n");
    return 0;
}

//the loop that lets a client select as many options as they want before disconnecting
void menu_loop() {
    int exit_loop = 0, option, mflag = 1;
    while(exit_loop == 0) {
        //resets anything stored in the string buffers
        memset(&str, 0, STR_SIZE);
        memset(&str_v, 0, STR_SIZE);
        memset(&str_c, 0, STR_SIZE);

        if(mflag == 1) { //prints this menu after all but a failed operation
            printf("================\nChoose an option by selecting a number:\n1 - Split string (Simple)\n2 - Split string (Adv.)\n3 - Merge strings (Simple)\n");
            printf("4 - Merge strings (Adv.)\n5 - Exit client program\n===============\n");
        }
        printf("Enter an option: > ");

        scanf("%d", &option); //reads the user input
        //clears input buffer
        char c;
        while(((c = getchar()) != EOF) && (c != '\n'));

        //sends message to the server through tcp to tell it what option was selected
        if(1 <= option && option <= 5) {
            num_bytes_s = send(tcp_socket, &option, sizeof(option), 0);
            if(num_bytes_s == -1) {
                printf("Error sending mode message: send() failed.");
                exit(1);
            }
        }

        mflag = 1;
        //runs a different function depending on the user input
        switch(option) {
            case 1: //simple split
                split_string();
                printf("Press enter to continue.\n");
                getchar();
                break;
            case 2: //adv split
                split_string();
                printf("Press enter to continue.\n");
                getchar();
                break;
            case 3: //simple merge
                merge_strings();
                printf("Press enter to continue.\n");
                getchar();
                break;
            case 4: //adv merge
                merge_strings();
                printf("Press enter to continue.\n");
                getchar();
                break;
            case 5: //exit program
                exit_loop = 1;
                break;
            default:
                mflag = 0; //doesnt reprint menu on invalid input
                printf("Invalid input, please try again.\n");
        }
    }
}

void split_string() {
    //gets input string
    printf("Enter string to be split: > ");
    fgets(str, STR_SIZE, stdin);
    sscanf(str, "%[^\n]", str);

    printf("String [%s] received. Sending to server through TCP.\n", str);

    //sends string to be split by TCP
    num_bytes_s = send(tcp_socket, &str, STR_SIZE, 0);
    if(num_bytes_s == -1) {
        printf("Error sending str over TCP: send() failed.");
        exit(1);
    }
    printf("%ld bytes sent through TCP.\n", num_bytes_s);

    //receives TCP string
    num_bytes_r = recv(tcp_socket, str_c, STR_SIZE, 0);
    if(num_bytes_s == -1) {
        printf("Error recieving str_c over TCP: recv() failed.");
        exit(1);
    }

    //receives UDP string
    num_bytes_r = recv(udp_socket, str_v, STR_SIZE, 0);
    if(num_bytes_r < 0) {
        printf("Error recieving str_v over UDP: recvfrom() failed.");
        exit(1);
    }

    //prints result
    printf("\n**Result of split: \n**UDP: [%s]\n**TCP: [%s]\n\n", str_v, str_c);

}

void merge_strings() {
    //gets input strings
    printf("Enter vowel portion: > ");
    fgets(str_v, STR_SIZE, stdin);
    sscanf(str_v, "%[^\n]", str_v);
    if(*str_v == '\n') {
        str_v[0] = '\0';
    }

    printf("Enter non-vowel portion: > ");
    fgets(str_c, STR_SIZE, stdin);
    sscanf(str_c, "%[^\n]", str_c);
    if(*str_c == '\n') {
        str_c[0] = '\0';
    }

    printf("Sending on UDP: [%s]\nSending on TCP: [%s]\n", str_v, str_c);

    //sends TCP string
    num_bytes_s = send(tcp_socket, str_c, STR_SIZE, 0);
    if(num_bytes_s == -1) {
        printf("Error sending str_c over TCP: send() failed.");
        exit(1);
    }
    printf("%ld bytes sent through TCP and ", num_bytes_s);

    //sends UDP string
    num_bytes_s = send(udp_socket, str_v, STR_SIZE, 0);
    if(num_bytes_s == -1) {
        printf("Error sending UDP string: sendto() failed.");
        exit(1);
    }
    printf("%ld bytes sent through UDP.\n", num_bytes_s);

    //receives TCP result string
    num_bytes_r = recv(tcp_socket, str, STR_SIZE, 0);
    if(num_bytes_s == -1) {
        printf("Error recieving str_c over TCP: recv() failed.");
        exit(1);
    }

    //prints result
    printf("\n**Result of merge: [%s]\n\n", str);
}