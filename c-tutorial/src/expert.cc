#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include "TCut.h"
#include "TFile.h"
#include "TSystem.h"
#include "TTree.h"
#include "TH1F.h"

#include "NeuroBayesExpert.hh"

using namespace std;

int main (int argc, char** argv) {

	vector<string> ExpertiseFiles;
		cout << argc << endl;
	int nExpertises = 0;	

	vector<Expert* > NBExperts;
	TFile output("evaluationsPlots.root","recreate");
	vector <TH1F* > hists;
	if(argc>1)
		for (int iarg = 1; iarg < argc; iarg++) {
			ExpertiseFiles.push_back(argv[iarg]);
			NBExperts.push_back( new Expert(ExpertiseFiles.back().c_str()) );
			cout << "Will use expertise in file '"<< ExpertiseFiles.back() << "'" << endl;	
			hists.push_back( new TH1F( (ExpertiseFiles.back() + "_signal").c_str() ,"Diskriminator Distribution",101,-1, 1));
			hists.push_back( new TH1F( (ExpertiseFiles.back() + "_background").c_str() ,"Diskriminator Distribution",101,-1, 1));
			nExpertises++;
		}
	else 	
		cout << "Specify a file " << endl;

	TFile *input(0);
	TString fname = "data/testData.root";
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

	const int nvar = 10;
	float* InputArray = new float[nvar];
	for(int ivar=0; ivar< nvar; ivar++) {
		char varname[10];
		sprintf(varname,"var%d",ivar);
		InputTree->SetBranchAddress(varname, &InputArray[ivar]);
	}
	float target;
	InputTree->SetBranchAddress("target", &target);

	int maxEvents = InputTree->GetEntries();
	for(int ievent=0; ievent< maxEvents; ievent++) {
		int ientry = InputTree->GetEntry(ievent);
		for(int iExpert=0; iExpert< nExpertises; iExpert++) {
			if(target) hists[2*iExpert]->Fill(NBExperts[iExpert]->nb_expert(InputArray));
			else  hists[2*iExpert + 1]->Fill(NBExperts[iExpert]->nb_expert(InputArray));
		}
	}

	input->Close();
	output.Write();
	output.Close();
}

