first in the terminal run the script ./compile.sh to compile node client and relay server

then open three different terminals,one each for node client and relay server

we will first run the relay server by the command ./s 8080 
where 8080 is the assigned port no

after that we can run node.c once in the folders node1, node2, node3
 by ./n 127.0.0.1 8080
to have multiple nodes up and running
127.0.0.1 denotes ip address of local host and 8080 is the server port no

after that we finally run client by ./c 127.0.0.1 8080
where client asks for a filename. We can provide the filename and it will see which node has a copy if any and get it from the particular node and display it
on the terminal
