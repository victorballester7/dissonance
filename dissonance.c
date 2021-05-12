#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define TOL 1.e-7
#define MIN_FREQ 20
#define MAX_FREQ 20000

void nota_musical(double freq_fonamental, int harmonics, double potencia, double F[harmonics][2]);
void nota_musical_manual(double freq_fonamental, int harmonics, double F[harmonics][2]);
double CBW(double x);
double disso_simple(double f1, double f2, double A1, double A2);
double disso_complexa(int harmonics_F1, int harmonics_F2, double F1[harmonics_F1][2], double F2[harmonics_F2][2]);

int main() {
  FILE *fitxer;
  int n;
  printf("Introduir 1 si es vol calcular la grafica per a sons simples.\nIntroduir 2 si es vol calcular la grafica per a sons complexos.\nIntroduir 3 si es vol calcular el valor de la dissonancia entre 2 sons.\n");
  scanf("%i", &n);
  int harmonics = 7, harmonics_F1, harmonics_F2, punts = 500; //harmonics = nombre d'harmonics, punts = punts del grafic.
  double r_max = 2, r = 1, potencia = 0.4, f1, f2, F1[harmonics][2], F2[harmonics][2]; //r_max = maxim ratio entre les dues frequencies i potencia es el valor k tal que l'harmonic i-essim te amplitud 1/i^k. F1[harmonics][2] = llista d'harmonics. A la primera columna la freqüència; a la segona, l'amplitud.
  switch (n) {
    case 1://So simple (taula per graficar).
      printf("Introduir una frequencia base:\n");
      scanf("%lf", &f1);
      fitxer = fopen("diss.txt", "w");
      if (fitxer == NULL) {
        printf("No s'ha pogut crear el fitxer.");
        return 1;
      }
      for (int k = 0; k <= punts; k++, r += (r_max - 1)/punts)
        fprintf(fitxer, "%.16G %.16G\n", r, disso_simple(f1, r * f1, 1, 1));
      fclose(fitxer);
      break;
    case 2: //So complex (taula per graficar).
      printf("Introduir una frequencia base:\n");
      scanf("%lf", &f1);
      if (f1 < MIN_FREQ || f1 > MAX_FREQ) {
        printf("Error! La frequencia es menor a 20 Hz o major a 20000 Hz.\n");
        return 1;
      }
      for (harmonics_F1 = 1; (harmonics_F1 + 1) * f1 <= MAX_FREQ && harmonics_F1 < harmonics; harmonics_F1++) {} //afegeix tants harmonics com sigui possible de f1 (sense superar "harmonics" a "harmonics_F1")
      nota_musical(f1, harmonics, potencia, F1);
      fitxer = fopen("diss.txt", "w");
      if (fitxer == NULL) {
        printf("No s'ha pogut crear el fitxer.");
        return 1;
      }
      for (int k = 0; k <= punts; k++, r += (r_max - 1)/punts) {
        for (harmonics_F2 = 1; (harmonics_F2 + 1) * f1 <= MAX_FREQ && harmonics_F2 < harmonics; harmonics_F2++) {} //afegeix tants harmonics com sigui possible de f2 (sense superar "harmonics" a "harmonics_F2")
        nota_musical(r*f1, harmonics_F2, potencia, F2);
        fprintf(fitxer, "%.16G %.16G\n", r, disso_complexa(harmonics_F1, harmonics_F2, F1, F2));
      }
      fclose(fitxer);
      break;
    case 3: //Valor de dissonacia entre dos sons introduits manualment.
      printf("Introduir la frequencia fonamental del so 1: ");
      scanf("%lf", &f1);
      printf("Introduir la frequencia fonamental del so 2: ");
      scanf("%lf", &f2);
      if (f1 < MIN_FREQ || f1 > MAX_FREQ || f2 < MIN_FREQ || f2 > MAX_FREQ) {
        printf("Error! Hi ha una frequencia menor a 20 Hz o major a 20000 Hz.\n");
        return 1;
      }
      for (harmonics_F1 = 1; (harmonics_F1 + 1) * f1 <= MAX_FREQ && harmonics_F1 < harmonics; harmonics_F1++) {} //afegeix tants harmonics com sigui possible de f2 (sense superar "harmonics" a "harmonics_F1")
      for (harmonics_F2 = 1; (harmonics_F2 + 1) * f2 <= MAX_FREQ && harmonics_F2 < harmonics; harmonics_F2++) {}//afegeix tants harmonics com sigui possible de f1 (sense superar "harmonics" a "harmonics_F2")
      nota_musical(f1, harmonics_F1, potencia, F1);
      nota_musical(f2, harmonics_F2, potencia, F2);
      printf("\nDissonancia: %.16G\n", disso_complexa(harmonics, harmonics, F1, F2));
      return 1;
    default:
      printf("No s'ha introduit un valor correcte.\n");
      return 1;
  }
  return 0;
}

void nota_musical(double freq_fonamental, int harmonics, double potencia, double F[harmonics][2]) { //potencia es el valor k tal que l'harmonic i-essim te amplitud 1/i^k.
  for (int i = 1; i <= harmonics; i++) {
    F[i-1][0] = i * freq_fonamental;
    F[i-1][1] = 1./pow(i, potencia);
  }
}

double CBW(double f) { //CBW=criticalbandwidth de la frequencia f.
  return 1.72 * pow(f, 0.65);
}

double disso_simple(double f1, double f2, double A1, double A2) {
  double max_diss = 0.25;
  double x = fabs(f1 - f2)/(CBW((f1 + f2)/2) * max_diss); //max_diss=maxima dissonancia en termes de la criticalband.
  return A1 * A2 * x * exp(1 - x);
}

double disso_complexa(int harmonics_F1, int harmonics_F2, double F1[harmonics_F1][2], double F2[harmonics_F2][2]) { //llista d'harmonics. A la primera columna la freqüència; a la segona,  l'amplitud.
  double dissoF1 = 0, dissoF2 = 0, dissoF1_F2 = 0;
  for (int i = 0; i < harmonics_F1; i++) {
    for (int j = i; j < harmonics_F1; j++) //nomes hem de comptar els termes (i, j) una vegada.
      dissoF1 += disso_simple(F1[i][0], F1[j][0], F1[i][1], F1[j][1]);
  }
  for (int i = 0; i < harmonics_F2; i++) {
    for (int j = i; j < harmonics_F2; j++) //nomes hem de comptar els termes (i, j) una vegada.
      dissoF2 += disso_simple(F2[i][0], F2[j][0], F2[i][1], F2[j][1]);
  }
  for (int i = 0; i < harmonics_F1; i++) {
    for (int j = 0; j < harmonics_F2; j++)
      dissoF1_F2 += disso_simple(F1[i][0], F2[j][0], F1[i][1], F2[j][1]);
  }
  return dissoF1 + dissoF2 + dissoF1_F2;
}
