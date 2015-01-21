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

#include "NeuroBayesTeacher.hh"
#include "NeuroBayesExternalFunction.hh"
#include "ntpleUtils.h"

using namespace std;

string varFile = "config/varFileList";
string optionFile = "config/optionList";
string inputFile_sig = "config/inputFileList_Htt_train_sig";
string inputFile_bkg = "config/inputFileList_Htt_train_bkg";

/*
const int nXvalSamples=5;
const int nXvalThisPart=1;
*/
   
// ---------------------------------------------------------------
void teacher(bool Iterate = 1) 
{


  std::cout << "=========================" << std::endl
            << "Start NeuroBayes Training" << std::endl
            << std::endl;


  //-----		
  //Read Info from Congif Files && setting the NeuroBayesTeacher
  map<string, int> VarProProFlagsMap;
  VarProProFlagsMap = readVarFile(varFile, false);
  string OptionValue;
  const char * OptionValue_char;
  int OptionValue_int;

  char** c_varnames;
  int nvar = VarProProFlagsMap.size();

  NeuroBayesTeacher* nb = NeuroBayesTeacher::Instance();

  nb->NB_DEF_NODE1(nvar+1);	// nodes in input layer
  nb->NB_DEF_NODE2(nvar+2);	// nodes in hidden layer
  nb->NB_DEF_NODE3(1);     	// nodes in output layer

  // kind of classification
  OptionValue = readStringOptionFromFile(optionFile, "DEF_TASK");
  OptionValue_char = OptionValue.c_str();
  nb->NB_DEF_TASK(OptionValue_char);

  // preprocessing global flag
  OptionValue = readStringOptionFromFile(optionFile, "DEF_PRE");
  OptionValue_int = atoi(OptionValue.c_str());
  nb->NB_DEF_PRE(OptionValue_int);

  // energy loss function : ENTROPY, QUADRATIC
  OptionValue = readStringOptionFromFile(optionFile, "DEF_LOSS");
  OptionValue_char = OptionValue.c_str();
  nb->NB_DEF_LOSS(OptionValue_char);

  //nb->NB_DEF_LEARNDIAG( 1 );	   // BFGS
  //nb->NB_DEF_EPOCH(200);           // weight update after n events

  nb->NB_DEF_SPEED(1.0);           // multiplicative factor to enhance global learning speed
  nb->NB_DEF_MAXLEARN(2.0);        // multiplicative factor to limit the global learning speed in any direction, this number should be smaller than NB_DEF_SPEED

  char ExpertiseFile[256];

  if(Iterate) {
    nb->NB_DEF_ITER(220);            // number of training iteration
    nb->NB_DEF_SHAPE("INCL");        // 'OFF', 'INCL', 'TOTL', 'DIAG'
    nb->NB_DEF_REG("OFF");        // 'OFF', 'RED', 'ARD', 'ASR', 'ALL'
    nb->NB_DEF_METHOD("BFGS");	   // bricht automatisch ab, wenn austrainiert
    sprintf(ExpertiseFile,"results/trainHtt_iter_BFGS_expertise.nb");
  } else {
    nb->NB_DEF_ITER(0);            // number of training iteration
    nb->NB_DEF_SHAPE("DIAG");        // 'OFF', 'INCL', 'TOTL'
    nb->NB_DEF_PRE(622);
    sprintf(ExpertiseFile,"results/trainHtt_expertise.nb");
  }

  cout << "Will put the Expertise in " << ExpertiseFile << endl;
  nb->SetOutputFile(ExpertiseFile);  // expert file

  //-----
  //Random seed number initialisation
  int i= 4701;
  int j=21;
  nb->NB_RANVIN(i,j,2);            //i has to be an odd number, the third argument is a debugging flag

  //-----
  //Setup DataTree
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
    return;
  }


  c_varnames = new char*[nvar];
  float* InputArray = new float[nvar];
  int ivar = 0;
  for( std::map<string,int>::iterator it = VarProProFlagsMap.begin(); it != VarProProFlagsMap.end(); ++it) {
    if( it->second > 10)
      nb->SetIndividualPreproFlag(ivar, it->second, it->first.c_str());
    c_varnames[ivar] = new char[it->first.size()];
    strcpy(c_varnames[ivar], it->first.c_str());
    ivar++;
  }

  //----
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
      nb->SetWeight(lumi*weight*split);
      nb->SetTarget(1.0);
      sigCount++;
      nb->SetNextInput(nvar,InputArray);
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
      nb->SetWeight(lumi*weight*split);
      nb->SetTarget(0.0);
      bkgCount++;
      nb->SetNextInput(nvar,InputArray);
    } else {
      std::cout << "Entry " << ientry << " does not exist" << std::endl;
      continue;
    }
 }
 cout << "\t #Backgroud \t " << bkgCount << endl;


	//perform training
	if(Iterate)	cout << "To see NeuroBayes output have a look at \"results/trainHtt_iter_nb_teacher.log\"" << endl;
	if(!Iterate)	cout << "To see NeuroBayes output have a look at \"results/trainHtt_nb_teacher.log\"" << endl;
	int original = dup(fileno(stdout));
   	fflush(stdout);
   	freopen("nb_teacher.log", "w", stdout);

	nb->TrainNet();

	fflush(stdout);
   	dup2(original, fileno(stdout));
   	close(original);
	//input->Close();

	nb->nb_correl_signi(c_varnames,"./results/trainHtt_correl_signi.txt","./results/trainHtt_correl_signi.html");

}

int main(int argc, char** argv) {

  if(argc<2) {
    cout << "Default: iteration " << endl;
    teacher();
    system("mv ahist.txt results/trainHtt_iter_ahist.txt");
    system("mv rescue.nb results/trainHtt_iter_rescue.nb");
    system("mv nb_teacher.log results/trainHtt_iter_nb_teacher.log");
  } else if(argc>=2) {
    if(atoi(argv[1]) == 1) {
    cout << "With option 1 you are not iterated " << endl;
    teacher(0);
    system("mv ahist.txt results/trainHtt_ahist.txt");
    system("mv nb_teacher.log results/trainHtt_nb_teacher.log");
  }
  else teacher();
  }
}
