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

using namespace std;

string outputDir = "results/";

bool checkFileExistence(string FileName){
  ifstream codegen(FileName.c_str(), ifstream::in);

  if(!codegen.is_open()) {
    cout << "PseudoCodegen-file " << FileName << " not found " << endl;
    return false;
  } else {
    return true;
  }
  
  return false;
} 

// ---------------------------------------------------------------
void teacher(string varFile, string optionFile, string inputFile_sig, string inputFile_bkg, bool usingSimonConf = false) 
{

  if(!checkFileExistence(varFile) || !checkFileExistence(optionFile) ||
     !checkFileExistence(inputFile_sig) || !checkFileExistence(inputFile_bkg) )  
    return;

  std::cout << "=========================" << std::endl
            << "Start NeuroBayes Training" << std::endl
            << std::endl;


  //-----		
  //Read Info from Congif Files && setting the NeuroBayesTeacher
  map<string, int> VarProProFlagsMap;
  VarProProFlagsMap = readVarFile(varFile, true);

  char** c_varnames;
  int nvar = VarProProFlagsMap.size();

  NeuroBayesTeacher* nb = NeuroBayesTeacher::Instance();

  nb->NB_DEF_NODE1(nvar+1);	// nodes in input layer
  nb->NB_DEF_NODE2(nvar+2);	// nodes in hidden layer
  nb->NB_DEF_NODE3(1);     	// nodes in output layer

  string output = outputDir +  "trainHtt" + DefineNBFeatures(nb, optionFile);
  if(usingSimonConf) output += "_Simon"; 

  //-----
  //Setting the Expertise File
  string ExpertiseFile = output + "_expertise.nb";
  cout << "Will put the Expertise in " << ExpertiseFile << endl;
  nb->SetOutputFile(ExpertiseFile.c_str());  

  //-----
  //Random seed number initialisation
  int i=4701;
  int j=21;
  nb->NB_RANVIN(i,j,2);            //i has to be an odd number, the third argument is a debugging flag

  //-----
  //Setup DataTree
  TFile *input(0);
  TString fname = "input/tauSignalAndBackground.root";
  //TString fname = "input/Trainings_Data.root";
  if (!gSystem->AccessPathName( fname )) {
    std::cout << "--- NeuroBayesTeacher  : accessing " << fname << std::endl;
    input = TFile::Open( fname );
  }

  TTree *InputTree = (TTree*)input->Get("TauCheck");
  //TTree *InputTree = (TTree*)input->Get("tree");
  if( InputTree->GetEntries() == 0 )
  {
    std::cout << "Error!! at least one file is empty" << std::endl;
    return;
  }

  float target = 2;
  InputTree->SetBranchAddress("target", &target);
  float lumi = 0.0;
  float weight = 0.0;
  float split = 0.0;

  InputTree->SetBranchAddress("lumiWeight", &lumi);
  InputTree->SetBranchAddress("weight", &weight);
  InputTree->SetBranchAddress("splitFactor", &split);

  //single variables var
  c_varnames = new char*[nvar];
  float* InputArray = new float[nvar];
  int ivar = 0;
  for(std::map<string,int>::iterator it = VarProProFlagsMap.begin(); it != VarProProFlagsMap.end(); ++it) {
    InputTree->SetBranchAddress(it->first.c_str(), &InputArray[ivar]);
    if(it->second > 10) {
      nb->SetIndividualPreproFlag(ivar, it->second, it->first.c_str());
    } else {
      unsigned int preproGlobal = output.find("_PRE");
      int preproFlagfixed = atoi(output.substr(preproGlobal+5, 2).c_str());
      nb->SetIndividualPreproFlag(ivar,preproFlagfixed,it->first.c_str());
    }
    c_varnames[ivar] = new char[it->first.size()];
    strcpy(c_varnames[ivar], it->first.c_str());
    ivar++;
  }


  //sig&bkg events :: NOT DIVIDE
  int sigCount = 0, bkgCount=0;
  int maxEvents = InputTree->GetEntries();
  cout << "\t Reading Events:" << maxEvents << endl;
  for(int ievent=0; ievent< maxEvents; ievent++) {
    int ientry = InputTree->GetEntry(ievent);
    if(ientry > 0){
      nb->SetWeight(lumi*weight*split);
//      nb->SetWeight(1.0);
      if(target){
        nb->SetTarget(1.0);
        sigCount++; // event is a SIGNAL event
      } else {
        nb->SetTarget(-1.0);
        bkgCount++; // event is a BKG event
      }
      nb->SetNextInput(nvar,InputArray);
    } else {
      std::cout << "Entry " << ientry << " does not exist" << std::endl;
      continue;
    }
  }

  cout << endl;
  cout << "\t #Signal \t " << sigCount << endl;
  cout << "\t #Backgroud \t " << bkgCount << endl;


  //perform training
  string LogName = output + "_nb_teacher.log";
  cout << "To see NeuroBayes output have a look at " << LogName << endl;
  int original = dup(fileno(stdout));
  fflush(stdout);
  freopen(LogName.c_str(), "w", stdout);

  nb->TrainNet();

  fflush(stdout);
  dup2(original, fileno(stdout));
  close(original);

  string CorrelNameTxt  = output + "_correl_signi.txt";
  string CorrelNameHtml = output + "_correl_signi.html";
  nb->nb_correl_signi(c_varnames,CorrelNameTxt.c_str(),CorrelNameHtml.c_str());

  string ahistName = "mv ahist.txt " + output + "_ahist.txt";
  system(ahistName.c_str());
  string rescueName = "mv rescue.nb " + output + "_rescue.nb";
  system(rescueName.c_str());

}



int main(int argc, char** argv) {


  if(argc<5 || argc>5) {

    if(argc==1){
      cout << "Running with default options files" << endl;
      cout << "config/varFileList_Simon, config/optionList_Simon, config/inputFileList_Htt_train_sig, config/inputFileList_Htt_train_bkg" << endl;
      teacher("config/varFileList_Simon","config/optionList_Simon","config/inputFileList_Htt_train_sig","config/inputFileList_Htt_train_bkg",1);
    } else {
      cerr << "Number of arguments not correct." << endl;
      cerr << "You should give in input the following input files: " << endl;
      cerr << "\t variables config file " << endl;
      cerr << "\t options config file " << endl;
      cerr << "\t input data signal config file " << endl;
      cerr << "\t input data background config file " << endl;
      return 0;
    }

  } else if (argc==5) {

    string varFile = argv[1];
    string optionFile = argv[2];
    string inputFile_sig = argv[3];
    string inputFile_bkg = argv[4];

    teacher(varFile,optionFile,inputFile_sig,inputFile_bkg);

  } 

  return 0;

}
