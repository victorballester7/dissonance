#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define TOL 1.e-7
#define MIN_FREQ 20
#define MAX_FREQ 20000

typedef struct {
  int numHarmonics;
  double (*frequencies)[2];
} socomplex;

void nota_musical(double freq_fonamental, socomplex S, double potencia);
socomplex *construccio_sons(int n, int MaxHarmonics, double potencia);
double CBW(double x);
double disso_simple(double f1, double f2, double A1, double A2);
double dissonancia(int n, socomplex *S);

int main() {
  FILE *fitxer;
  int n;
  printf("Introduir 1 si es vol calcular la grafica per a sons simples.\nIntroduir 2 si es vol calcular la grafica per a sons complexos.\nIntroduir 3 si es vol calcular el valor de la dissonancia entre 2 sons.\n");
  scanf("%i", &n);
  int harmonics = 7, punts = 500; //harmonics = nombre d'harmonics, punts = punts del grafic.
  double r_max = 2, r = 1 + (r_max - 1) / punts, potencia = 0.4, f1; //r_max = maxim ratio entre les dues frequencies i potencia es el valor k tal que l'harmonic i-essim te amplitud 1/i^k. F1[harmonics][2] = llista d'harmonics. A la primera columna la freqüència; a la segona, l'amplitud.
  switch (n) {
    case 1: //imprimeix en un fitxer de text una taula per graficar (so simple).
      printf("Introduir una frequencia base:\n");
      scanf("%lf", &f1);
      fitxer = fopen("diss.txt", "w");
      if (fitxer == NULL) {
        printf("No s'ha pogut crear el fitxer.");
        return 1;
      }
      for (int k = 0; k <= punts; k++, r += (r_max - 1) / punts)
        fprintf(fitxer, "%.16G %.16G\n", r, disso_simple(f1, r * f1, 1, 1));
      fclose(fitxer);
      break;
    case 2: { //imprimeix en un fitxer de text una taula per graficar (so complex).
      socomplex *S = construccio_sons(2, harmonics, potencia); // s'ha d'introduir dues vegades la mateixa frequencia inicial
      if (S == NULL)
        return 1;
      fitxer = fopen("diss.txt", "w");
      if (fitxer == NULL) {
        printf("No s'ha pogut crear el fitxer.");
        return 1;
      }
      fprintf(fitxer, "%.16G %.16G\n", r, dissonancia(2,S));
      f1 = S[0].frequencies[0][0];
      for (int k = 0; k < punts; k++, r += (r_max - 1) / punts) {
        S[1].numHarmonics = 1;
        while ((S[1].numHarmonics + 1) * r * f1 <= MAX_FREQ && S[1].numHarmonics < harmonics) (S[1].numHarmonics)++; //afegeix tants harmonics com sigui possible de f1 (sense superar "harmonics") a "harmonics_F2"
        nota_musical(r * f1, S[1], potencia);
        fprintf(fitxer, "%.16G %.16G\n", r, dissonancia(2,S));
      }
      fclose(fitxer);
      break;
    }
    case 3: { //Valor de dissonacia entre dos sons introduits manualment.
      printf("Introduir el nombre de sons: ");
      scanf("%i", &n);
      socomplex *S = construccio_sons(n, harmonics, potencia);
      if (S == NULL)
        return 1;
      printf("\nDissonancia: %.16G\n", dissonancia(n,S));
      return 1;
    }
    default:
      printf("No s'ha introduit un valor correcte.\n");
      return 1;
  }
  return 0;
}

void nota_musical(double freq_fonamental, socomplex S, double potencia) { //potencia es el valor k tal que l'harmonic i-essim te amplitud 1/i^k.
  for (int i = 1; i <= S.numHarmonics; i++) {
    S.frequencies[i - 1][0] = i * freq_fonamental;
    S.frequencies[i - 1][1] = 1. / pow(i, potencia);
  }
}

socomplex *construccio_sons(int n, int MaxHarmonics, double potencia){
  double f;
  static socomplex *S;
  S = (socomplex *) malloc(n * sizeof(socomplex));
  if (S == NULL){
    printf("Error en l'assignacio de memoria.\n");
    return NULL;
  }
  for (int i = 0; i < n; i++) {
    printf("Introduir la frequencia fonamental del so %i: ", i + 1);
    scanf("%lf", &f);
    if (f < MIN_FREQ || f > MAX_FREQ) {
      printf("Error! Hi ha una frequencia menor a 20 Hz o major a 20000 Hz.\n");
      return NULL;
    }
    while ((S[i].numHarmonics + 1) * f <= MAX_FREQ && S[i].numHarmonics < MaxHarmonics) (S[i].numHarmonics)++; //afegeix tants MaxHarmonics com sigui possible de f (sense superar "MaxHarmonics") a "harmonics_F1"
    S[i].frequencies = (double (*) [2]) malloc(S[i].numHarmonics * 2 * sizeof(double));
    if (S[i].frequencies == NULL) {
      printf("Error en l'assignacio de memoria.\n");
      return NULL;
    }
    nota_musical(f, S[i], potencia);
  }
  return S;
}

double CBW(double f) { //CBW = criticalbandwidth de la frequencia f.
  return 1.72 * pow(f, 0.65);
}

double disso_simple(double f1, double f2, double A1, double A2) {
  double max_diss = 0.25;
  double x = fabs(f1 - f2) / (CBW((f1 + f2) / 2) * max_diss); //max_diss = maxima dissonancia en termes de la criticalband.
  return A1 * A2 * x * exp(1 - x);
}

double dissonancia(int n, socomplex S[n]){
  double disso = 0;
  for (int k = 0; k < n; k++) {
    for (int m = k; m < n; m++) {
      for (int i = 0; i < S[k].numHarmonics; i++) {
        for (int j = 0; j < S[m].numHarmonics; j++) { //nomes hem de comptar els termes (i, j) una vegada.
          if (m == k)
            disso += 0.5 * disso_simple(S[k].frequencies[i][0], S[m].frequencies[j][0], S[k].frequencies[i][1], S[m].frequencies[j][1]);
          else if (fabs(S[k].frequencies[0][0] - S[m].frequencies[0][0]) > TOL) //no es compta el mateix so repetit dues vegades.
            disso += disso_simple(S[k].frequencies[i][0], S[m].frequencies[j][0], S[k].frequencies[i][1], S[m].frequencies[j][1]);
        }
      }
    }
  }
  return disso;
}