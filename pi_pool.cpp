#include <iostream>
#include <vector>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include <pthread.h>
#include <cstdlib>
#include <cmath>
#include <queue>
#include <time.h>

using namespace std;

#define CENTER (0.5)
#define NTHREADS (20)
//#define DEBUG
typedef void *(*Pfn) (void *);

/* class structure for the thread pool */
class ThreadPool {
    public:
        queue<Pfn> taskQ;
        pthread_t myThreads[NTHREADS];
    
    // constructor requires wrapper function for threads
    ThreadPool ( Pfn foo ) {
        int i ;
        for ( i = 0; i < NTHREADS; i++ ) {
            pthread_create( &myThreads[i], NULL, foo, NULL );
        }
    }
    
    // join all of the threads in the pool
    void joinAll() {
        int i ;
        for ( i = 0; i < NTHREADS; i++ ) {
            pthread_join( myThreads[i], NULL ) ;
        }
    }
    
    // push a new task onto the queue
    void push(Pfn code) {
        taskQ.push(code);
    }

    // pop a task from the queue
    Pfn pop() {
        Pfn task = taskQ.front();
        taskQ.pop() ;
        return task ;
    }

    // returns the length of the queue
    unsigned long len() {
        return taskQ.size() ;
    }
};

/* prototypes */
void *point(void*);
double dist(double x1, double y1, double x2, double y2);

void *completeTasks (void *arg);
ThreadPool createThreadPool(void);

/* global variables */
static int in = 0;
static pthread_mutex_t mutexIn = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t mutexQ = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t condQ = PTHREAD_COND_INITIALIZER;
static ThreadPool myPool(completeTasks) ;
static int done = 0 ;

/* main function for monte carlo pi estimation
 * argv[1] is used as the number of points, default is 10 for invalid
 * dies if no argv[1] with usage error message
 *
 * ** using a thread pool **
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
    
    /* ---=== adding in a thread pool ===--- */
    
    // fill the queue
    pthread_mutex_lock( &mutexQ );
    int i;
    for ( i = 0; i < numPoints; i++) {
        myPool.push(point);
    }
    #ifdef DEBUG
    cout << "initial queue len=" << myPool.len() << endl;
    #endif
    pthread_cond_broadcast ( &condQ );
    pthread_mutex_unlock( &mutexQ );
    
    // check every now and then to see if the queue is empty
    while ( !done ) {
        usleep ( 100000 ) ;
        #ifdef DEBUG
        cout << "sleeping ..." << endl;
        #endif
        pthread_mutex_lock( &mutexQ );
        done = myPool.len() == 0 ;
        pthread_mutex_unlock( &mutexQ );
    }
    
    pthread_cond_broadcast ( &condQ );
    myPool.joinAll() ;

    // print pi estimation
    float pi = 4 * in / (float) numPoints;
    cout << "Estimation of pi using " << numPoints << " points is " << pi << endl;
    
    // end the clock
    diff = clock();
    int msec = diff * 1000 / CLOCKS_PER_SEC;
    cout << "Time of calculation: " << (float)(msec)/1000 << endl;
    
    return 0 ;
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
        return threadarg;
    }
    
    pthread_mutex_lock( &mutexIn );
    in++;
    pthread_mutex_unlock( &mutexIn );
    
    return threadarg;
}

/* dist
 * finds the distance between to points (x1, y1) and (x2, y2)
 */
double dist(double x1, double y1, double x2, double y2) {
    return sqrt( pow(x2 - x1, 2) + pow(y2 - y1, 2) );
}


/* ------======= Thread Pool =====------ */
/* completeTasks
 * wrapper function for the threads in the thread pool
 * assigns threads to tasks
 */
void *completeTasks (void *arg) {
    int id = rand() % 100;
   
    while(1) {
        // lock mutex
        pthread_mutex_lock( &mutexQ );
        
        // wait for turn
        while ( myPool.len() == 0 && !done ) {
            pthread_cond_wait( &condQ, &mutexQ );
        }

        // if done, give up the mutex
        if ( done ) {
            pthread_mutex_unlock( &mutexQ );
            pthread_exit(NULL);
        }

        Pfn job = myPool.pop();
        pthread_cond_broadcast ( &condQ );
        #ifdef DEBUG
        cout << "thread: " << id << endl;
        #endif
        
        // unlock mutext
        pthread_mutex_unlock( &mutexQ );
        
        // run next job
        job(NULL);
    }
}

