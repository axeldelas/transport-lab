#include <stdio.h>
#include "application.h"
#include "couche_transport.h"
#include "services_reseau.h"

int main(int argc, char* argv[])
{
    unsigned char message[MAX_INFO]; /* message de l'application */
    int taille_msg; /* taille du message */
    int taille_fenetre = 4;
    unsigned int curseur = 0;
    unsigned int borne_inf = 0;
    paquet_t tabp[SEQ_NUM_SIZE];
    paquet_t pack;
    int retour;
    int indRenvoi;
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
          /* Départ du temporisateur pour l'envoi de la fenêtre : 1 temporisateur pour la fenêtre */
          depart_temporisateur(0, 100);
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
            /* Décalage de la fenêtre de manière cumulative si pas d'erreur du paquet */
            if (verifier_controle(pack) && dans_fenetre(borne_inf, pack.num_seq, taille_fenetre)) {
              borne_inf = (pack.num_seq+1)%SEQ_NUM_SIZE;
              arreter_temporisateur(0);
              /* Si il reste encore des paquets */
              if (!(borne_inf==curseur))
                depart_temporisateur(0, 100);
            }
          }
          /* Pas de paquet reçu : timeout */
          else {
            /* Renvoi de tous les paquets depuis le paquet timeout */
            indRenvoi = borne_inf;
            depart_temporisateur(NUM_TEMP, 100);
            while (!(indRenvoi==curseur)) {
              vers_reseau(&tabp[indRenvoi]);
              inc8(&indRenvoi);
            }
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
