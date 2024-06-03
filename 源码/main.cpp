#include<iostream>
#include<fstream>

#include "PageRank.h"

using namespace std;

double beta = 0.85;
double epsilon = 1e-5;
int block_num = 4;

char input_filename[20] = "Data.txt";
char output_filename[20] = "Result.txt";
char output_filename_all[20] = "Result_all.txt";

int main() {
	PageRank pagerank(beta, epsilon, block_num, input_filename,  output_filename, output_filename_all);

	//pagerank.rank_base();

	pagerank.rank_block();

	pagerank.mysort();

	pagerank.write_to_result();

	return 0;
	system("pause");
}