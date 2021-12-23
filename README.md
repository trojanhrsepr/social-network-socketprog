Name: Prasanna Rajagopalan

Project Description:

Implemented a social matching service where two clients issue a request for finding their compatibility. This request is sent to a Central Server which interacts with three other backend servers for pulling information and data processing. The Central server connects to the Topology server (server T) which has the user social network information. Central server established connection to the Score server (server S) which stores the compatibility scores for each user. Finally, the server uses the network topology and scores to generate a graph that connects both users, and provides the smallest matching gap between them. Both the matching gap and the graph generated is sent back to both clients.

Server T has access to a database file named edgelist.txt, and Server S has access to a database file named scores.txt. Both clients and the Central server communicate over a TCP connection while the communication between Central and the Back-Servers T, S & P is over a UDP connection. 

Completion:
Phase 1,2,3 and 4 are complete. Optional phase is not complete

Additions:
Added header files for each of the servers to enhance code readability.
All extra header files are standard C++ Headers:
#include <string>
#include <map>
#include <set>
#include <fstream>
#include <algorithm>
#include <vector>
#include <list>
#include <iostream>
#include <queue>
#include <algorithm>
#include <iomanip>

Also added an extra file called common.h for adding constants and all utilities common to all codes.

Sources used:
Geeks for Geeks link: https://www.geeksforgeeks.org/bfs-using-vectors-queue-per-algorithm-clrs/ and https://www.geeksforgeeks.org/breadth-first-search-or-bfs-for-a-graph/ were used as reference. 

BFS Sample implementations using vectors was picked up due to familiarity of handling data type earlier.
Though the class implementations for Graphs is similar, function definitions have been tweaked to serve purpose of this particular problem.

Code reference for socket communication for both TCP and UDP from Beej's Guide to Socket Programming. Used same variables as Ch6 Pg35. Switched out generic commands for IPv4/v6 to IPv4 only
Also used sample UDP and TCP implementations from Beej.

For processing, class implements Dijkstra's algorithm inspired by https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-greedy-algo-7/ for implementation. Changes in the algorithm have been attempted but overall functionality is similar.

Geeks For Geeks, Cplusplus.com, and StackOverflow were also used to unblock certain exceptions and code snippets might appear to resolve issues there. I have not attempted to change variables and hide usage of code from there.

Issues faced in project:
1. Code duplication: constants header was created and lots of reusage was possible
2. Common file has function defintions. This was done to avoid creation of more files.
3. UDP Socket creating trouble in M1 machines. Had a lot of issues setting things up at the start
4. Integer vectors unable to be sent through sockets
5. Changing all files to reverse compatible CPP files
6. Used similar variables to that of Beej Socket Programming and Geeks for Geeks where I have used help to be transparent on reusing code. Structure and the entire functioning of code was a lot of hard work!

Hoping the code is readable enough for you to work with. I have tested this with the sample file provided in Piazza. The same file will be visible in directory.
