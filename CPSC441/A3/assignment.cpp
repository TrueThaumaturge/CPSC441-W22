//RUN WITH ./assignment.exe < file.txt

#include <string.h>
#include <stdio.h>
#include <iostream>
#include <queue>

using namespace std;

#define BUFFER_SIZE 100 //number of packets
#define MBPS 5
int BPS; //WLAN transfer rate

//stats to keep track of
int recieved_packets, dropped_packets, delivered_packets;
double avg_d_queue_total;
int avg_d_queue_n;
double curr_time = 0;

void simulation(int);
void pause();
struct Packet {
    int bits;
    double t_in, t_out; //time in and time out
    double d_trans, d_queue; //transmission and queueing delay

    Packet(double t_arr, int bytes) {
        t_in = t_arr;
        bits = bytes * 8;
        d_trans = (double)bits / BPS;
    }

    void calc() {
        d_queue = curr_time - t_in;
        avg_d_queue_total += d_queue;
        avg_d_queue_n++;
        //cout << "curr_time:" << curr_time << " | tin:" << t_in << endl;
        t_out = t_in + d_trans + d_queue;
    }
};


int main() {
    simulation(MBPS);
    return 0;
}

void pause() {
    cout << "Press any character to continue..." << endl;
    cin.get();
}

void simulation(int Mbps) {
    cout << "Started simulation for " << Mbps << "Mbps, please wait..." << endl;
    BPS = Mbps * 1000000; //sets bit rate
    //initialize everything to 0
    recieved_packets = dropped_packets = delivered_packets = 0;

    queue<Packet> buffer; //router packet buffer - FIFO

    //iterates through each event in the packet trace until it reaches end of file
    while(true) {

        //receive packet from packet trace
        int size;
        cin >> curr_time >> size; //reads one line of data and parses the time and size
        if (cin.eof()) { 
            break;
        }
        Packet rcvd_pkt(curr_time, size); //wraps time and size into packet structure
        recieved_packets++;

        double pkt_trace_time = curr_time;
        //check for departures in the buffer
        while(!buffer.empty() && buffer.front().t_out < pkt_trace_time) {
            curr_time = buffer.front().t_out;
            buffer.pop();
            delivered_packets++;
            if(!buffer.empty()) { //if there's another packet behind it
                buffer.front().calc(); //calculates d_queue and t_out for newly-in-front packet
            }
        }
        curr_time = pkt_trace_time;

        //try to send current packet
        if(buffer.size() < BUFFER_SIZE) { //if there's room
            buffer.push(rcvd_pkt); //queue packet
            if(buffer.size() == 1) { //if this is the first packet in the queue
                buffer.front().calc(); //it's already at the front so go ahead and calculate the t_out
            }
        }
        else if(buffer.size() == BUFFER_SIZE) { //if buffer is full
            dropped_packets++; //packet is ignored and dropped!
        }
    }

    //we finished reading the packet trace, now we empty the queue
    while(!buffer.empty()) {
        curr_time = buffer.front().t_out;
        buffer.pop();
        delivered_packets++;
        if(!buffer.empty()) { //if there's another packet behind it
            buffer.front().calc(); //calculates d_queue and t_out for newly-in-front packet
        }
    }


    //final stats
    printf("==========================Results==========================\n");
    printf("Buffer size: %d packets\nTransfer rate: %dMbps\n", BUFFER_SIZE, MBPS);
    printf("-----------------------------------------------------------\n");
    printf("Number of incoming packets: %d packets\n", recieved_packets);
    printf("Number of delivered packets: %d packets\n", delivered_packets);
    printf("Number of lost packets: %d packets\n", dropped_packets);
    printf("-----------------------------------------------------------\n");
    double packet_loss_p = (double)dropped_packets / recieved_packets * 100;
    printf("Packet loss percentage: %lf%%\n", packet_loss_p);
    double avg_d_queue = avg_d_queue_total / avg_d_queue_n;
    printf("Average queueing delay for delivered packets: %lf seconds\n", avg_d_queue);
    printf("===========================================================\n");
}