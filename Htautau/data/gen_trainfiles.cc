#include <iostream>
#include <TFile.h>
#include <TRandom.h>
#include <TH1F.h>
#include <TF1.h>
#include <TTree.h>
#include <vector>
#include <string>
#include <sstream>
std::string itos(int i)	// convert int to string
{
	std::stringstream s;
	s << i;
	return s.str();
}
void gen_trainfiles() {
	int nEvents = 100000;
	const int nVars = 10;
	
	TFile* traindata = new TFile("Trainings_Data.root","RECREATE");

	TRandom* targetGen = new TRandom(0);

	vector<TRandom*> RandVarGenerator;
	vector<TF1*> FuncVarGenerator;
	Float_t varcont[nVars];

	TTree* dataTree = new TTree("tree","Tree with MonteCarlo generated trainings data");
	for (int iVar = 0; iVar < nVars; iVar++) {
		RandVarGenerator.push_back(new TRandom(0));
		TString varname = "var" + itos(iVar);
		TString leafname = varname + "/F";
		dataTree->Branch(varname,&varcont[iVar],leafname);
	}
	float target = 0;
	dataTree->Branch("target",&target,"target/F");

	FuncVarGenerator.push_back(new TF1("f1","cos(x)+1",0,10));
	FuncVarGenerator.push_back(new TF1("f2","sin(2*x)+1",0,10));
	FuncVarGenerator.push_back(new TF1("f3","2*x*x-0.5*x*x*x*x",-2,2));
	FuncVarGenerator.push_back(new TF1("f4","sqrt(x)",-2,2));

	for (int iEvent = 0 ; iEvent < nEvents; iEvent++) {
		target = 0;
		if(targetGen->Uniform(1) > 0.6) target = 1;
		if(target) {
			varcont[0] = RandVarGenerator[0]->Gaus(-0.1,2);
			varcont[1] = RandVarGenerator[1]->Binomial(9,0.5);
			varcont[2] = RandVarGenerator[2]->Landau(2,5);
			varcont[3] = RandVarGenerator[3]->Gaus(-2,3)*FuncVarGenerator[2]->GetRandom();
			varcont[4] = FuncVarGenerator[0]->GetRandom();
			varcont[5] = FuncVarGenerator[0]->GetRandom() + FuncVarGenerator[2]->GetRandom();
			//varcont[6] = RandVarGenerator[3]->Gaus(0,1);
			varcont[6] = varcont[0] + RandVarGenerator[3]->Gaus(1,0.2);
			varcont[7] = FuncVarGenerator[3]->GetRandom() * RandVarGenerator[1]->Gaus(1,0.3) ;
			varcont[8] = (int)FuncVarGenerator[0]->GetRandom();
			varcont[9] = FuncVarGenerator[3]->GetRandom()*FuncVarGenerator[0]->GetRandom();

			if(targetGen->Uniform(1) > 0.8) varcont[9] = -999.;
		}
		else {
			varcont[0] = RandVarGenerator[0]->Gaus(1,2);
			varcont[1] = RandVarGenerator[1]->Binomial(9,0.7);
			varcont[2] = RandVarGenerator[2]->Landau(1,8);
			varcont[3] = RandVarGenerator[3]->Gaus(1,2) * FuncVarGenerator[2]->GetRandom();
			varcont[4] = FuncVarGenerator[1]->GetRandom();
			varcont[5] = FuncVarGenerator[0]->GetRandom() - FuncVarGenerator[2]->GetRandom();
			//varcont[6] = FuncVarGenerator[2]->GetRandom()*RandVarGenerator[1]->Gaus(1,0.4)*2;
			varcont[6] = varcont[0] + RandVarGenerator[3]->Gaus(1,0.4);
			varcont[7] = FuncVarGenerator[3]->GetRandom()*RandVarGenerator[3]->Gaus(1,0.4);
			varcont[8] = (int)FuncVarGenerator[1]->GetRandom();
			varcont[9] = FuncVarGenerator[3]->GetRandom()*FuncVarGenerator[1]->GetRandom();

			if(targetGen->Uniform(1) < 0.20) varcont[9] = -999.;
		}
		dataTree->Fill();
	}

	traindata->Write();
}
