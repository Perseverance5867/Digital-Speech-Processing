#include "hmm.h"
#include <math.h>

#define MAX_TIME	50
#define	MAX_STATE	10
#define	MAX_OBSERV	26
#define	MAX_SEQ		200
#define MAX_LINE 	256

int main(int argc, char *argv[]){
	HMM hmms[5];
	load_models(argv[1], hmms, 5);
	dump_models(hmms, 5);

	FILE *fp = open_or_die(argv[2], "r");
	FILE *res = open_or_die(argv[3], "w");

	int t = 0;
	char contents[56] = {'\0'};
	while (fscanf(fp, "%s", contents) > 0){

		int ans = 0, M = 5;
		double max = -1;
		while (M --){

			// initialize and calculate for array "delta"
			double delta[MAX_TIME][MAX_STATE] = {{0}};
			for (int k = 0; k < hmms[M].state_num; k ++)
				delta[0][k] = hmms[M].initial[k] * hmms[M].observation[contents[0] - 'A'][k];

			t = 0;
			while (t < 49){
				for (int k = 0; k < hmms[M].state_num; k ++){
					double tmp = -1;
					for (int i = 0; i < hmms[M].state_num; i ++)
						if (tmp < delta[t][i] * hmms[M].transition[i][k]) tmp = delta[t][i] * hmms[M].transition[i][k];

					delta[t + 1][k] = tmp * hmms[M].observation[contents[t + 1] - 'A'][k];
				}
				t ++;
			}

			// update the new maximum
			for (int k = 0; k < hmms[M].state_num; k ++)
				if (max < delta[49][k]) max = delta[49][k], ans = M + 1;
		}
		fprintf(res, "model_0%d.txt %e\n", ans, max);
		// printf("%d\n", ans);
	}
	fclose(fp);
	fclose(res);

	return 0;
}