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
   
// ---------------------------------------------------------------
int main() 
{
   	std::cout << "Start NeuroBayes Setup" << std::endl
        	<< "======================" << std::endl
             	<< std::endl;

	int nvar = 10;
	//create NeuroBayes instance
  	NeuroBayesTeacher* nb = NeuroBayesTeacher::Instance();

	//Define how much nodes should be in each layer. 
	nb->NB_DEF_NODE1(nvar+1);       // nodes in input layer, has to be (#Number of inputvariables) + 1
  	nb->NB_DEF_NODE2(nvar+2);      	// nodes in hidden layer, is in principel free
  	nb->NB_DEF_NODE3(1);       	// nodes in output layer, always one for classification


	char ExpertiseFile[256];
	sprintf(ExpertiseFile,"results/train_simplest_expertise.nb");

	cout << "Will put the Expertise in " << ExpertiseFile << endl;
  	nb->SetOutputFile(ExpertiseFile);  // Define expert file

	//Setup DataTree
	TFile *input(0);
      	TString fname = "data/Trainings_Data.root";
      	if (!gSystem->AccessPathName( fname )) {
         	std::cout << "--- NeuroBayesTeacher  : accessing " << fname << std::endl;
         	input = TFile::Open( fname );
      	}
	else {
		cout << fname << " could not be found, please generate it, or copy it to this folder " << endl;
		return 1;
	}

      	TTree *InputTree     = (TTree*)input->Get("tree");
	cout << "Tree accessed" << endl;

	float target;
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

		if(target) 	nb->SetTarget(1.0), sigCount++; // event is a BACKGROUND event
		else  		nb->SetTarget(-1.0), bkgCount++; // event is a SIGNAL event

		nb->SetNextInput(nvar,InputArray);
	}
	cout << endl;
	cout << "\t #Signal \t " << sigCount << endl;
	cout << "\t #Backgroud \t " << bkgCount << endl;
	
	//perform training
	nb->TrainNet(); //tell NeuroBayes to train the net

	input->Close();

	system("mv ahist.txt results/train_simplest_ahist.txt");
	system("mv rescue.nb results/train_simplest_rescue.nb");
}
