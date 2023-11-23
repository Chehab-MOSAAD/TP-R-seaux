# TP-Reseaux
USE in CONNECTED mode :
This work focuses on the use of SOCK_STREAM sockets AF_INET. It is a reminder of the operations to be performed on the one hand by a client process and on the other by a server process.

Operations performed in order on the SERVER side :
• Created a listening socket.
• Associated a socket address (IP address and port number) with the service.
• Listen to the socket for incoming TCP connection establishment requests.
• Accept an incoming TCP connection and create a new socket dedicated to that connection.
• Receive/Read and then Transmit/Write to the new socket.
• Closed the new socket

Operations carried out in order on the CLIENT side :
• Created a socket.
• Connected to the server by giving the address of the remote socket (server IP address and port number service).
• Send/Write and then Receive/Read to the socket.
• Closed the socket.
