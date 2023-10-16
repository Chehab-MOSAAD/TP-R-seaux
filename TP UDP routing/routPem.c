/*=============================================================*/
//Programme simulant un protocole de routage dynamique simplifié
// Ce programme code uniquement le comportement
// d'émetteur d'une annonce de routage
// vers UN SEUL routeur voisin pour UN échange initial de routes
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

#define BUF_SIZE_OUT 64 // we should send less...
#define IPV4_ADR_STRLEN 16  // == INET_ADDRSTRLEN
#define LOCALHOST "127.0.0.1"
#define NO_BASE_PORT 17900  // base number for computing real port number
#define CL_PORT 8080
#define SERV_PORT 9090
#define MAXBUFF 1024

/* =================================================================== */
/* FONCTION PRINCIPALE : PEER PROCESSUS DE ROUTAGE ROLE EMETTEUR ONLY  */
/* =================================================================== */

int main(int argc, char **argv) {

  // Usage routPem IDIP@ssRouter  MyNumberRouter NeigborNumberRouter
  // Example routPem 10.1.1.1 1 2

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

  int client_number, sockfd;
  client_number = (*argv[1]) - 48;

  struct sockaddr_in servaddr, cliaddr;

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
    perror("erreur creation du socket!");
    exit(EXIT_FAILURE);
    }

  memset(&servaddr, 0, sizeof(servaddr));
  memset(&cliaddr, 0, sizeof(cliaddr));

  // Remplir les informations du client
  cliaddr.sin_family = AF_INET;
  cliaddr.sin_addr.s_addr = INADDR_ANY;
  cliaddr.sin_port = htons(CL_PORT + client_number);

  // Liez le socket avec l'adresse du serveur
  if (bind(sockfd, (const struct sockaddr *)&cliaddr, sizeof(cliaddr)) < 0 ) {
    perror("bind failed");
    exit(EXIT_FAILURE);
    }

  // Remplir les informations du serveur
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(SERV_PORT);

  int len, n, from, to, from_message;

  while (1){
      printf("Type r for receive mode or s for send mode:");
      char response;
      scanf("%c", &response);

      if (response == 's'){
        char buffer[MAXBUFF];
        printf("Enter a message : ");
        scanf("%s", buffer);

        len = sizeof(servaddr);
        sendto(sockfd, (const char *)buffer, strlen(buffer), 0, (const struct sockaddr *)&servaddr, len);
      }

      if (response == 'r'){
        char buffer[MAXBUFF];
        len = sizeof(servaddr);
        n = recvfrom(sockfd, (char *)buffer, MAXBUFF, 0, (struct sockaddr *)&servaddr, &len);
        buffer[n] = '\0';
        printf("%s\n", buffer);
      }
  }
    exit(EXIT_SUCCESS);
}
