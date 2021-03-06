#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>
#include "lib_aln_inexact_matching.h"

int main(int argc, char *argv[])
{
	clock_t begin, end;
	double time_spent;
	//Path where is locate database sequences in the FASTA format
	char* path = "/home/mattia/Desktop/Bisat/src/lib_aln_inexact_matching/data";

	//Reference genome inside test.fa
	const char reference_genome[] = "";

	/*
	 * The index is stored in the same directory of genome.
	 * All the files that make up the index will be prefixed with the name of
	 * the file containing the genome.
	 */
	char *prefix = path;

	//Create FMD-Index
	//lib_aln_index(path, prefix, BWTALGO_AUTO);

	//Pattern to search
	const char pattern_input[] = "TTTTTTTTTTTGTTTTTTTTTTTTGTTTTTTT";

	//Max number of mismatch between hit found by lib_aln_bound_backtracking method and input pattern
	const uint8_t nmismatch = 1;

	//Load FMD-index in memory
	bwaidx_t *idx = lib_aln_idx_load(prefix); // load the BWA index

	if (idx == NULL)
	{
		printf("Index load failed.\n");
		exit(EXIT_FAILURE);
	}

	//Number of hit found by lib_aln_bound_backtracking
	uint32_t numHit = 0;

	bool* pos = (bool *) malloc(sizeof(bool) * 32);
	for (int i = 0; i < 32; i++)
	{
		pos[i] = true;
	}

	/*
	 * Remember:
	 *
	 *For type search
	 *-ARBITRARY_HIT
	 *-ALL_HITS
	 *
	 *-For type output:
	 *-ALLOW_REV_COMP
	 *-NO_REV_COMP
	 */

	begin = clock();
	search_result** result = lib_aln_bound_backtracking(idx, pattern_input, nmismatch, pos, &numHit, ALL_HITS, ALLOW_REV_COMP);
	end = clock();
	time_spent = (double) (end - begin) / CLOCKS_PER_SEC;

	printf("\n*example.c*\n");

	printf("\nThe reference genome is: %s\n", reference_genome);
	printf("The maximum number of mismatches is: %" PRIu8 "\n", nmismatch);
	printf("Input pattern is: %s\n", pattern_input);

	//Print the search results on-screen
	if (numHit == 0)
		printf("No matches have been found that respect the input parameters.\n");
	else
	{
		for (int i = 0; i < numHit; i++)
		{
			printf("\nHit found---> %ld\n", result[i]->hit);
			printf("-This is present in position(s): ");

			for (int j = 0; j < result[i]->num_occur; j++)
			printf(" %" PRIu64, result[i]->positions_to_ref[j]);

			if (result[i]->is_rev_comp == false)
				printf("\n-Forward compared to the input string.\n");
			else
				printf("\n-Reverse complement compared to the input string.\n");

			printf("-Number of mismatch(es): %" PRIu8 "\n", result[i]->n_mismatches);

			if (result[i]->n_mismatches > 0)
			{
				printf("-Mismatch(es) in position(s):");
			for (int j = 0; j < result[i]->n_mismatches; j++)
			printf(" %" PRIu32, result[i]->different_positions[j]);
		}
	}
}

printf("Time: %f sec\n", time_spent);
printf("\n");
//free memory
lib_aln_sr_destroy(result, numHit);
lib_aln_idx_destroy(idx);
free(pos);
return 0;
}
