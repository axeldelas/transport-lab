#include <stdio.h>
#include "application.h"
#include "couche_transport.h"
#include "services_reseau.h"

int main(int argc, char* argv[])
{
    unsigned char message[MAX_INFO]; /* message pour l'application */
    paquet_t paquet; /* paquet utilisé par le protocole */
    paquet_t pack;
    int fin = 0; /* condition d'arrêt */
    int paquetAttendu = 0;

    init_reseau(RECEPTION);

    printf("[TRP] Initialisation reseau : OK.\n");
    printf("[TRP] Debut execution protocole transport.\n");

    pack.type = ACK;
    pack.lg_info = 0;

    /* tant que le récepteur reçoit des données */
    while ( !fin ) {

        de_reseau(&paquet);

        if (verifier_controle(paquet)) {
          pack.num_seq = paquetAttendu-1;
          pack.somme_ctrl = generer_controle(pack);
          /* Cas où le paquet reçu est en séquence et qu'on ACK ce paquet */
          if (paquet.num_seq==paquetAttendu) {
            /* extraction des données du paquet reçu qui correspond au paquet attendu */
            for (int i=0; i<paquet.lg_info; i++)
              message[i] = paquet.info[i];
            fin = vers_application(message, paquet.lg_info);
            inc8(&paquetAttendu);
          }
          vers_reseau(&pack);
        }
    }

    printf("[TRP] Fin execution protocole transport.\n");
    return 0;
}
