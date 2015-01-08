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
   
// ---------------------------------------------------------------
int main() 
{
  std::cout << "Start NeuroBayes Setup" << std::endl
  << "======================" << std::endl
  << std::endl;

  int nvar = 12;
  //create NeuroBayes instance
  NeuroBayesTeacher* nb = NeuroBayesTeacher::Instance();

  //Define how much nodes should be in each layer. 
  nb->NB_DEF_NODE1(nvar+1);       // nodes in input layer, has to be (#Number of inputvariables) + 1
  nb->NB_DEF_NODE2(nvar+2);      	// nodes in hidden layer, is in principel free
  nb->NB_DEF_NODE3(1);       	// nodes in output layer, always one for classification


  char ExpertiseFile[256];
  sprintf(ExpertiseFile,"results/trainHtt_v1_expertise.nb");

  cout << "Will put the Expertise in " << ExpertiseFile << endl;
  nb->SetOutputFile(ExpertiseFile);  // Define expert file

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


/*	cout << "Reading Events" << endl;i
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
	nb->TrainNet(); //tell NeuroBayes to train the net

	//input->Close();

	system("mv ahist.txt results/trainHtt_v1_ahist.txt");
	system("mv rescue.nb results/trainHtt_v1_rescue.nb");

  return 0;
}
