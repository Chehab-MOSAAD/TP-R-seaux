/*================================================*/
//Programme simulant un protocole de routage dynamique simplifié
// Ce programme code uniquement le comportement
// de récpeteur d'une annonce de routage
// émise depuis UN SEUL routeur voisin pour UN échange initial de routes
// T. Desprats - Novembre 2022

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h> // struct sockaddr_in
#include <time.h>
#include <signal.h>
#include <stdbool.h>

#include "tabrout.h"

#define BUF_SIZE_IN 64 // we should receive less...
#define IPV4_ADR_STRLEN 16  // == INET_ADDRSTRLEN
#define LOCALHOST "127.0.0.1"
#define NO_BASE_PORT 17900  // base number for computing real port number
#define CL_PORT 8080
#define SERV_PORT 9090
#define MAXBUFF 1024

/* =================================================================== */
/* FONCTION PRINCIPALE : PEER PROCESSUS DE ROUTAGE ROLE RECEPTEUR ONLY */
/* =================================================================== */
int main(int argc, char **argv) {

  // Usage routPrec IDIP@ssRouter  MyNumberRouter NeigborNumberRouter
  // Example routPrec 10.1.1.1 1 2

  char idInitConfigFile [20]; //Id of the configuration file of the router
  char myId [32]; // String array representing the whole id of the Router

  routing_table_t myRoutingTable; //Routing TABLE

  /* Building ID Router from command args */
  sprintf(myId,"R%s %s",argv[2],argv[1]);
  printf("ROUTEUR : %s\n",myId );
  //printf("construction id fichier\n");
  /* Building Config File ID from command args */
  sprintf(idInitConfigFile,"R%sCfg",argv[2]);
  strcat(idInitConfigFile,".txt");
  //printf("\n Nom fichier Configuration : %s",idInitConfigFile);
  /* Loading My Routing Table from Initial Config file */
  init_routing_table(&myRoutingTable, idInitConfigFile);
  printf("ROUTEUR : %d entrées initialement chargées \n",myRoutingTable.nb_entry);
  display_routing_table(&myRoutingTable,myId);

  int sockfd;
  struct sockaddr_in servaddr, cliaddr;

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
    perror("erreur creation du socket!");
    exit(EXIT_FAILURE);
    }

  memset(&servaddr, 0, sizeof(servaddr));
  memset(&cliaddr, 0, sizeof(cliaddr));

  // Remplir les informations du serveur
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(SERV_PORT);

  // Liez le socket avec l'adresse du serveur
  if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 ) {
    perror("bind failed");
    exit(EXIT_FAILURE);
    }

  struct sockaddr_in addrs[3];
  for (int i = 0; i<3; ++i){
    memset(&addrs[i], 0, sizeof(addrs[i]));
    addr[i].sin_family = AF_INET;
    addr[i].sin_addr.s_addr = INADDR_ANY;
    addr[i].sin_port = htons(CL_PORT + i);
  }

  int len, n, from, to, from_message;
  char buffer[MAXBUFFER];

  while (1){
    len = sizeof(cliaddr);
    n = recvfrom(sockfd, (char *)buffer, MAXBUFFER, 0, (struct sockaddr *)&cliaddr, &len);
    buffer[n] = '\n';
    from = ntohs(cliaddr.sin_port) - CL_PORT;
    from_message = buffer[0] - 48;

    if (from != from_message){
        continue;
    }
    to = buffer[1] - 48
    if (from == to){
        continue;
    }
    if (to < 0 || to > 2){
        continue;
    }

    len  = sizeof(addrs[to]);
    sendto(sockfd, (const char *)buffer, strlen(buffer), 0, (const struct sockaddr *)&addrs[to], len);
  }

  // Display new content of my routing table
  display_routing_table(&myRoutingTable,myId);
  exit(EXIT_SUCCESS);
 }
