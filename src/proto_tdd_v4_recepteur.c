#include <stdio.h>
#include "application.h"
#include "couche_transport.h"
#include "services_reseau.h"

int main(int argc, char* argv[])
{
    paquet_t paquet; /* paquet utilisé par le protocole */
    paquet_t pack;
    paquet_t buffer[SEQ_NUM_SIZE]; /* Buffer pour les paquets */
    int taille_fenetre = 4;
    int indTabpACK[SEQ_NUM_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0}; /* Indice des paquets de la fenêtre qui ont été ACK */
    int fin = 0; /* condition d'arrêt */
    int borne_inf = 0;

    init_reseau(RECEPTION);

    printf("[TRP] Initialisation reseau : OK.\n");
    printf("[TRP] Debut execution protocole transport.\n");

    pack.type = ACK;
    //pack.lg_info = 0;

    /* tant que le récepteur reçoit des données */
    while ( !fin ) {

        de_reseau(&paquet);

        if (verifier_controle(paquet)) {
          pack.num_seq = paquet.num_seq;
          pack.somme_ctrl = generer_controle(pack);
          /* Si on a reçu un paquet qu'on n'a pas encore bufferisé et qui est dans la fenêtre */
          if (indTabpACK[paquet.num_seq]!=1 && dans_fenetre(borne_inf, paquet.num_seq, taille_fenetre)) {
            indTabpACK[paquet.num_seq] = 1;
            buffer[paquet.num_seq] = paquet;
          }
          /* Si on a un/des ACK en séquence de dispo alors on peut les envoyer à l'application et décaler la fenêtre */
          while (indTabpACK[borne_inf]) {
            fin = vers_application(buffer[borne_inf].info, buffer[borne_inf].lg_info);
            indTabpACK[borne_inf] = 0;
            inc8(&borne_inf);
          }
          vers_reseau(&pack);
        }
    }

    printf("[TRP] Fin execution protocole transport.\n");
    return 0;
}
