#include <stdio.h>
#include "application.h"
#include "couche_transport.h"
#include "services_reseau.h"

int main(int argc, char* argv[])
{
    unsigned char message[MAX_INFO]; /* message de l'application */
    int taille_msg; /* taille du message */
    int prochain_paquet = 0;
    int nb_envoi;
    int retour;
    paquet_t paquet; /* paquet utilisé par le protocole */

    init_reseau(EMISSION);

    printf("[TRP] Initialisation reseau : OK.\n");
    printf("[TRP] Debut execution protocole transport.\n");

    /* lecture de donnees provenant de la couche application */
    de_application(message, &taille_msg);

    /* tant que l'émetteur a des données à envoyer */
    while ( taille_msg != 0 ) {

        /* construction paquet */
        for (int i=0; i<taille_msg; i++) {
            paquet.info[i] = message[i];
        }
        paquet.lg_info = taille_msg;
        paquet.type = DATA;
        paquet.num_seq = prochain_paquet;
        paquet.somme_ctrl = generer_controle(paquet);

        nb_envoi=0;
        do {
          vers_reseau(&paquet);
          depart_temporisateur(prochain_paquet, 100);
          retour = attendre();
          nb_envoi++;
        } while (!(retour==PAQUET_RECU) && nb_envoi<MAX_ENVOI);

        if (nb_envoi>=MAX_ENVOI) {
          fprintf(stderr, "Arrêt nb_envoi>=MAX_ENVOI\n");
          return 1;
        }

        de_reseau(&paquet);
        arreter_temporisateur(prochain_paquet);
        inc2(&prochain_paquet);

        /* lecture des donnees suivantes de la couche application */
        de_application(message, &taille_msg);
    }

    printf("[TRP] Fin execution protocole transfert de donnees (TDD).\n");
    return 0;
}
