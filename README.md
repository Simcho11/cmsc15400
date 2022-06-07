# cmsc15400
This is a repository of the projects I completed as a part of CMSC15400 - Introduction to Computer Systems. These projects were completed Spring of 2022 and were all done in C.

## P1: Bit Manipulation
Here we were given "puzzles" to solve using bit manipulation - pretty straight forward. This project was very interesting and enjoyable as I had never worked with shifting bits to this degree before.

**Successes**
- Got a perfect score
- Most solutions are extremely efficient

**Room for Improvement**
- Some solutions could be more efficient

**Takeaways**
- Got much better at understanding bit manipulation
- Got comfortable using bit manipulation

## P3: Cache
In this project I programmed a cache simulator. This cache simulator takes a valgrind memory trace as input and simulates the hit/miss behavior of cache memory. I used the allocate-on-write policy to handle write misses and the least-recently-used policy to handle evictions. I found implementing LRU was a uniquely challenging problem.

**Successes**
- Got a perfect score
- Lots of comments to clearly explain what is going on

**Room for Improvement**
- Have extra unused variables and junk
- Certain tasks may have been better handled with the use of helper functions for clarity

**Takeaways**
- A better understanding of how cache eviction works

## P4: Shell
For this project I programmed my own shell from scratch. This shell is able to parse a command line into arguments and create a child process which run the command. When the child process finishes, the shell then prompts the user again for another command.

**Successes**
- Created a shell that can run pretty much anything the Unix shell can

**Room for Improvement**
- Did not get a fully correct result
- Extremely unoptimized
- Very hard to read

**Takeaways**
- A deep understanding of how child processes work
- Practice parsing C "strings"

