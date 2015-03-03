void addtree(){
  TChain* ntu = new TChain("tree");
  ntu->Add("data/Trainings_Data_bkg.root");
  ntu->Add("data/Trainings_Data_sig.root");
  ntu->Merge("big.root");
}

void copytree(bool isSignal, string nameTreeOld) {
   TFile *f = new TFile(nameTreeOld.c_str(),"update");
   TTree *T = (TTree*)f->Get("TauCheck");
   float target;
   TBranch *bpt = T->Branch("target",&target,"target/F");
   Long64_t nentries = T->GetEntries();
   for (Long64_t i=0;i<nentries;i++) {
      T->GetEntry(i);
      if(isSignal) target = 1;
      else         target = 0;
      bpt->Fill();
   }
   T->Print();
   T->Write();
   delete f;
}

void produceTrees()
{
  copytree(true,"data/train_mvain_mu_sync_vbfhiggs_0.root");
  copytree(false,"data/test_mvain_mu_sync_dy1j_0.root");
  copytree(false,"data/test_mvain_mu_sync_dy2j_0.root");
  copytree(false,"data/test_mvain_mu_sync_dy3j_0.root");
  copytree(false,"data/test_mvain_mu_sync_dy4j_0.root");

  TChain* ntu = new TChain("TauCheck");
  ntu->Add("data/train_mvain_mu_sync_vbfhiggs_0.root");
  ntu->Add("data/test_mvain_mu_sync_dy1j_0.root");
  ntu->Add("data/test_mvain_mu_sync_dy2j_0.root");
  ntu->Add("data/test_mvain_mu_sync_dy3j_0.root");
  ntu->Add("data/test_mvain_mu_sync_dy4j_0.root");
  ntu->Merge("data/tauSignalAndBackground.root");

}
