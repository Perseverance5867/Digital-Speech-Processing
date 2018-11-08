#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "hmm.h"

int main(int argc, char *argv[]){
	FILE *cor = open_or_die(argv[1], "r");
	FILE *cmp = open_or_die(argv[2], "r");

	double k = 2500, flag = 0;
	while (k --){
		char trash[20];
		int correct, compare;
		fscanf(cor, "model_0%d.txt\n", &correct);
		fscanf(cmp, "model_0%d.txt%s\n", &compare, trash);
		if (correct == compare) flag ++;
	}

	FILE *fp = open_or_die(argv[3], "w");
	fprintf(fp, "%f\n", flag / 2500);
}
