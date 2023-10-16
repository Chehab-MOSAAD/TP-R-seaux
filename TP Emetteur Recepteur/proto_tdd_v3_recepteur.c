/*************************************************************
* proto_tdd_v3 -  récepteur                                  *
* TRANSFERT DE DONNEES  v3                                  *
*                                                            *
* Protocole sans contrôle de flux, sans reprise sur erreurs  *
*                                                            *
* E. Lavinal - Univ. de Toulouse III - Paul Sabatier         *
**************************************************************/

#include <stdio.h>
#include "application.h"
#include "couche_transport.h"
#include "services_reseau.h"

/* =============================== */
/* Programme principal - récepteur */
/* =============================== */
int main(int argc, char* argv[])
{
    unsigned char message[MAX_INFO]; /* message pour l'application */
    paquet_t paquet; /* paquet utilisé par le protocole */
    paquet_t pack; /*acquitemment*/
    int fin = 0; /* condition d'arrêt */
    int prochain=0;
    int rep;

    

    init_reseau(RECEPTION);

    printf("[TRP] Initialisation reseau : OK.\n");
    printf("[TRP] Debut execution protocole transport.\n");

  
    /* tant que le récepteur reçoit des données */
    while ( !fin ) {

        de_reseau(&paquet);

        if(verifier_controle(paquet)){
            switch (paquet.type)
            {
                case CON_REQ:
                    //pour ne pas recevir le meme paquet 2 fois 
                    if(paquet.num_seq == prochain){
                        rep = vers_application_mode_c(T_CONNECT,NULL,0);
                        //de_application_mode_c(T_CONNECT_ACCEPT)
                        if (rep == T_CONNECT_ACCEPT)
                        {   
                            pack.type = CON_ACCEPT;
                    
                        }
                        else{
                            pack.type = CON_REFUSE;
                        }
                        prochain = inc(prochain)%SEQ_NUM_SIZE;
                    }
                    vers_reseau(&pack);
                    break;
                case DATA: 
                    if(paquet.num_seq == prochain){
                        /* extraction des donnees du paquet recu */
                        for (int i=0; i<paquet.lg_info; i++) {
                            message[i] = paquet.info[i];
                        }
                        /* remise des données à la couche application */
                        vers_application_mode_c(T_DATA,message, paquet.lg_info);
                        prochain = inc(prochain)%SEQ_NUM_SIZE;
                        pack.type = ACK;
                    }
                    
                    vers_reseau(&pack);
                    break;
                case CON_CLOSE:
                    //pour ne pas recevir le meme paquet 2 fois 
                    if(paquet.num_seq == prochain){
                        //fin = 1; 
                        /*on verifier que le service T_DISCONNECT est bien remonte a l'appli*/
                        if(vers_application_mode_c(T_DISCONNECT,NULL,0)){
                            fin =1; /*si oui on applique la condition d'arret pour sortir de la boucle while */
                            printf("[TRP] DECONNECT.\n");
                        }
                        pack.type = CON_CLOSE_ACK;
                        prochain = inc(prochain)%SEQ_NUM_SIZE;
                    }

                    else{
                        if(vers_application_mode_c(T_DISCONNECT,NULL,0)){
                            fin =1; /*si oui on applique la condition d'arret pour sortir de la boucle while */
                            printf("[TRP] DECONNECT.\n");
                        } 
                    }
                    
                    vers_reseau(&pack);
                    //printf("[TRP] DECONNECT.\n");
                    break;
            }

        }
        
    }
    printf("[TRP] Fin execution protocole transport.\n");
    return 0;
}
