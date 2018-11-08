#include "hmm.h"
#include <math.h>

#define MAX_TIME	50
#define	MAX_STATE	10
#define	MAX_OBSERV	26
#define	MAX_SEQ		200
#define MAX_LINE 	256

int main(int argc, char *argv[]){
	HMM hmm;
	loadHMM(&hmm, argv[2]);

	int N = atoi(argv[1]);
	while (N --){
		FILE *fp = open_or_die(argv[3], "r");

		int t = 0;
		char contents[56] = {'\0'};
		double tmp_initial[MAX_STATE] = {0};
		double tmp_summation[MAX_STATE] = {0};
		double tmp_transition[MAX_STATE][MAX_STATE] = {{0}};
		double tmp_observation[MAX_OBSERV][MAX_STATE] = {{0}};
		while (fscanf(fp, "%s", contents) > 0){
			// initialize and calculate for array "alpha"
			double alpha[MAX_TIME][MAX_STATE] = {{0}};
			for (int i = 0; i < hmm.state_num; i ++)
				alpha[0][i] = hmm.initial[i] * hmm.observation[contents[0] - 'A'][i];

			t = 0;
			while (t < 49){
				for (int k = 0; k < hmm.state_num; k ++){
					double tmp = 0.0;
					for (int i = 0; i < hmm.state_num; i ++)
						tmp += alpha[t][i] * hmm.transition[i][k];

					alpha[t + 1][k] = tmp * hmm.observation[contents[t + 1] - 'A'][k];
				}
				t ++;
			}

			// // print alpha
			// printf("========================alpha========================\n");
			// t = 0;
			// while (t < 50){
			// 	for (int k = 0; k < hmm.state_num; k ++)
			// 		printf("%.5f ", alpha[t][k]);
			// 	printf("\n");
			// 	t ++;
			// }
			// printf("=====================================================\n");

			// initialize and calculate for array "beta"
			double beta[MAX_TIME][MAX_STATE] = {{0}};
			for (int j = 0; j < hmm.state_num; j ++)
				beta[49][j] = 1.0;

			t = 49;
			while (t > 0){
				for (int k = 0; k < hmm.state_num; k ++){
					double tmp = 0.0;
					for (int j = 0; j < hmm.state_num; j ++)
						tmp += beta[t][j] * hmm.transition[k][j] * hmm.observation[contents[t] - 'A'][j];

					beta[t - 1][k] = tmp;
				}
				t --;
			}

			// // print beta
			// printf("========================beta=========================\n");
			// t = 50;
			// while (t >= 0){
			// 	for (int k = 0; k < hmm.state_num; k ++)
			// 		printf("%.5f ", beta[t][k]);
			// 	printf("\n");
			// 	t --;
			// }
			// printf("=====================================================\n");

			// calcalate for array "gamma"
			double gamma[MAX_TIME][MAX_STATE] = {{0}};
			t = 0;
			while (t < 50){
				double tmp = 0.0;
				for (int k = 0; k < hmm.state_num; k ++)
					tmp += alpha[t][k] * beta[t][k];

				for (int k = 0; k < hmm.state_num; k ++){
					gamma[t][k] = alpha[t][k] * beta[t][k] / tmp;
					
					// update the result of array "initial"
					if (t == 0) tmp_initial[k] += gamma[t][k];

					// update the result of array "summation"
					tmp_summation[k] += gamma[t][k];

					// update the result of array "observation"
					tmp_observation[contents[t] - 'A'][k] += gamma[t][k];
				}
				t ++;
			}

			// // print gamma
			// printf("========================gamma========================\n");
			// t = 0;
			// while (t < 50){
			// 	for (int k = 0; k < hmm.state_num; k ++)
			// 		printf("%.5f ", gamma[t][k]);
			// 	printf("\n");
			// 	t ++;
			// }
			// printf("=====================================================\n");

			// calculate for array "epsilon"
			double epsilon[MAX_TIME][MAX_STATE][MAX_STATE] = {{{0}}};
			t = 0;
			while (t < 49){
				double tmp = 0.0;
				for (int k1 = 0; k1 < hmm.state_num; k1 ++)
					for (int k2 = 0; k2 < hmm.state_num; k2 ++)
						tmp += alpha[t][k1] * hmm.transition[k1][k2] * beta[t + 1][k2] * hmm.observation[contents[t + 1] - 'A'][k2];

				for (int k1 = 0; k1 < hmm.state_num; k1 ++)
					for (int k2 = 0; k2 < hmm.state_num; k2 ++){
						epsilon[t][k1][k2] = alpha[t][k1] * hmm.transition[k1][k2] * beta[t + 1][k2] * hmm.observation[contents[t + 1] - 'A'][k2] / tmp;

						// update the result of array "transition"
						tmp_transition[k1][k2] += epsilon[t][k1][k2];
					}
				t ++;
			}
		}

		// update the "model list"
		for (int k = 0; k < hmm.state_num; k ++)
			hmm.initial[k] = tmp_initial[k] / 10000;

		for (int k = 0 ; k < hmm.state_num; k ++)
			for (int i = 0 ; i < hmm.state_num; i ++)
				hmm.transition[k][i] = tmp_transition[k][i] / tmp_summation[k];

		for (int k = 0; k < hmm.observ_num; k ++)
			for (int j = 0; j < hmm.state_num; j ++)
				hmm.observation[k][j] = tmp_observation[k][j] / tmp_summation[j];

		fclose(fp);
	}

	FILE *fp = open_or_die(argv[4], "w");
	dumpHMM(fp, &hmm);
	fclose(fp);
	return 0;
}
