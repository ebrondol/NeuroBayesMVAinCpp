#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cstdlib>

#include "TChain.h"
#include "TCut.h"
#include "TFile.h"
#include "TSystem.h"
#include "TTree.h"
#include "TH1F.h"

#include "NeuroBayesExpert.hh"
#include "NeuroBayesExternalFunction.hh"

using namespace std;

string varFile = "config/varFileList";
string inputFile_sig = "config/inputFileList_Htt_test_sig";
string inputFile_bkg = "config/inputFileList_Htt_test_bkg";

// ---------------------------------------------------------------
int main (int argc, char** argv) {

  vector<string> ExpertiseFiles;
  string ExpFileName;
  //cout << argc << endl;
  int nExpertises = 0;	

  vector<Expert* > NBExperts;
  TFile output("results/HttEvaluationsPlots.root","recreate");
  vector <TH1F* > hists;
  if(argc>1) {
    for (int iarg = 1; iarg < argc; iarg++) {

      ExpertiseFiles.push_back(argv[iarg]);
      NBExperts.push_back( new Expert(ExpertiseFiles.back().c_str()) );
      cout << "Will use expertise in file '"<< ExpertiseFiles.back() << "'" << endl;	
      string str = argv[iarg];
      std::size_t pos = str.find("train");
      ExpFileName = str.substr(pos);     // get from "train" to the end

      hists.push_back( new TH1F( (ExpFileName + "_signal").c_str() ,"Diskriminator Distribution",101,-1, 1));
      hists.push_back( new TH1F( (ExpFileName + "_background").c_str() ,"Diskriminator Distribution",101,-1, 1));
      nExpertises++;
    }
  } else { 	
    cout << "Specify a file " << endl;
  }

  vector<TString> SignalFiles = readInputFile(inputFile_sig, false);
  vector<TString> BkgFiles = readInputFile(inputFile_bkg, false);

  std::cout << "Input files:" << std::endl;
  TChain* ntu_sig = new TChain("TauCheck");
  for(unsigned int i = 0; i < SignalFiles.size(); i++){
    ntu_sig -> Add(SignalFiles.at(i));
    std::cout << "\t" << SignalFiles.at(i) << " added at the Sig chain." << std::endl;
  }
  TChain* ntu_bkg = new TChain("TauCheck");
  for(unsigned int i = 0; i < BkgFiles.size(); i++){
    ntu_bkg -> Add(BkgFiles.at(i));
    std::cout << "\t" << BkgFiles.at(i) << " added at the Bkg chain." << std::endl;
  }

  if( ntu_sig->GetEntries() == 0 || ntu_bkg->GetEntries() == 0 )
  {
    std::cout << "Error!! at least one file is empty" << std::endl;
    return 0;
  }

  map<string, int> VarProProFlagsMap;
  VarProProFlagsMap = readVarFile(varFile, false);
  int nvar = VarProProFlagsMap.size();
  float* InputArray = new float[nvar];
  int ivar = 0;


  //Reading Events
  cout << "Reading Events:" << endl;
  int sigCount = 0, bkgCount=0;
  float lumi = 0.0;
  float weight = 0.0;
  float split = 0.0;

  //sig events
  ivar = 0;
  for(std::map<string,int>::iterator it = VarProProFlagsMap.begin(); it != VarProProFlagsMap.end(); ++it) {
    ntu_sig->SetBranchAddress(it->first.c_str(), &InputArray[ivar]);
    ivar++;
  }
  ntu_sig->SetBranchAddress("lumiWeight", &lumi);
  ntu_sig->SetBranchAddress("weight", &weight);
  ntu_sig->SetBranchAddress("splitFactor", &split);

  int maxEvents = ntu_sig->GetEntries();
  for(int ievent=0; ievent< maxEvents; ievent++) {
    int ientry = ntu_sig->GetEntry(ievent);
    if(ientry > 0){
      sigCount++;
      for(int iExpert=0; iExpert< nExpertises; iExpert++) {
        hists[2*iExpert]->Fill(NBExperts[iExpert]->nb_expert(InputArray), lumi*weight*split);
      }
    } else {
      std::cout << "Entry " << ientry << " does not exist" << std::endl;
      continue;
    }
  }
  cout << "\t #Signal \t " << sigCount << endl;


  //bkg events
  ivar = 0;
  for(std::map<string,int>::iterator it = VarProProFlagsMap.begin(); it != VarProProFlagsMap.end(); ++it) {
    ntu_bkg->SetBranchAddress(it->first.c_str(), &InputArray[ivar]);
    ivar++;
  }
  ntu_bkg->SetBranchAddress("lumiWeight", &lumi);
  ntu_bkg->SetBranchAddress("weight", &weight);
  ntu_bkg->SetBranchAddress("splitFactor", &split);

  maxEvents = ntu_bkg->GetEntries();
  for(int ievent=0; ievent< maxEvents; ievent++) {
    int ientry = ntu_bkg->GetEntry(ievent);
    if(ientry > 0){
      bkgCount++;
      for(int iExpert=0; iExpert< nExpertises; iExpert++) {
        hists[2*iExpert + 1]->Fill(NBExperts[iExpert]->nb_expert(InputArray), lumi*weight*split);
      }
    } else {
      std::cout << "Entry " << ientry << " does not exist" << std::endl;
      continue;
    }
  }
  cout << "\t #Background \t " << bkgCount << endl;

  output.Write();
  output.Close();

}

