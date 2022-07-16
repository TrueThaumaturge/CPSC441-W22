//PROGRAM WONT WORK PROPERLY ON WINDOWS!!! USE LINUX!!!!!

#include "a4.h"

class Bozon {
    private:
        int id; //unique id to identify each bozon -> for debugging
        bool yodelingFlag; //whether bozon is currently yodeling or not
            //false = sleeping
            //true = yodelling
        double eventDuration; //the duration of the next sleep/yodel event
        double nextEventTime; //the next event time

    public:
        //constructors
        Bozon() = default;
        Bozon(int id) {
            this->id = id;
            yodelingFlag = false;
            setSleep();
        } 

        //getters
        bool isYodeling() {
            return yodelingFlag;
        }
        double getNextEvent() {
            return nextEventTime;
        }
        double getEventDuration() {
            return eventDuration;
        }
        int getID() {
            return id;
        }
        void setYodel() {
            yodelingFlag = true; //now yodeling
            if(fixedYodel) {
                eventDuration = NATIONALYODEL;
            }
            else {
                eventDuration = Exponential(yodelMean); //randomizes next sleep duration
            }
            nextEventTime = currTime + eventDuration; //sets next sleep event
        }
        void setSleep() {
            yodelingFlag = false;
            eventDuration = Exponential(sleepMean); //randomizes next yodel duration
            nextEventTime = currTime + eventDuration; //sets next sleep event
        }

        void display() {
            cout << "ID: " << id << endl;
            cout << "yodelingFlag: " << yodelingFlag << endl;
            cout << "nextEventTime: " << nextEventTime << endl;
            cout << "eventDuration: " << eventDuration << endl;
        }
};

int main() {
    srand(time(NULL)); //seeds randomization
	
    cout << "Bozon Colony Simulation" << endl;
    //gathers user inputs
    while(colonySize < 1) {
        cout << "Enter colony size, M. (1 or greater)\n >";
        cin >> colonySize;
    }
    while(sleepMean <= 0) {
        cout << "Enter mean sleep period, S. (Greater than 0)\n >";
        cin >> sleepMean;
    }
    if(!fixedYodel) { //doesn't gather yodel info for fixed yodel
        while(yodelMean <= 0) {
            cout << "Enter mean yodel duration, Y. (Greater than 0)\n >";
            cin >> yodelMean;
        }
    }

    //creates bozon colony
    colony = new Bozon[colonySize];
    for(int i = 0; i < colonySize; i++) {
        colony[i] = Bozon(i);
        #if TESTING
        colony[i].display();
        cout << endl;
        #endif
    }

    //runs the time simulation
    simulation();

    //prints formated simulation results
    cout << fixed;
    cout << setprecision(3);
    cout << "===================Simulation Results=====================" << endl;
    cout << "Size (M) = " << colonySize << " colony" << endl;
    cout << "Mean sleep time (S) = " << sleepMean << " time units" << endl;
    if(fixedYodel) {
        double natYodel = NATIONALYODEL;
        cout << "National yodel time = " << natYodel << " time units" << endl;
    }
    else {
        cout << "Mean yodel time (Y) = " << yodelMean << " time units" << endl;
    }
    cout << "Time analyzed: " << currTime << endl;
    cout << "==========================================================" << endl;
    cout << "Time silent: " << timeSilent << "\tPercentage silent: " << (timeSilent / currTime * 100) << "%" << endl;
    cout << "Time melodious: " << timeMelodious << "\tPercentage melodious: " << (timeMelodious / currTime * 100) << "%" << endl;;
    cout << "Time screechy: " << timeScreechy << "\tPercentage screechy: " << (timeScreechy / currTime * 100) << "%" << endl;;
    cout << "----------------------------------------------------------" << endl;
    cout << "Number of attempted yodels: " << numYodels << endl;
    cout << "Number of perfect yodels: " << numPerfectYodels << "\t(" << ((double)numPerfectYodels / (double)numYodels * 100) << "%)" << endl;
    cout << "==========================================================" << endl;
    cout << endl;

    return 0;
}

void simulation() {
    cout << "Starting simulation...\n" << endl;

    //start by initializing all variables to 0
    currTime = 0;
    timeSilent = timeMelodious = timeScreechy = 0;
    numYodels = numPerfectYodels = currYodeling = 0;
    
    int maybePerfectID = -1; //ID of a potentially perfect yodeling bozon

    Bozon* nextBozon;
    double nextEvent;
    //main simulation loop
    while(true) {
        nextBozon = getNextBozon(); //grabs the bozon having the next event
        //following two are just to make the code a bit more readable
        nextEvent = nextBozon->getNextEvent(); //grabs the time of the next event - we'll advance to here shortly

        //if the loop needs to end
        if(nextEvent > ENDTIME) {
            //we account for the remaining time in our stats
            if(currYodeling == 0) { //silent
                timeSilent += (ENDTIME - currTime);
            }
            else if(currYodeling == 1) { //melodious
                timeMelodious += (ENDTIME - currTime);
            }
            else if(currYodeling >= 2) { //screechy
                timeScreechy += (ENDTIME - currTime);
            }
            currTime = ENDTIME;
            break; //and break the loop
        }

        //records passed time under its proper category
        if(currYodeling == 0) { //silent
            timeSilent += (nextEvent - currTime);
        }
        else if(currYodeling == 1) { //melodious
            timeMelodious += (nextEvent - currTime);
        }
        else if(currYodeling >= 2) { //screechy
            timeScreechy += (nextEvent - currTime);
        }
        currTime = nextEvent; //advances time

        #if TESTING
        cout << "Current time: " << currTime << endl;
        cout << "# yodeling before event: " << currYodeling << endl;
        cout << "ID: " << nextBozon->getID() << endl;
        #endif

        //we moved the time up, now we need to make the discrete event actually happen
        //if it's currently yodelling we make it sleep
        if(nextBozon->isYodeling()) {
            nextBozon->setSleep(); //now sleeping
            currYodeling--; //it stopped yodeling
            numYodels++; //yodel finished so total number goes up
            if(currYodeling == 0 && maybePerfectID == nextBozon->getID()) {
                numPerfectYodels++;
            }
        }
        //if it's currently sleeping we make it yodel
        else {
            nextBozon->setYodel(); //now yodeling
            currYodeling++; //it started yodeling
            if(currYodeling == 1) { //flag it as a potential perfect yodel
                maybePerfectID = nextBozon->getID();
            }
            else { //or reset the flag if it fails
                maybePerfectID = -1;
            }
        }

        #if TESTING
        cout << "isYodeling: " << nextBozon->isYodeling() << endl;
        cout << "# yodeling after event: " << currYodeling << endl;
        cout << "Next event for this bozon: " << nextBozon->getNextEvent() << endl;
        cout << endl;
        #endif
    }
}

//insertion sort - algorithm adapted from https://www.geeksforgeeks.org/insertion-sort/
//sorts bozon list from smallest to largest event time
Bozon* getNextBozon() {
    Bozon* smallest = &colony[0];
    for(int i = 1; i < colonySize; i++) { //iterates through colony
        //if the current bozon's event occurs before the current smallest
        if(colony[i].getNextEvent() < smallest->getNextEvent()) {
            smallest = &colony[i]; //that's the new smallest
        }
    }
    return smallest;
}

//CODE FROM https://pages.cpsc.ucalgary.ca/~carey/CPSC441/ass4/rand.txt
//=====================================================================
/* Parameters for random number generation. */
#define MAX_INT 2147483647 /* Maximum positive integer 2^31 - 1 */

/* Generate a random floating point number uniformly distributed in [0,1] */
double Uniform01() {
    double randnum;
    /* get a random positive integer from random() */
    randnum = (double)1.0 * rand();
    /* divide by max int to get something in the range 0.0 to 1.0  */
    randnum = randnum / (1.0 * MAX_INT);
    return (randnum);
}

/* Generate a random floating point number from an exponential    */
/* distribution with mean mu.                                     */
double Exponential(double mu) {
    double randnum, ans;

    randnum = Uniform01();
    ans = -(mu)*log(randnum);
    return (ans);
}
//END OF CODE SNIPPET
//======================================================================