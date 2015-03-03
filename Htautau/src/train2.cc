#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cstdlib>

#include "TCut.h"
#include "TFile.h"
#include "TSystem.h"
#include "TTree.h"

#include "NeuroBayesTeacher.hh"

using namespace std;

/*
const int nXvalSamples=5;
const int nXvalThisPart=1;
*/
   
// ---------------------------------------------------------------
void teacher(bool Iterate) 
{

	bool Htautau = false;

   	std::cout << "Start NeuroBayes Setup" << std::endl
        	<< "======================" << std::endl
             	<< std::endl;

		
   	std::cout << "Reading Pseudocodegenfile in" << std::endl;

	string codegenfile = "config/pseudocodegen";
	if(Htautau)	codegenfile = "config/varFileList_Simon";
	ifstream codegen(codegenfile.c_str(), ifstream::in);
	if(codegen.bad()) {cout << "PseudoCodegen-file " << codegenfile << " not found " << endl;return;}
	
	string buffer;
	getline(codegen,buffer); //VARSET 1 auslesen
	//cout << buffer << endl;

	vector<string> varnames;
	char** c_varnames;
	std::vector<int> preproflags;

	//int iterator=0;
	while(true) {

		getline(codegen,buffer); 

		unsigned int name_end=buffer.find(" ");
		varnames.push_back( buffer.substr(0, name_end ));
		if(varnames.back() == "ENDVARSET") {varnames.pop_back(); break;}

		if(name_end < buffer.size() ) {
			string flag = buffer.substr(name_end, buffer.size());
			preproflags.push_back( atoi(flag.c_str()) );
		}
		else preproflags.push_back(0);
	}



	int nvar = varnames.size();
	//create NeuroBayes instance
  	NeuroBayesTeacher* nb = NeuroBayesTeacher::Instance();

	nb->NB_DEF_NODE1(nvar+1);       // nodes in input layer
  	nb->NB_DEF_NODE2(nvar+2);      	// nodes in hidden layer
  	nb->NB_DEF_NODE3(1);       	// nodes in output layer

  	nb->NB_DEF_TASK("CLA");    // binominal classification (default)

  	nb->NB_DEF_PRE(612);
  	nb->NB_DEF_REG("REG");           // 'OFF','REG' (def) ,'ARD','ASR','ALL'
  	nb->NB_DEF_LOSS("ENTROPY");      // 'ENTROPY'(def),'QUADRATIC'

  	nb->NB_DEF_LEARNDIAG( 1 );	   // BFGS

  	//nb->NB_DEF_EPOCH(200);           // weight update after n events

  	nb->NB_DEF_SPEED(1.0);           // multiplicative factor to enhance global learning speed
  	nb->NB_DEF_MAXLEARN(1.0);        // multiplicative factor to limit the global learning speed in any direction, this number should be smaller than NB_DEF_SPEED

	char ExpertiseFile[256];

	if(Iterate) {
  		nb->NB_DEF_ITER(100);            // number of training iteration
  		nb->NB_DEF_METHOD("BFGS");	   // bricht automatisch ab, wenn austrainiert
  	//	nb->NB_DEF_PRE(612);
  	//	nb->NB_DEF_SHAPE("DIAG");        // 'OFF', 'INCL', 'TOTL'
		sprintf(ExpertiseFile,"results/train2_iter_expertise.nb");
	}
	else {
  		nb->NB_DEF_ITER(0);            // number of training iteration
  		nb->NB_DEF_PRE(622);
		sprintf(ExpertiseFile,"results/train2_expertise.nb");
	}
	nb->NB_DEF_SHAPE("OFF");        // 'OFF', 'INCL', 'TOTL'
	cout << "Will put the Expertise in " << ExpertiseFile << endl;
  	nb->SetOutputFile(ExpertiseFile);  // expert file


  	int i= 4701;
  	int j=21;
  	nb->NB_RANVIN(i,j,2);            // random number seed initialisation, i has to be an odd number, the third argument is a debugging flag

	//Setup DataTree
	TFile *input(0);
      	TString fname = "input/Trainings_Data.root";
      	if(Htautau)	fname = "input/tauSignalAndBackground.root";
      	if (!gSystem->AccessPathName( fname )) {
         	std::cout << "--- NeuroBayesTeacher  : accessing " << fname << std::endl;
         	input = TFile::Open( fname );
      	}

	string nameTree = "tree";
	if(Htautau)	nameTree = "TauCheck";
      	TTree *InputTree = (TTree*)input->Get(nameTree.c_str());
	cout << "Tree accessed" << endl;

	float target = 2;
	InputTree->SetBranchAddress("target", &target);

	float lumi = 1.0;
	float weight = 0.0;
	float split = 0.0;
	if(Htautau){
	  InputTree->SetBranchAddress("lumiWeight", &lumi);
	  InputTree->SetBranchAddress("weight", &weight);
	  InputTree->SetBranchAddress("splitFactor", &split);
	}
	
	c_varnames = new char*[nvar];
	float* InputArray = new float[nvar];
	for(int ivar=0; ivar< nvar; ivar++) {
		InputTree->SetBranchAddress(varnames[ivar].c_str(), &InputArray[ivar]);
		if(preproflags[ivar] > 10)nb->SetIndividualPreproFlag(ivar, preproflags[ivar]);
		c_varnames[ivar] = new char[varnames[ivar].size()];
		strcpy(c_varnames[ivar], varnames[ivar].c_str());
	}

	cout << "Reading Events" << endl;
	int sigCount = 0, bkgCount=0;
	int maxEvents = InputTree->GetEntries();
	for(int ievent=0; ievent< maxEvents; ievent++) {
		int ientry = InputTree->GetEntry(ievent);
		if(Htautau){	nb->SetWeight(lumi*weight*split);}
		else{ 		nb->SetWeight(1.0); } //set weight of event
		if(target) nb->SetTarget(1.0), sigCount++; // event is a SIGNAL event
		else  nb->SetTarget(-1.0), bkgCount++; // event is a BKG event
		nb->SetNextInput(nvar,InputArray);
	}
	cout << endl;
	cout << "\t #Signal \t " << sigCount << endl;
	cout << "\t #Backgroud \t " << bkgCount << endl;

	//perform training
	cout << "To see NeuroBayes output have a look at \"results/train2_nb_teacher.log\"" << endl;
	int original = dup(fileno(stdout));
   	fflush(stdout);
   	freopen("nb_teacher.log", "w", stdout);

	nb->TrainNet();

	fflush(stdout);
   	dup2(original, fileno(stdout));
   	close(original);
	input->Close();

	nb->nb_correl_signi(c_varnames,"./results/train2_correl_signi.txt","./results/train2_correl_signi.html");
}

int main(int argc, char** argv) {
	if(argc<2) {
		cout << "If you want to iterate in this training call program with option \"1\" " << endl
			<< "  " << argv[0] << " 1" << endl;
			teacher(0);
		system("mv ahist.txt results/train2_ahist.txt");
		system("mv nb_teacher.log results/train2_nb_teacher.log");
	}
	else if(argc>=2) {
		if(atoi(argv[1]) == 1) {
			cout << "Will iterate! " << endl;
			teacher(1);
		system("mv ahist.txt results/train2_iter_ahist.txt");
		system("mv rescue.nb results/train2_iter_rescue.nb");
		system("mv nb_teacher.log results/train2_iter_nb_teacher.log");
		}
		else teacher(0);
	}
}
