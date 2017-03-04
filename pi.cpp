#include <iostream>
#include <vector>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <pthread.h>
#include <cstdlib>
#include <cmath>
#include <time.h>

using namespace std;

#define CENTER 0.5

/* prototypes */
void *point(void*);
double dist(double x1, double y1, double x2, double y2);

/* global variables */
static int in = 0;
static pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

/* main function for monte carlo pi estimation
 * argv[1] is used as the number of points, default is 10 for invalid
 * dies if no argv[1] with usage error message
 */
int main(int argc, char *argv[]) {
    // get user input on number of points for estimation, default for invalid = 10
    if (argc != 2) {
        cout << "Error: usage -- ./pi <number of points>" << endl;
        exit(-1);
    }
    string arg = argv[1];
    stringstream mystream(arg);
    int numPoints;
    if (!(mystream >> numPoints) || numPoints < 1 ) {
        cout << "invalid number of points, using 10 instead" << endl;
        numPoints = 10;
    }
    
    // start the clock
    clock_t start = clock(), diff;
    
    // create and join the threads for point estimation
    pthread_t *threads = (pthread_t *) calloc(numPoints, sizeof(pthread_t));
    
    int i;
    for (i = 0; i < numPoints; i++) {
        pthread_create( &threads[i], NULL, point, NULL );
    }
    
    int j;
    for (j = 0; j < numPoints; j++) {
        pthread_join( threads[j], NULL);
    }
    
    // print pi estimation
    float pi = 4 * in / (float) numPoints;
    cout << "Estimation of pi using " << numPoints << " points is " << pi << endl;
    
    // end the clock
    diff = clock();
    int msec = diff * 1000 / CLOCKS_PER_SEC;
    cout << "Time spent estimating: " << (float)(msec)/1000 << " seconds" << endl;
    
    return 1;
    
}

/* point
 * function for threads to execute
 * finds a random point (0.0-1.0, 0.0-1.0)
 * determines whether it falls within a unit circle
 */
void *point (void *threadarg) {
    double x = double( rand()/double(RAND_MAX) );
    double y = double( rand()/double(RAND_MAX) );
    double myDist = dist(x, y, CENTER, CENTER);
    
    if ( myDist > CENTER ) {
        pthread_exit(NULL);
        return threadarg;
    }
    
    pthread_mutex_lock( &mutex1 );
    in++;
    pthread_mutex_unlock( &mutex1 );
    
    pthread_exit(NULL);
    
    return threadarg;
}

/* dist
 * finds the distance between to points (x1, y1) and (x2, y2)
 */
double dist(double x1, double y1, double x2, double y2) {
    return sqrt( pow(x2 - x1, 2) + pow(y2 - y1, 2) );
}


