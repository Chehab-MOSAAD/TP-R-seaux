/* =================================================================== */
// Progrmame Client à destination d'un joueur qui doit deviner la case
// du trésor. Après chaque coup le résultat retourné par le serveur est
// affiché. Le coup consite en une abcsisse et une ordonnée (x, y).
/* =================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define N 10
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define MAGENTA "\033[35m"

/* ====================================================================== */
/*                  Affichage du jeu en mode texte brut                   */
/* ====================================================================== */
void afficher_jeu(int jeu[N][N], int res, int points, int coups)
{

    printf("\n************ TROUVEZ LE TRESOR ! ************\n");
    printf("    ");
    for (int i = 0; i < 10; i++)
        printf("  %d ", i + 1);
    printf("\n    -----------------------------------------\n");
    for (int i = 0; i < 10; i++){
        printf("%2d  ", i + 1);
        for (int j = 0; j < 10; j++){
            printf("|");
            switch (jeu[i][j]){
            case -1:
                printf(" 0 ");
                break;
            case 0:
                printf(GREEN " T " RESET);
                break;
            case 1:
                printf(YELLOW " %d " RESET, jeu[i][j]);
                break;
            case 2:
                printf(RED " %d " RESET, jeu[i][j]);
                break;
            case 3:
                printf(MAGENTA " %d " RESET, jeu[i][j]);
                break;
            }
        }
        printf("|\n");
    }
    printf("    -----------------------------------------\n");
    printf("Pts dernier coup %d | Pts total %d | Nb coups %d\n", res, points, coups);
}

/* ====================================================================== */
/*                    Fonction principale                                 */
/* ====================================================================== */
int main(int argc, char **argv){

    int jeu[N][N];
    int lig, col;
    int res = -1, points = 0, coups = 0;

    /* Init args */
    if (argc != 3){
        printf(" problem de entrer ");
        exit(1);
    }

    // char client = argv[0];
    char *Ipclient = argv[1];
    char *nbPorte = argv[2];

    /* Init jeu */
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            jeu[i][j] = -1;

    /* Creation socket TCP */
    int sok_id;
    int domaine = AF_INET;
    int type = SOCK_STREAM;
    int protocole = 0;

    sok_id = socket(domaine, type, protocole);
    if (sok_id == -1){
        perror("Socket creation error");
        return EXIT_FAILURE;
    }

    /* Init caracteristiques serveur distant (struct sockaddr_in) */
    struct sockaddr_in dst_serv_adr;

    int nb_port_to_int = atoi(nbPorte);
    memset(&dst_serv_adr, 0, sizeof(dst_serv_adr));
    dst_serv_adr.sin_family = AF_INET;
    dst_serv_adr.sin_port = htons(nb_port_to_int);
    inet_pton(AF_INET, Ipclient, &(dst_serv_adr.sin_addr));

    /* Etablissement connexion TCP avec process serveur distant */
    if (connect(sok_id, (struct sockaddr *)&dst_serv_adr, sizeof(dst_serv_adr)) == -1){
        perror("Connection error");
        close(sok_id);
        return EXIT_FAILURE;
    }

    /* Tentatives du joueur : stoppe quand tresor trouvé */
    do{
        afficher_jeu(jeu, res, points, coups);
        printf("\nEntrer le numéro de ligne : ");
        scanf("%d", &lig);
        printf("Entrer le numéro de colonne : ");
        scanf("%d", &col);

        /* Construction requête (serialisation en chaines de caractères) */
        char message_to_send[4];
        sprintf(message_to_send, "%d %d", lig, col);
        int nb_octet_message = 4;

        /* Envoi de la requête au serveur (send) */
        send(sok_id, message_to_send, nb_octet_message, 0);

        /* Réception du resultat du coup (recv) */
        char recieved_message[2];

        /* Deserialisation du résultat en un entier */
        int nb_octet_to_recieve = 2;
        recv(sok_id, recieved_message, nb_octet_to_recieve, 0);
        // printf("print recievd mess %d",recieved_message);

        res = atoi(recieved_message);

        /* Mise à jour */
        if (lig >= 1 && lig <= N && col >= 1 && col <= N)
            jeu[lig - 1][col - 1] = res;
        points += res;
        coups++;

    } while (res);

    /* Fermeture connexion TCP */

    if (shutdown(sok_id, SHUT_RDWR) == -1) {
        perror("Shutdown error");
        close(sok_id);
        return EXIT_FAILURE;
    }

    if (close(sok_id) == -1) {
        perror("Close error");
        return EXIT_FAILURE;
    }

    afficher_jeu(jeu, res, points, coups);
    printf("\nBRAVO : trésor trouvé en %d essai(s) avec %d point(s)"
           " au total !\n\n",
           coups, points);
    return 0;
}
