#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int main(){
    double a, b, c, d, e, f;
    FILE *fitxer;
    fitxer = fopen("diss.txt","r");
    if (fitxer == NULL){
        printf("No s'ha pogut llegir el fitxer.\n");
        return 1;
    }
    while (EOF != fscanf(fitxer, "%lf %lf %lf %lf %lf %lf", &a, &b, &c, &d, &e, &f)) {
        double h = fabs(c - a);
        double ddf = (f - 2 * d + b)/(h * h);
        printf("Derivada 2 en r = %.16G: %.16G\n", c, ddf);
    }

    return 0;
}