//File: server.h
//Name: Quentin Jennings
//UCID: 30089570
//Section: L01 (Carey)
#ifndef CLIENT
#define CLIENT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void server_loop();
void split_string(int advflag);
void merge_strings(int advflag);

#endif