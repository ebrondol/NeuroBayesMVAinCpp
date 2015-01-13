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

using namespace std;

/*
const int nXvalSamples=5;
const int nXvalThisPart=1;
*/
   
// ---------------------------------------------------------------
void teacher(bool Iterate = 1) 
{


   	std::cout << "Start NeuroBayes Setup" << std::endl
        	<< "======================" << std::endl
             	<< std::endl;

		
   	std::cout << "Reading Pseudocodegenfile in" << std::endl;

	string codegenfile = "variablesHtt";
	ifstream codegen(codegenfile.c_str(), ifstream::in);
	if(codegen.bad()) {cout << "PseudoCodegen-file " << codegenfile << " not found " << endl;return;}
	
	string buffer;
	getline(codegen,buffer); //STARTVARSET auslesen
	//cout << buffer << endl;

	vector<string> varnames;
	char** c_varnames;
	std::vector<int> preproflags;

	//int iterator=0;
  std::cout << "Variables: " << std::endl;
  while(true) {

    getline(codegen,buffer); 

    unsigned int name_end=buffer.find(" ");
    varnames.push_back( buffer.substr(0, name_end ));
    if(varnames.back() == "ENDVARSET") {varnames.pop_back(); break;}

    std::cout << varnames.back() << std::endl;

    if(name_end < buffer.size() ) {
      string flag = buffer.substr(name_end, buffer.size());
      preproflags.push_back( atoi(flag.c_str()) );
    }
    else preproflags.push_back(0);

  }



  int nvar = varnames.size();

  //create NeuroBayes instance
  NeuroBayesTeacher* nb = NeuroBayesTeacher::Instance();

  nb->NB_DEF_NODE1(nvar+1);	// nodes in input layer
  nb->NB_DEF_NODE2(nvar+2);	// nodes in hidden layer
  nb->NB_DEF_NODE3(1);     	// nodes in output layer

  nb->NB_DEF_TASK("CLA");    	// binominal classification (default)

  nb->NB_DEF_PRE(612);		//
  nb->NB_DEF_LOSS("ENTROPY");      // 'ENTROPY'(def),'QUADRATIC'

  	//nb->NB_DEF_LEARNDIAG( 1 );	   // BFGS

  	//nb->NB_DEF_EPOCH(200);           // weight update after n events

  	nb->NB_DEF_SPEED(1.0);           // multiplicative factor to enhance global learning speed
  	nb->NB_DEF_MAXLEARN(1.0);        // multiplicative factor to limit the global learning speed in any direction, this number should be smaller than NB_DEF_SPEED

	char ExpertiseFile[256];

	if(Iterate) {
  		nb->NB_DEF_ITER(100);            // number of training iteration
  		//nb->NB_DEF_METHOD("BFGS");	   // bricht automatisch ab, wenn austrainiert
		sprintf(ExpertiseFile,"results/trainHtt_v3_iter_expertise.nb");
	}
	else {
  		nb->NB_DEF_ITER(0);            // number of training iteration
  		nb->NB_DEF_SHAPE("DIAG");        // 'OFF', 'INCL', 'TOTL'
  		nb->NB_DEF_PRE(622);
		sprintf(ExpertiseFile,"results/trainHtt_v3_expertise.nb");
	}
	//nb->NB_DEF_SHAPE("DIAG");        // 'OFF', 'INCL', 'TOTL'
	cout << "Will put the Expertise in " << ExpertiseFile << endl;
  	nb->SetOutputFile(ExpertiseFile);  // expert file


  	int i= 4701;
  	int j=21;
  	nb->NB_RANVIN(i,j,2);            // random number seed initialisation, i has to be an odd number, the third argument is a debugging flag

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
    return;
  }

  c_varnames = new char*[nvar];
  float* InputArray = new float[nvar];
  for(int ivar=0; ivar< nvar; ivar++) {
    ntu_Sig->SetBranchAddress(varnames[ivar].c_str(), &InputArray[ivar]);
    if(preproflags[ivar] > 10)
      nb->SetIndividualPreproFlag(ivar, preproflags[ivar],varnames[ivar].c_str());
    c_varnames[ivar] = new char[varnames[ivar].size()];
    strcpy(c_varnames[ivar], varnames[ivar].c_str());
  }


        cout << "Reading Events" << endl;
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
                nb->SetTarget(0.0);
                bkgCount++;
                nb->SetNextInput(nvar,InputArray);
        }
        cout << "\t #Backgroud \t " << bkgCount << endl;


	//perform training
	if(Iterate)	cout << "To see NeuroBayes output have a look at \"results/trainHtt_v3_iter_nb_teacher.log\"" << endl;
	if(!Iterate)	cout << "To see NeuroBayes output have a look at \"results/trainHtt_v3_nb_teacher.log\"" << endl;
	int original = dup(fileno(stdout));
   	fflush(stdout);
   	freopen("nb_teacher.log", "w", stdout);

	nb->TrainNet();

	fflush(stdout);
   	dup2(original, fileno(stdout));
   	close(original);
	//input->Close();

	nb->nb_correl_signi(c_varnames,"./results/trainHtt_v3_correl_signi.txt","./results/trainHtt_v3_correl_signi.html");
}

int main(int argc, char** argv) {

  if(argc<2) {
    cout << "Default: iteration " << endl;
    teacher();
    system("mv ahist.txt results/trainHtt_v3_iter_ahist.txt");
    system("mv rescue.nb results/trainHtt_v3_iter_rescue.nb");
    system("mv nb_teacher.log results/trainHtt_v3_iter_nb_teacher.log");
  } else if(argc>=2) {
    if(atoi(argv[1]) == 1) {
    cout << "With option 1 you are not iterated " << endl;
    teacher(0);
    system("mv ahist.txt results/trainHtt_v3_ahist.txt");
    system("mv nb_teacher.log results/trainHtt_v3_nb_teacher.log");
  }
  else teacher();
  }
}
