#include <iostream>
#include <string>
#include <fstream>
#include <vector>

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
int teacher() 
{
   	std::cout << "Start NeuroBayes Setup" << std::endl
        	<< "======================" << std::endl
             	<< std::endl;

	int nvar = 10;
	//create NeuroBayes instance
  	NeuroBayesTeacher* nb = NeuroBayesTeacher::Instance();

	nb->NB_DEF_NODE1(nvar+1);       // nodes in input layer
  	nb->NB_DEF_NODE2(nvar+2);      	// nodes in hidden layer
  	nb->NB_DEF_NODE3(1);       	// nodes in output layer

 	nb->NB_DEF_ITER(100);            // number of training iteration
  	nb->NB_DEF_METHOD("BFGS");	 // will stop automatically if there's nothing more to learn
  	nb->NB_DEF_LEARNDIAG( 1 );	   // BFGS
  	nb->NB_DEF_PRE(612);		 // flatten input variables, de-correlate input variables and rotate all linear dependence with target to the first new input variable, i.e. X(2)

	char ExpertiseFile[256];
	sprintf(ExpertiseFile,"train1_expertise.nb");

	cout << "Will put the Expertise in " << ExpertiseFile << endl;
  	nb->SetOutputFile(ExpertiseFile);  // expert file
	
	//Setup DataTree
	TFile *input(0);
      	TString fname = "data/Trainings_Data.root";
      	if (!gSystem->AccessPathName( fname )) {
         	std::cout << "--- NeuroBayesTeacher  : accessing " << fname << std::endl;
         	input = TFile::Open( fname );
      	}

      	TTree *InputTree     = (TTree*)input->Get("tree");
	cout << "Tree accessed" << endl;

	float target, var0;
	InputTree->SetBranchAddress("target", &target);
	
	float* InputArray = new float[nvar];
	for(int ivar=0; ivar< nvar; ivar++) {
		char varname[10];
		sprintf(varname,"var%d",ivar);
		InputTree->SetBranchAddress(varname, &InputArray[ivar]);
	}

	cout << "Reading Events" << endl;
	int sigCount = 0, bkgCount=0;
	int maxEvents = InputTree->GetEntries();
	for(int ievent=0; ievent< maxEvents; ievent++) {
		int ientry = InputTree->GetEntry(ievent);
		nb->SetWeight(1.0);  //set weight of event
		if(target) nb->SetTarget(1.0), sigCount++; // event is a BACKGROUND event
		else  nb->SetTarget(-1.0), bkgCount++; // event is a SIGNAL event
		nb->SetNextInput(nvar,InputArray);
	}
	cout << endl;
	cout << "\t #Signal \t " << sigCount << endl;
	cout << "\t #Backgroud \t " << bkgCount << endl;

	//perform training
	cout << "To see NeuroBayes output have a look at \"nb_teacher.log\"" << endl;
	//stdout is redirected to the file "nb_teacher.log"
	int original = dup(fileno(stdout));
   	fflush(stdout);
   	freopen("nb_teacher.log", "w", stdout);

	nb->TrainNet();

	fflush(stdout);
   	dup2(original, fileno(stdout));
   	close(original);
	input->Close();
}

int main(int argc, char** argv) {
	teacher();
}
