/* =================================================================== */
// Progrmame Serveur qui calcule le résultat d'un coup joué à partir
// des coordonnées reçues de la part d'un client "joueur".
// Version CONCURRENTE : N clients/joueurs à la fois
/* =================================================================== */

#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "tresor.h"
#define nbPorteServeur 5555

/* =================================================================== */
/* FONCTION PRINCIPALE : SERVEUR CONCURRENT                            */
/* =================================================================== */
int main(int argc, char **argv){
    char recievedCoordinate[4], lesPointsChaine[2];
    int lengthMessage = 4, tailleMat = 10, coordLigneTres = 2, coordColTres = 5, lenMessage = 2, lesPoints = -1;
    int ligneCoordinate, colCoordinate;

    int servSocket, domaine = AF_INET, type = SOCK_STREAM, protocIP = 0;

    // Creation socket
    servSocket = socket(domaine, type, protocIP);
    if (servSocket == -1){
        printf("Erreur de creation de socket \n");
        exit(1);
    }

    // Binding associer une adrese socket
    struct sockaddr_in addServeur;

    memset(&addServeur, 0, sizeof(addServeur));
    addServeur.sin_family = AF_INET;
    addServeur.sin_port = htons(nbPorteServeur);
    addServeur.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(servSocket, (struct sockaddr *)&addServeur, sizeof(addServeur)) == -1){
        perror("Bind error");
        close(servSocket);
        return EXIT_FAILURE;
    }

    // Metre en ecoute listen
    int nbMaxdemand = 3;
    int serveurEcout = listen(servSocket, nbMaxdemand);
    if (serveurEcout == -1){
        perror("Listen error");
        close(servSocket);
        return EXIT_FAILURE;
    }

    // Accepter une connexion TCP entrante et faire une fork pour créer une fils qui sera dédiée à chaque nouvelle connexion
    struct sockaddr addrClient;
    socklen_t sizeAddClient = sizeof(addrClient);

    int socketClient = accept(servSocket, (struct sockaddr *)&addrClient, &sizeAddClient);

    if (socketClient < 0){
        perror("accept failed");
        close(servSocket);
        exit(EXIT_FAILURE);
    }

    unsigned short echoServPort;
    pid_t processID;
    unsigned int childProcCount = 0;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <Server Port>\n", argv[0]);
        exit(1);
    }

    echoServPort = atoi(argv[1]);
    servSocket = CreateTCPServerSocket(echoServPort);

    for (;;) {
        socketClient = AcceptTCPConnection(servSocket);

        if ((processID = fork()) < 0) DieWithError (“fork() failed”);
        else if (processID = 0) {

            while (lesPoints != 0){
                printf("a\n");
                recv(socketClient, recievedCoordinate, lengthMessage, 0);
                printf("b %s\n", recievedCoordinate);
                ligneCoordinate = recievedCoordinate[0] - '0';
                colCoordinate = recievedCoordinate[2] - '0';
                printf(" c %d %d \n ", ligneCoordinate, colCoordinate);

                int reponseRecherTresor = recherche_tresor(tailleMat, coordLigneTres, coordColTres, ligneCoordinate, colCoordinate);
                printf(" d %d", reponseRecherTresor);
                lesPoints = reponseRecherTresor;

                sprintf(lesPointsChaine, "%d", lesPoints);
                send(socketClient, lesPointsChaine, lenMessage, 0);
            }

            close(servSocket);
            HandleTCPClient(socketClient);
            exit(0);
            }

            close(socketClient);
            childProcCount++;

            while (childProcCount) {
                processID = waitpid((pid_t) -1, NULL, WHOANG);

                if (processID < 0) DieWithError (“...”);
                else if (processID == 0) break;
                else childProcCount--;
            }
        }

    return 0;
} // end main
