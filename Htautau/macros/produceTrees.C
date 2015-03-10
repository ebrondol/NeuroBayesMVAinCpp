
void addtree(){
  TChain* ntu = new TChain("tree");
  ntu->Add("data/Trainings_Data_bkg.root");
  ntu->Add("data/Trainings_Data_sig.root");
  ntu->Merge("big.root");
}

void createCopyTreee(string nameTreeOld, string nameTreeNew){

  TFile *oldfile = new TFile(nameTreeOld.c_str(),"read");
  TTree *oldtree = (TTree*)oldfile->Get("TauCheck");

  //Create a clone of old tree in new file
  TFile *newfile = new TFile(nameTreeNew.c_str(),"recreate");
  TTree *T = oldtree->CloneTree();

  T->Write();

  delete oldfile;
  delete newfile;

}

void addTargetBranch(bool isSignal, string nameTree ) {

  TFile *f = new TFile(nameTree.c_str(),"read");
  TTree *T = (TTree*)f->Get("TauCheck");

  //check the existence of target branch
  TBranch* target_branch = T->FindBranch("target");
  if(target_branch) {
    cout << "target branch already exist into this tree." << endl;
    return;
  }

  float target;
  TBranch *bpt = T->Branch("target",&target,"target/F");
  Long64_t nentries = T->GetEntries();

  for (Long64_t i=0 ; i<nentries ; i++) {
     T->GetEntry(i);
     if(isSignal) target = 1;
     else         target = 0;
     bpt->Fill();
  }
  //T->Print();
  T->Write();

  cout << "added target branch to the tree." << endl;
  delete f;
}

void produceTrees()
{
  
  createCopyTreee("data/train_mvain_mu_sync_vbfhiggs_0.root", "data/train_mvain_mu_sync_vbfhiggs_0_new.root");
  createCopyTreee("data/train_mvain_mu_sync_dy1j_0.root", "data/train_mvain_mu_sync_dy1j_0_new.root");
  createCopyTreee("data/train_mvain_mu_sync_dy2j_0.root", "data/train_mvain_mu_sync_dy2j_0_new.root");
  createCopyTreee("data/train_mvain_mu_sync_dy3j_0.root", "data/train_mvain_mu_sync_dy3j_0_new.root");
  createCopyTreee("data/train_mvain_mu_sync_dy4j_0.root", "data/train_mvain_mu_sync_dy4j_0_new.root");

  addTargetBranch(true,"data/train_mvain_mu_sync_vbfhiggs_0_new.root");
  addTargetBranch(false,"data/train_mvain_mu_sync_dy1j_0_new.root");
  addTargetBranch(false,"data/train_mvain_mu_sync_dy2j_0_new.root");
  addTargetBranch(false,"data/train_mvain_mu_sync_dy3j_0_new.root");
  addTargetBranch(false,"data/train_mvain_mu_sync_dy4j_0_new.root");

  TChain* ntu = new TChain("TauCheck");
  ntu->Add("data/train_mvain_mu_sync_vbfhiggs_0_new.root");
  ntu->Add("data/train_mvain_mu_sync_dy1j_0_new.root");
  ntu->Add("data/train_mvain_mu_sync_dy2j_0_new.root");
  ntu->Add("data/train_mvain_mu_sync_dy3j_0_new.root");
  ntu->Add("data/train_mvain_mu_sync_dy4j_0_new.root");
  ntu->Merge("data/tauSignalAndBackground.root");

}
