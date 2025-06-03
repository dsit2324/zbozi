#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUTFILE "zbozi.txt"
#define OUTPUTTXT "vypis_zbozi.txt"
#define ODD ";\n"
#define MAX 100

typedef struct {
    char dodavatel[10];
    char nazev[100];
    int hmotnost;
    int pocet;
    float cena;
} ZBOZI;

ZBOZI *nactiSoubor(ZBOZI *seznam, int *pocet) {
    FILE *fr = fopen(INPUTFILE, "r");
    if (fr == NULL) {
        printf("Chyba: soubor %s se nepodarilo otevrit.\n", INPUTFILE);
        return NULL;
    }

    char radek[256];
    int i = 0;
    ZBOZI *novy = NULL;

    fgets(radek, sizeof(radek), fr); // preskocit hlavicku

    while (fgets(radek, sizeof(radek), fr)) {
        novy = (ZBOZI *)realloc(seznam, sizeof(ZBOZI) * (i + 1));
        if (novy == NULL) {
            printf("Chyba alokace pameti.\n");
            free(seznam);
            return NULL;
        }
        seznam = novy;

        char *token = strtok(radek, ODD);
        int j = 0;

        while (token != NULL) {
            switch (j) {
                case 0: strcpy(seznam[i].dodavatel, token); break;
                case 1: strcpy(seznam[i].nazev, token); break;
                case 2: seznam[i].hmotnost = atoi(token); break;
                case 3: seznam[i].pocet = atoi(token); break;
                case 4: seznam[i].cena = atof(token); break;
            }
            token = strtok(NULL, ODD);
            j++;
        }
        i++;
    }

    fclose(fr);
    *pocet = i;
    return seznam;
}

void nahradPodtrzitka(char *text) {
    for (int i = 0; text[i]; i++) {
        if (text[i] == '_') {
            text[i] = ' ';
        }
    }
}

void vypisSeznam(ZBOZI *seznam, int pocet) {
    float celkovaCena = 0;
    int celkovaHmotnost = 0;
    float nejvyssiCena = 0;
    char nejdrazsiNazev[100];

    printf("U C T E N KA\n");
    printf("dodavatel   nazev zbozi hmotnost pocet   cena     cena\n");
    printf("(zkratka)               (1 kusu)  kusu  (1 kusu)  celkem\n");
    printf("---------------------------------------------------------\n");

    for (int i = 0; i < pocet; i++) {
        float cenaCelkem = seznam[i].cena * seznam[i].pocet;
        char upravenyNazev[100];
        strcpy(upravenyNazev, seznam[i].nazev);
        nahradPodtrzitka(upravenyNazev);

        printf("%-5s %-20s %3d g %5d %8.2f Kc  %5.2f Kc\n",
               seznam[i].dodavatel,
               upravenyNazev,
               seznam[i].hmotnost,
               seznam[i].pocet,
               seznam[i].cena,
               cenaCelkem);

        celkovaCena += cenaCelkem;
        celkovaHmotnost += seznam[i].hmotnost * seznam[i].pocet;

        if (seznam[i].cena > nejvyssiCena) {
            nejvyssiCena = seznam[i].cena;
            strcpy(nejdrazsiNazev, seznam[i].nazev);
        }
    }

    nahradPodtrzitka(nejdrazsiNazev);

    printf("------------------------------------------------------------\n");
    printf("Cena celkem               : %.2f Kc\n", celkovaCena);
    printf("Pocet polozek             : %d\n", pocet);
    printf("Celkova hmotnost zbozi    : %d kg %d g\n", celkovaHmotnost / 1000, celkovaHmotnost % 1000);
    printf("Nejdrazsi nakoupene zbozi : %s (%.2f Kc)\n", nejdrazsiNazev, nejvyssiCena);
}

void ulozTXT(ZBOZI *seznam, int pocet) {
    FILE *fw = fopen(OUTPUTTXT, "w");
    if (fw == NULL) {
        printf("Chyba: nelze vytvorit soubor %s.\n", OUTPUTTXT);
        return;
    }

    fprintf(fw, "Zbozi od externich dodavatelu\n");
    fprintf(fw, "dodavatel      nazev            hmotnost v gramech   cena 1 kusu v Kc\n");

    for (int i = 0; i < pocet; i++) {
        if (strcmp(seznam[i].dodavatel, "OP") != 0) {
            char upravenyNazev[100];
            strcpy(upravenyNazev, seznam[i].nazev);
            nahradPodtrzitka(upravenyNazev);

            fprintf(fw, "%-10s  %-23s     %5d          %.2f\n",
                    seznam[i].dodavatel,
                    upravenyNazev,
                    seznam[i].hmotnost,
                    seznam[i].cena);
        }
    }

    fclose(fw);
    printf("Byl vytvoren soubor %s.\n", OUTPUTTXT);
}

int main() {
    ZBOZI *seznam = NULL;
    int pocet = 0;

    seznam = nactiSoubor(seznam, &pocet);
    if (seznam == NULL) {
        return 1;
    }

    vypisSeznam(seznam, pocet);
    ulozTXT(seznam, pocet);

    free(seznam);
    return 0;
}
