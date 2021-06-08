#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define TOL 1.e-7
#define MIN_FREQ 20
#define MAX_FREQ 20000

typedef struct{
  double f; // frequency
  double a; // amplitude
} sosimple;

typedef struct {
  int numHarmonics; // number of harmonics
  sosimple *s; // vector of "numHarmonics" simple sounds
} socomplex;

void nota_musical(double freq_fonamental, socomplex S, double potencia);
int construccio_sons(int n, socomplex S[n], int MaxHarmonics, double potencia);
double CBW(double x);
double delta(sosimple s1, sosimple s2);
double d(socomplex S1, socomplex S2);
double dissonace(int n, socomplex S[n]);

int main() {
  FILE *fitxer;
  int n;
  printf("Introduir 1 si es vol calcular la grafica per a sons simples.\nIntroduir 2 si es vol calcular la grafica per a sons complexos.\nIntroduir 3 si es vol calcular el valor de la dissonancia entre 2 sons.\nIntroduir 4 si es vol calcular el grafic de dissonancia en per una fraccio p/q.\n");
  scanf("%i", &n);
  int harmonics = 9, punts = 500; //harmonics = nombre d'harmonics, punts = punts del grafic.
  double r_max = 2, r = 1, potencia = 0.75, f1; //r_max = maxim ratio entre les dues frequencies i potencia es el valor k tal que l'harmonic i-essim te amplitud 1/i^k. F1[harmonics][2] = llista d'harmonics. A la primera columna la freqüència; a la segona, l'amplitud.
  switch (n) {
    case 1: //imprimeix en un fitxer de text una taula per graficar (so simple).
      printf("Introduir una frequencia base:\n");
      scanf("%lf", &f1);
      fitxer = fopen("diss.txt", "w");
      if (fitxer == NULL) {
        printf("No s'ha pogut crear el fitxer.");
        return 1;
      }
      sosimple s1={f1,1},s2={f1,1};
      for (int k = 0; k <= punts; k++, r += (r_max - 1) / punts){
        s2.f = r * f1;
        fprintf(fitxer, "%.16G %.16G\n", r, delta(s1,s2));
      }
      fclose(fitxer);
      break;
    case 2: { //imprimeix en un fitxer de text una taula per graficar (so complex).
      socomplex S[2];
      int t = construccio_sons(1, S, harmonics, potencia); // s'ha d'introduir dues vegades la mateixa frequencia inicial
      if (t == 1)
        return 1;
      fitxer = fopen("diss.txt", "w");
      if (fitxer == NULL) {
        printf("No s'ha pogut crear el fitxer.");
        return 1;
      }
      f1 = S[0].s[0].f;
      for (int k = 0; k <= punts; k++, r += (r_max - 1) / punts) {
        S[1].numHarmonics = 0;
        while ((S[1].numHarmonics + 1) * r * f1 <= MAX_FREQ && S[1].numHarmonics < harmonics) (S[1].numHarmonics)++; //afegeix tants harmonics  a "S[1].numHarmonics" com sigui possible de r * f1 (sense superar "harmonics")
        if (k == 0) {
          S[1].s = (sosimple *) malloc(S[1].numHarmonics * sizeof(sosimple));
          if (S[1].s == NULL) {
            printf("Error en l'assignacio de memoria.\n");
            return 1;
          }
        }
        nota_musical(r * f1, S[1], potencia);
        if (fabs(r - 1) < TOL)
          fprintf(fitxer, "%.16G %.16G\n", r, dissonace(1,S));
        else
          fprintf(fitxer, "%.16G %.16G\n", r, dissonace(2,S));
      }
      fclose(fitxer);
      break;
    }
    case 3: { //Valor de dissonacia entre n sons.
      printf("Introduir el nombre de sons complexos: ");
      scanf("%i", &n);
      socomplex *S = (socomplex *) malloc(n * sizeof(socomplex));
      if (S == NULL) {
        printf("Error en l'assignacio de memoria.\n");
        return 1;
      }
      int t = construccio_sons(n, S, harmonics, potencia);
      if (t == 1)
        return 1;
      printf("\nDissonancia: %.16G\n", dissonace(n,S));
      return 1;
    }
    case 4:{
      int p, q;
      double f1_min = 60, f1_max = 1000;
      socomplex S[2];
      printf("Introduir dos nombres p i q, tals que p > q: ");
      scanf("%i%i", &p, &q);
      fitxer = fopen("diss.txt", "w");
      if (fitxer == NULL) {
        printf("No s'ha pogut crear el fitxer.");
        return 1;
      }
      f1 = f1_min;
      for (int k = 0; k <= punts; k++, f1 += (f1_max - f1_min) / punts) {
        for (int i = 0; i < 2; i++) {
          double f2 = pow(p * 1. / q, i) * f1;
          S[i].numHarmonics = 1;
          while ((S[i].numHarmonics + 1) * f2 <= MAX_FREQ && S[i].numHarmonics < harmonics) (S[i].numHarmonics)++; //afegeix tants harmonics  a "S[1].numHarmonics" com sigui possible de r * f1 (sense superar "harmonics")
          S[i].s = (sosimple *) malloc(S[i].numHarmonics * sizeof(sosimple));
          if (S[i].s == NULL) {
            printf("Error en l'assignacio de memoria.\n");
            return 1;
          }
          nota_musical(f2, S[i], potencia);
        }
        fprintf(fitxer, "%.16G %.16G\n", f1, dissonace(2,S));
      }
      return 4;
    }
    default:
      printf("No s'ha introduit un valor correcte.\n");
      return 1;
  }
  return 0;
}

void nota_musical(double freq_fonamental, socomplex S, double potencia) { //potencia es el valor k tal que l'harmonic i-essim te amplitud 1/i^k.
  for (int i = 1; i <= S.numHarmonics; i++) {
    S.s[i - 1].f = i * freq_fonamental;
    S.s[i - 1].a = 1. / pow(i, potencia);
  }
}

int construccio_sons(int n, socomplex S[n], int MaxHarmonics, double potencia){// if n = -1, it means we want to introduce only one complex tone, but memory for two complex tones.
  double f;
  for (int i = 0; i < n; i++) {
    printf("Introduir la frequencia fonamental del so %i: ", i + 1);
    scanf("%lf", &f);
    if (f < MIN_FREQ || f > MAX_FREQ) {
      printf("Error! Hi ha una frequencia menor a 20 Hz o major a 20000 Hz.\n");
      return  1;
    }
    S[i].numHarmonics = 0;
    while ((S[i].numHarmonics + 1) * f <= MAX_FREQ && S[i].numHarmonics < MaxHarmonics) (S[i].numHarmonics)++; //afegeix tants harmonics  a "S[i].numHarmonics" com sigui possible de f (sense superar "MaxHarmonics"). S[i].numHarmonics per defecte es 0.
    S[i].s = (sosimple *) malloc(S[i].numHarmonics * sizeof(sosimple));
    if (S[i].s == NULL) {
      printf("LError en l'assignacio de memoria.\n");
      return 1;
    }
    nota_musical(f, S[i], potencia);
  }
  return 0;
}

double CBW(double f) { //CBW = criticalbandwidth de la frequencia f.
  return 1.72 * pow(f, 0.65);
}

double delta(sosimple s1, sosimple s2) {
  double max_diss = 0.25;
  double x = fabs(s1.f - s2.f) / (CBW((s1.f + s2.f) / 2) * max_diss); //max_diss = maxima dissonancia en termes de la criticalband.
  return s1.a * s2.a * x * exp(1 - x);
}

double d(socomplex S1, socomplex S2) {
  double diss = 0;
  for (int i = 0; i < S1.numHarmonics; i++) {
    for (int j = 0; j < S2.numHarmonics; j++)
      diss += delta(S1.s[i], S2.s[j]) / 2;
  }
  return diss;
}

double dissonace(int n, socomplex S[n]) {// dissonace between "n" complex tones
  double diss = 0;
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++)
      diss += d(S[i], S[j]);
  }
  return diss;
}