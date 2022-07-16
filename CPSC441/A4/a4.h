#include <string.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <time.h>
#include <math.h>

using namespace std;

//assignment bonus
bool fixedYodel = true; //flag that toggles a random yodel or fixed "national yodel"

//macros
#define NATIONALYODEL 10.0; //duration of national yodel in time units
#define ENDTIME (50.0 * 1000000.0) //time units when the program stops running
#define TESTING 0 //set to 1 to enable debug outputs


//GLOBAL VARIABLES
double sleepMean, yodelMean;
int colonySize;

double currTime = 0; //current simulation time

double timeSilent, timeMelodious, timeScreechy;
int numYodels, numPerfectYodels;
int currYodeling;

class Bozon;
Bozon* colony; //array of bozon objects

int main();
void simulation();
Bozon* getNextBozon();

double Exponential(double mu);
double Uniform01();