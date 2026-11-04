#include <stdio.h>
#include "couche_transport.h"
#include "services_reseau.h"

/* ************************************** */
/* Fonctions utilitaires couche transport */
/* ************************************** */

int generer_controle(paquet_t p) {

  int controle;

  /* Etape 1 : contrôle sur les trois octets de l'en-tête */
  controle = p.type^p.num_seq^p.lg_info;

  /* Etape 2 : contrôle sur tous les octets de données du paquet */
  for (int i=0; i<p.lg_info; i++)
    controle = controle^p.info[i];

  return controle;
}

int verifier_controle(paquet_t p) {

  return generer_controle(p)==p.somme_ctrl;
}

void inc2(int *prochain_paquet) {

  *prochain_paquet = (*prochain_paquet+1)%2;
}

void inc8(int *prochain_paquet) {

  *prochain_paquet = (*prochain_paquet+1)%8;
}

/* ======================================================================= */
/* =================== Fenêtre d'anticipation ============================ */
/* ======================================================================= */

/*--------------------------------------*/
/* Fonction d'inclusion dans la fenetre */
/*--------------------------------------*/
int dans_fenetre(unsigned int inf, unsigned int pointeur, int taille) {

    unsigned int sup = (inf+taille-1) % SEQ_NUM_SIZE;

    return
        /* inf <= pointeur <= sup */
        ( inf <= sup && pointeur >= inf && pointeur <= sup ) ||
        /* sup < inf <= pointeur */
        ( sup < inf && pointeur >= inf) ||
        /* pointeur <= sup < inf */
        ( sup < inf && pointeur <= sup);
}
