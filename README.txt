HOMEWORK 3
Authors: Teresa Condon and Sara Perkins
Version: 3.4.2017

How To Run: run our makefile with "make all" or "make". This will create two executables: 'pi' and ‘pi_pool’. Both executables require a single integer command-line argument specifying how many points should be used to estimate the value of pi. pi creates each point on its own thread, while pi_pool utilizes a thread pool. The thread pool currently contains NTHREADS = 20.

Questions:
1. Is there a number of threads for which only your thread pool works?
The original pi estimation with a thread per point seems to start really struggling around 100k threads (we terminated it after a bit), whereas the thread pool handles 100k points really well because it is limited to 20 threads.

2. Could you find examples where the thread pool performs quicker?
50k points of estimation has a very noticeable difference to the eye in time of estimation. pi takes a few seconds whereas pi_pool is still fairly quick.

3. Could you find examples where the thread pool is slower?
10 points of estimation. pi takes 0.002 seconds whereas pi_pool takes 0.004 seconds. Our prediction is that if the points of estimation is < 20 pi will be quicker than pi_pool, as pi_pool always creates 20 threads regardless which is expensive in terms of time efficiency.

Time for 10k threads:
pi: 	 2.075 seconds
pi_pool: 0.067 seconds

Time Spent Developing:
Teresa Condon: 8hrs 
Sara Perkins: 8hrs
