#include <stdio.h>
#include "application.h"
#include "couche_transport.h"
#include "services_reseau.h"

int main(int argc, char* argv[])
{
    unsigned char message[MAX_INFO]; /* message de l'application */
    unsigned int curseur = 0;
    unsigned int borne_inf = 0;
    paquet_t tabp[SEQ_NUM_SIZE];
    paquet_t pack;
    int taille_msg; /* taille du message */
    int taille_fenetre = 4;
    int indTabpACK[SEQ_NUM_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0}; /* Indice des paquets de la fenêtre qui ont été ACK */
    int retour;
    int nbRenvoi;

    init_reseau(EMISSION);

    printf("[TRP] Initialisation reseau : OK.\n");
    printf("[TRP] Debut execution protocole transport.\n");

    /* Lecture des données venant de la couche application */
    de_application(message, &taille_msg);

    /* tant que l'émetteur a des données à envoyer */
    while ( taille_msg != 0 || borne_inf != curseur) {

        if (dans_fenetre(borne_inf, curseur, taille_fenetre) && taille_msg>0) {
          /* Construction du paquet */
          for (int i=0; i<taille_msg; i++)
            tabp[curseur].info[i] = message[i];
          tabp[curseur].type = DATA;
          tabp[curseur].lg_info = taille_msg;
          tabp[curseur].num_seq = curseur;
          tabp[curseur].somme_ctrl = generer_controle(tabp[curseur]);
          vers_reseau(&tabp[curseur]);
          nbRenvoi++;
          depart_temporisateur(curseur, 100);
          inc8((int*)&curseur);
          /* Lecture des données venant de la couche application */
          de_application(message, &taille_msg);
        }

        else {
          retour = attendre();
          /* Si on a reçu un paquet : forte probabilité que ce soit un ACK */
          if (retour==PAQUET_RECU) {
            de_reseau(&pack);
            /* Si il y a eu étape de renvoie précédemment, alors on peut réinitialiser le compteur */
            nbRenvoi = 0;
            /* Si le paquet n'a pas d'erreur et est dans le fenêtre, alors on stock son indice et on décale la fenêtre si c'est en séquence */
            if (verifier_controle(pack) && dans_fenetre(borne_inf, pack.num_seq, taille_fenetre)) {
              indTabpACK[pack.num_seq] = 1;
              arreter_temporisateur(pack.num_seq);
              while (indTabpACK[borne_inf]) {
                indTabpACK[borne_inf] = 0;
                inc8((int*)&borne_inf);
              }
            }
          }
          /* Pas de paquet reçu : timeout donc on renvoie celui qui n'a pas été ACK */
          else {
            vers_reseau(&tabp[retour]);
            depart_temporisateur(retour, 100);
            nbRenvoi++;
            if (nbRenvoi>=MAX_ENVOI) {
              fprintf(stderr, "Arrêt nbRenvoi>=MAX_ENVOI\n");
              return 1;
            }
          }
        }
    }

    printf("[TRP] Fin execution protocole transfert de donnees (TDD).\n");
    return 0;
}
