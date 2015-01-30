#include <iostream>
#include <cmath>
#include <iostream> 
#include <vector>
#include <fstream>
#include <string>

#include "TFile.h"
#include "TH1F.h"
#include "TROOT.h"
#include "TString.h"

#include "TAMS/TAMS.h"

using namespace std;

float fom_plot(TH1F* histo1,TH1F* histo2, std::string plotname = "results/FoMplot.pdf", bool rebin = false, bool useMinbr = false) {


  float fom;
  float sys = 0.2; // systematic error
  TAMS ta (histo1, histo2, sys);
  if(rebin)  ta.rebinEqui();
  if(useMinbr) ta.setbr(1e-6);
  std::cout << "The br used is: " << ta.getbr() << std::endl;
  ta.calc();
  fom = ta.ams_syst_stat();

  std::cout << "The Figure of Merit is: " << fom << std::endl;

//  string filename(plotname);
  ta.savePlot( plotname );

  return fom;

}
