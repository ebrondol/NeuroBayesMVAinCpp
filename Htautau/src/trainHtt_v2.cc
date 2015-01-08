#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include "TChain.h"
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

  int nvar = 12;
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
  sprintf(ExpertiseFile,"results/trainHtt_v2_expertise.nb");

  cout << "Will put the Expertise in " << ExpertiseFile << endl;
  nb->SetOutputFile(ExpertiseFile);  // expert file
	
  //Setup DataTree
  cout << "Reading Signal and Bkg Tree" << endl;

  TString dir = "Input/";
  std::vector<TString> SignalFiles;
  std::vector<TString> BkgFiles;

  SignalFiles.push_back(dir+"train_mvain_mu_sync_vbfhiggs_0.root");
  SignalFiles.push_back(dir+"train_mvain_mu_sync_vbfhiggs_norecoil_0.root");

  BkgFiles.push_back(dir+"train_mvain_mu_sync_dy1j_0.root");
  BkgFiles.push_back(dir+"train_mvain_mu_sync_dy2j_0.root");
  BkgFiles.push_back(dir+"train_mvain_mu_sync_dy3j_0.root");
  BkgFiles.push_back(dir+"train_mvain_mu_sync_dy4j_0.root");

  std::cout << "Input files:" << std::endl;
  TChain* ntu_Sig = new TChain("TauCheck");
  for(unsigned int i = 0; i < SignalFiles.size(); i++){
    ntu_Sig -> Add(SignalFiles.at(i));
    std::cout << "\t" << SignalFiles.at(i) << " added at the Sig chain." << std::endl;
  }
  TChain* ntu_Bkg = new TChain("TauCheck");
  for(unsigned int i = 0; i < BkgFiles.size(); i++){
    ntu_Bkg -> Add(BkgFiles.at(i));
    std::cout << "\t" << BkgFiles.at(i) << " added at the Bkg chain." << std::endl;
  }

  if( ntu_Sig->GetEntries() == 0 || ntu_Bkg->GetEntries() == 0 )
  {
    std::cout << "Error!! at least one file is empty" << std::endl;
    return 0;
  }



/*	TFile *input(0);
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
*/


  float* InputArray = new float[nvar];
  ntu_Sig->SetBranchAddress("svfit_mass", &InputArray[0]);
  ntu_Sig->SetBranchAddress("dr_leptau", &InputArray[1]);
  ntu_Sig->SetBranchAddress("jdeta", &InputArray[2]);
  ntu_Sig->SetBranchAddress("mjj", &InputArray[3]);
  ntu_Sig->SetBranchAddress("jeta1eta2", &InputArray[4]);
  ntu_Sig->SetBranchAddress("pt_tot", &InputArray[5]);
  ntu_Sig->SetBranchAddress("met_centrality", &InputArray[6]);
  ntu_Sig->SetBranchAddress("mt_1", &InputArray[7]);
  ntu_Sig->SetBranchAddress("lep_etacentrality", &InputArray[8]);
  ntu_Sig->SetBranchAddress("pt_sum", &InputArray[9]);
  ntu_Sig->SetBranchAddress("sphericity", &InputArray[10]);
  ntu_Sig->SetBranchAddress("mvis", &InputArray[11]);

/*
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
*/

        cout << "Reading Signal Events" << endl;
        int sigCount = 0, bkgCount=0;
        int maxEvents = ntu_Sig->GetEntries();
        for(int ievent=0; ievent< maxEvents; ievent++) {
                int ientry = ntu_Sig->GetEntry(ievent);
                nb->SetWeight(1.0);
                nb->SetTarget(1.0);
                sigCount++;
                nb->SetNextInput(nvar,InputArray);
        }
        cout << "\t #Signal \t " << sigCount << endl;

        maxEvents = ntu_Bkg->GetEntries();
        for(int ievent=0; ievent< maxEvents; ievent++) {
                int ientry = ntu_Bkg->GetEntry(ievent);
                nb->SetWeight(1.0);
                nb->SetTarget(-1.0);
                bkgCount++;
                nb->SetNextInput(nvar,InputArray);
        }
        cout << "\t #Backgroud \t " << bkgCount << endl;


	//perform training
	cout << "To see NeuroBayes output have a look at \"results/trainHtt_v2_nb_teacher.log\"" << endl;
	//stdout is redirected to the file "nb_teacher.log"
	int original = dup(fileno(stdout));
   	fflush(stdout);
   	freopen("nb_teacher.log", "w", stdout);

	nb->TrainNet();

	fflush(stdout);
   	dup2(original, fileno(stdout));
   	close(original);
	//input->Close();
}

int main(int argc, char** argv) {
	teacher();
	system("mv ahist.txt results/trainHtt_v2_ahist.txt");
	system("mv rescue.nb results/trainHtt_v2_rescue.nb");
	system("mv nb_teacher.log results/trainHtt_v2_nb_teacher.log");
}
