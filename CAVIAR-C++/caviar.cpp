#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <math.h>
#include <unistd.h> 

#include "Util.h"
#include "PostCal.h"
#include "TopKSNP.h"

using namespace std;

int main( int argc, char *argv[]  ){
	int tmpSize = 0;
	int totalCausalSNP = 2;
	int snpCount  = 0;
	double NCP = 5.7;
	double rho = 0;
	double * sigma;
	double * stat;
	char * configure;
	int * rank;
	bool histFlag = false;
	string * snpNames;
	int oc = 0;	
	string ldFile = "";
	string zFile  = "";
	string outputFileName = "";
	string geneMapFile = "";	

	while ((oc = getopt(argc, argv, "vhl:o:z:r:c:f:")) != -1) {
		switch (oc) {
			case 'v':
				cout << "version 0.1" << endl;
			case 'h':
				cout << "Options: " << endl;
  				cout << "-h, --help            		show this help message and exit " << endl;
  				cout << "-o OUTFILE, --out=OUTFILE 	specify the output file" << endl;
  				cout << "-l LDFILE, --ld_file=LDFILE  	the ld input file" << endl;
  				cout << "-z ZFILE, --z_file=ZFILE	the z-score and rsID files" << endl;
  				cout << "-r RHO, --rho-prob=RHO		set $pho$ probability " << endl;
				cout << "-c causal			set the maximum number of causal SNPs" << endl;
				cout << "-f 1				to out the probaility of different number of causal SNP" << endl;
				exit(0);
			case 'l':
				ldFile = string(optarg);
				break;
			case 'o':
				outputFileName = string(optarg);
				break;
			case 'z':
				zFile = string(optarg);
				break;
			case 'r':
				rho = atof(optarg);
				break;
			case 'c':
				totalCausalSNP = atoi(optarg);
				break;
			case 'f':
                                histFlag = true;
                                break;
			case ':':
			case '?':
			default:
				cout << "Strange" << endl;
				break;
		}
	}
	
	//program is running
	cout << "@-------------------------------------------------------------@" << endl;
	cout << "| CAVIAR!		| 	   v0.1         |  29/Jul/2015 | " << endl;
	cout << "|-------------------------------------------------------------|" << endl;
	cout << "| (C) 2014 Farhad Hormozdiari, GNU General Public License, v2 |" << endl;
	cout << "|-------------------------------------------------------------|" << endl;
	cout << "| For documentation, citation & bug-report instructions:      |" << endl;
	cout << "| 		http://genetics.cs.ucla.edu/caviar/            |" << endl;
	cout << "@-------------------------------------------------------------@" << endl;	

	fileSize(ldFile, tmpSize);
	snpCount = (int)sqrt(tmpSize);
	
	sigma     = new double[snpCount * snpCount];
        stat      = new double[snpCount];
        configure = new char[snpCount];
        rank      = new int[snpCount];
	snpNames  = new string [snpCount];

	importData(ldFile, sigma);		
	for(int i = 0; i < snpCount*snpCount; i++){
                if(i%(snpCount+1) == 0)
                        sigma[i] = sigma[i] + 0.2;
        }

	importDataFirstColumn(zFile, snpNames);
	importDataSecondColumn(zFile, stat);

	PostCal post(sigma, snpCount, totalCausalSNP);
	post.findOptimalSetGreedy(stat, NCP, configure, rank, rho);

	ofstream outputFile;
	string outFileNameSet = string(outputFileName)+"_set";
	outputFile.open(outFileNameSet.c_str());
	for(int i = 0; i < snpCount; i++) {
		if(configure[i] == '1')
			outputFile << snpNames[i] << endl;
	}
	outputFile.close();

	// output the post values for each variant
	string outFileNamePost = string(outputFileName)+"_post";
	outputFile.open(outFileNamePost.c_str());
	for(int i = 0; i < snpCount; i++) {
	  outputFile << snpNames[i] << "\t" << post.postProbs[i] << endl;
	}
	outputFile.close();	

        //output the histogram data to file
        if(histFlag)
                post.printHist2File(string(outputFileName)+"_hist");
	
	return 0;
}
