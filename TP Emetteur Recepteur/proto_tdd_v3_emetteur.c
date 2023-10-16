/*************************************************************
* proto_tdd_v3 -  émetteur                                   *
* TRANSFERT DE DONNEES  v3                                   *
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
/* Programme principal - émetteur  */
/* =============================== */
int main(int argc, char* argv[])
{
    unsigned char message[MAX_INFO]; /* message de l'application */
    int taille_msg; /* taille du message */
    paquet_t paquet; /* paquet utilisé par le protocole */
    paquet_t pack;
    int service;
    int evt;
    int prochain=0;


    init_reseau(EMISSION);

    printf("[TRP] Initialisation reseau : OK.\n");
    printf("[TRP] Debut execution protocole transport.\n");


    de_application_mode_c(&service, message,&taille_msg);
   
    if(service == T_CONNECT){
        paquet.lg_info = 0;
        paquet.type = CON_REQ;
        paquet.num_seq = prochain;
        paquet.somme_ctrl = generer_controle(paquet);

       
        /*pour gerer la perte des paquets*/
        /*cas PERTE_CON_REQ*/
        vers_reseau(&paquet);
        depart_temporisateur(100);
        evt = attendre();

        while(evt != -1){
            vers_reseau(&paquet);
            depart_temporisateur(100);
            evt = attendre();
        }
            
        de_reseau(&pack);
        arret_temporisateur();
         
        if(pack.type == CON_ACCEPT ){
            vers_application_mode_c(T_CONNECT_ACCEPT,NULL,0);
        }
        else{
            vers_application_mode_c(T_CONNECT_REFUSE,NULL,0);
        }
        /*cas PERTE_CON_ACCEPT*/
        prochain = inc(prochain)% SEQ_NUM_SIZE;
        
    }


    de_application_mode_c(&service, message,&taille_msg);
    while(service == T_DATA){
        /* tant que l'émetteur a des données à envoyer */
        

        /* construction paquet */
        for (int i=0; i<taille_msg; i++) {
            paquet.info[i] = message[i];
        }
        paquet.lg_info = taille_msg;
        paquet.type = DATA;
        paquet.num_seq = prochain;
        paquet.somme_ctrl = generer_controle(paquet);
      
        /* remise à la couche reseau */
        vers_reseau(&paquet);
        depart_temporisateur(100);
        evt = attendre();

        while(evt != -1){
            vers_reseau(&paquet);
            depart_temporisateur(100);
            evt = attendre();
        }
            
        de_reseau(&pack);
        arret_temporisateur();
        

       
        prochain = inc(prochain)% SEQ_NUM_SIZE;
        /* lecture des donnees suivantes de la couche application */
        de_application_mode_c(&service, message,&taille_msg);
    }

    
   
    if(service == T_DISCONNECT){
        paquet.lg_info = 0;
        paquet.type = CON_CLOSE;
        paquet.num_seq = prochain; 
        paquet.somme_ctrl = generer_controle(paquet);

        /*pour gerer la perte des paquets*/
        /*cas PERTE_CON_CLOSE*/
        vers_reseau(&paquet);
        depart_temporisateur(100);
        evt = attendre();

        while(evt != -1){
            vers_reseau(&paquet);
            depart_temporisateur(100);
            evt = attendre();
        }
            
        de_reseau(&pack);
        arret_temporisateur();
        
    
        if(pack.type == CON_CLOSE_ACK ){
          printf("[TRP] Fin execution protocole transfert de donnees (TDD).\n");
        }
        /*cas PERTE_CON_CLOSE_ACK*/
        prochain = inc(prochain)% SEQ_NUM_SIZE;
        
    }
    return 0;
}
