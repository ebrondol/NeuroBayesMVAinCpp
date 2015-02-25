//TAMS
//Author: Martin Flechl, 2014
//calculate average median significance of two histograms

#ifndef ROOT_TAMS
#define ROOT_TAMS

#ifndef ROOT_TH1F
#include "TH1F.h"
#endif

#include <iostream> //only for cout
#include "TCanvas.h" //only to save plot
#include "TLegend.h" //only to save plot
#include "TStyle.h" //only to save plot
//#include "THStack.h" //only to save plot

#include <vector>

//below: rescale inputs, e.g. to extrapolate to 13 TeV running and more lumi
//const float lsc=0.29;
//const float sc13_s=2.37;
//const float sc13_b=1.70;

//below: do not rescale inputs
const float lsc=1.0;
const float sc13_s=1;
const float sc13_b=1;

const float bmin=0.1;

class TH1F;

class TAMS {
 protected:
  float m_simple[3]; //simple s/sqrt(b)
  float m_simple_syst[3]; //simple s/sqrt(b(1+sys*sys*b))
  float m_simple_stat[3]; //simple s/sqrt(b) with stat. unc. on b
  float m_simple_syst_stat[3]; //simple s/sqrt(b(1+sys*sys*b)) with stat unc. on b
  float m_ams[3]; //ams //0 central, 1 -err, 2 +err
  float m_ams_stat[3]; //ams with stat unc on b
  float m_ams_syst[3]; //ams with sys unc on b
  float m_ams_syst_stat[3]; //ams with sys unc on b

  float m_rsys;
  float m_breg;
  TH1F *m_h1; //histo signal
  TH1F *m_h2; //histo bkg
  TH1F *m_h1s; //histo signal, with syst unc
  TH1F *m_h2s; //histo bkg, with syst unc
  int ER;


 public:
  TAMS() { m_rsys=0.1; m_simple[0]=-1; m_simple_syst[0]=-1; m_simple_stat[0]=-1; m_simple_syst_stat[0]=-1; m_ams[0]=-1; m_ams_stat[0]=-1; m_ams_syst[0]=-1; m_ams_syst_stat[0]=-1; };
  //  TAMS(TH1F *_h1, TH1F *_h2, float _rsys=0.1, float _breg=1.0) { m_h1=_h1; m_h2=_h2; m_rsys=_rsys; m_breg=_breg; ER=0; m_simple[0]=-1; m_simple_syst[0]=-1; m_simple_stat[0]=-1; m_simple_syst_stat[0]=-1; m_ams[0]=-1; m_ams_stat[0]=-1; m_ams_syst[0]=-1; m_ams_syst_stat[0]=-1; };
  TAMS(TH1F *_h1, TH1F *_h2, float _rsys=0.1, float _breg=1.0) { m_h1=_h1; m_h2=_h2; m_rsys=_rsys; m_breg=_breg; ER=0; m_simple[0]=-1; m_simple_syst[0]=-1; m_simple_stat[0]=-1; m_simple_syst_stat[0]=-1; m_ams[0]=-1; m_ams_stat[0]=-1; m_ams_syst[0]=-1; m_ams_syst_stat[0]=-1; TH1::SetDefaultSumw2(kTRUE); m_h1->Scale(sc13_s*lsc); m_h2->Scale(sc13_b*lsc); };
  virtual ~TAMS(){};

  void seth1(TH1F *_h){ m_h1=_h; };
  void seth2(TH1F *_h){ m_h2=_h; };
  void seth(TH1F *_h1, TH1F *_h2){ m_h1=_h1; m_h2=_h2; };
  void setsys(float _sys){ m_rsys=_sys; }
  void setbr(float _br){ m_breg=_br; }
  float getbr(){ return m_breg; }

  float _any(float *m, int o){ if (m<0) this->calc(); if (o==0) return m[o]; else if (o==-1) return m[1]; else if (o==1) return m[2]; else return -1; };  //-err: -1; central: 0; +err: 1
  float ams(int o=0){ float *m_any=m_ams; return _any(m_any, o); }
  float ams_stat(int o=0){ float *m_any=m_ams_stat; return _any(m_any, o); }
  float ams_syst(int o=0){ float *m_any=m_ams_syst; return _any(m_any, o); }
  float ams_syst_stat(int o=0){ float *m_any=m_ams_syst_stat; return _any(m_any, o); }
  //  float ams(int o=0){ if (m_ams<0) this->calc(); if (o==0) return m_ams[o]; else if (o==-1) return m_ams[1]; else if (o==1) return m_ams[2]; else return -1; };  //-err: -1; central: 0; +err: 1

  float simple(int o=0){ float *m_any=m_simple; return _any(m_any, o); }
  float simple_stat(int o=0){ float *m_any=m_simple_stat; return _any(m_any, o); }
  float simple_syst(int o=0){ float *m_any=m_simple_syst; return _any(m_any, o); }
  float simple_syst_stat(int o=0){ float *m_any=m_simple_syst_stat; return _any(m_any, o); }

  void listAll(int really_all=0){
    std::cout << "Systematics: " << this->m_rsys << std::endl;
    std::cout << "AMS              :" << this->ams(0)              << "\t -" << this->ams(-1)              << " +" << this->ams(+1)              << std::endl;
    std::cout << "AMS    stat      :" << this->ams_stat(0)         << "\t -" << this->ams_stat(-1)         << " +" << this->ams_stat(+1)         << std::endl;
    std::cout << "AMS    syst      :" << this->ams_syst(0)         << "\t -" << this->ams_syst(-1)         << " +" << this->ams_syst(+1)         << std::endl;
    std::cout << "AMS    stat syst :" << this->ams_syst_stat(0)    << "\t -" << this->ams_syst_stat(-1)    << " +" << this->ams_syst_stat(+1)    << std::endl;

    if (really_all){
      std::cout << "Simple           :" << this->simple(0)           << "\t -" << this->simple(-1)           << " +" << this->simple(+1)           << std::endl;
      std::cout << "Simple stat      :" << this->simple_stat(0)      << "\t -" << this->simple_stat(-1)      << " +" << this->simple_stat(+1)      << std::endl;
      std::cout << "Simple syst      :" << this->simple_syst(0)      << "\t -" << this->simple_syst(-1)      << " +" << this->simple_syst(+1)      << std::endl;
      std::cout << "Simple stat syst :" << this->simple_syst_stat(0) << "\t -" << this->simple_syst_stat(-1) << " +" << this->simple_syst_stat(+1) << std::endl;
    }
  }

  void savePlot(TString fname="plot_tams.png"){

    if (!ER){
      m_h1s=new TH1F(*m_h1); //if no equi-rebin, signal
      m_h2s=new TH1F(*m_h2);  //if no equi-rebin, bkg
    }

    std::cout << std::scientific;
    //34.8353 --  1423.2 events after presel, s / b
//    for (int i=1; i<=m_h1s->GetNbinsX(); i++){
      //      std::cout << "Bin " << i << "\t" << m_h1s->GetBinContent(i)/34.8353 << "\t" << m_h2s->GetBinContent(i)/1423.2  << std::endl;
//      std::cout << "Bin " << i << "\t" << m_h1s->GetBinContent(i)/m_h2s->GetBinContent(i)  << std::endl;
//    }

    gStyle->SetOptStat(0);
    m_h2s->SetXTitle("BDT score");
    m_h2s->SetYTitle("Events");
    m_h1s->SetLineColor(kRed);
    m_h2s->SetLineColor(kBlue);

    TH1F *m_h3=new TH1F(*m_h2);
    TH1F *m_h2b=new TH1F(*m_h2s);
    for (int i=0; i<=m_h3->GetNbinsX()+1; i++) m_h3->SetBinError(i,0);
    m_h3->Add(m_h1s);

    m_h3->SetMarkerSize(0.9);
    m_h3->SetLineWidth(3);
    m_h2s->SetFillColor(kBlack);
    m_h2s->SetFillStyle(3004);
    m_h2s->SetMarkerSize(0.0);

    m_h2b->SetLineColor(kOrange+1);
    m_h2b->SetFillColor(kOrange+1);

    TLegend *leg=new TLegend(0.65,0.75,0.85,0.85);
    leg->AddEntry(m_h1s,"Signal","lep");
    leg->AddEntry(m_h2s,"Background","lep");
    leg->SetFillColor(10);
    leg->SetShadowColor(10);
    leg->SetLineColor(10);

    TLegend *leg2=new TLegend(0.45,0.7,0.85,0.85);
    leg2->AddEntry(m_h2b,"Background","fe");
    leg2->AddEntry(m_h3,"Signal+Background","lep");
    leg2->SetFillColor(10);
    leg2->SetShadowColor(10);
    leg2->SetLineColor(10);

    TCanvas *cx=new TCanvas("cx","cx");
    m_h2s->Draw();
    m_h1s->Draw("same");
    gPad->SetLogy();
    leg->Draw();
    gPad->SaveAs(fname);

    TCanvas *cy=new TCanvas("cy","cy");
    m_h2b->Draw("hist");
    m_h2s->Draw("E2same");
    m_h3->Draw("same");
    gPad->SetLogy();
    leg2->Draw();
    fname.ReplaceAll(".pdf","2.pdf");
    gPad->SaveAs(fname);

    cx->Close();
    cy->Close();
    if (m_h3) delete m_h3;
    if (m_h2b) delete m_h2b;
  }

  void rebinEqui() {
    this->rebin();
    int nbins=m_h1->GetNbinsX();

    double *bin_contents_s=new double[nbins+2]; //2: over/underflow; here: empty
    double *bin_contents_b=new double[nbins+2];
    double *bin_errors_s=new double[nbins+2];
    double *bin_errors_b=new double[nbins+2];

    for (int j=0; j<=nbins+1; j++){
      bin_contents_s[j]=m_h1->GetBinContent(j);
      bin_contents_b[j]=m_h2->GetBinContent(j);
      bin_errors_s[j]=m_h1->GetBinError(j);
      bin_errors_b[j]=m_h2->GetBinError(j);
    }
    float blow=m_h1->GetBinLowEdge(1);
    float bhigh=m_h1->GetBinLowEdge(nbins+1);

    if (m_h1) delete m_h1;
    if (m_h2) delete m_h2;
    //    if (m_h1s) delete m_h1s;
    //    if (m_h2s) delete m_h2s;

    m_h1=new TH1F("s","",nbins,blow, bhigh );
    m_h2=new TH1F("b","",nbins,blow, bhigh );
    m_h1s=new TH1F("ss","",nbins,blow, bhigh );
    m_h2s=new TH1F("bs","",nbins,blow, bhigh );

    m_h1->SetContent(bin_contents_s);
    m_h2->SetContent(bin_contents_b);
    m_h1s->SetContent(bin_contents_s);
    m_h2s->SetContent(bin_contents_b);
    m_h1->SetError(bin_errors_s);
    m_h2->SetError(bin_errors_b);

    for (int i=0; i<=nbins+1; i++){
      bin_errors_s[i]=sqrt( pow(bin_errors_s[i],2) + pow(bin_contents_s[i]*m_rsys,2) );
      bin_errors_b[i]=sqrt( pow(bin_errors_b[i],2) + pow(bin_contents_b[i]*m_rsys,2) );
    }
    m_h1s->SetError(bin_errors_s);
    m_h2s->SetError(bin_errors_b);

    if (bin_contents_s) delete[] bin_contents_s;
    if (bin_contents_b) delete[] bin_contents_b;
    if (bin_errors_s) delete[] bin_errors_s;
    if (bin_errors_b) delete[] bin_errors_b;

    ER=1;
  }

  void rebin() {
    //    const float RELSTATMAX=0.5;
    const float RELSTATMAX=0.60;
    //XX
    //    const float BINC=1.33;
    const float BINC=1.0;

    std::vector<float> bin_edge;
    std::vector<float> bin_s;
    std::vector<float> bin_b;
    std::vector<float> bin_berr;
    std::vector<float> bin_serr;
    
    int nedges=m_h1->GetNbinsX()+1; //edges=bins+1
    float highest_edge=m_h1->GetBinLowEdge( nedges );
    bin_edge.push_back(highest_edge);

    float s=0;
    float b=0;
    float berr2=0;
    float serr2=0;
    float bprev=0;

    for (int i=nedges-1; i>=1; i--){ //loop over bin edges
      s+=m_h1->GetBinContent(i);
      b+=m_h2->GetBinContent(i);
      serr2+=pow(m_h1->GetBinError(i),2);
      berr2+=pow(m_h2->GetBinError(i),2);

      float t_edge=m_h1->GetBinLowEdge( i );

      //check if this is a new edge
      if ( b<1e-3 ) continue; //if b is negativ or 0 or very small, continue
      if ( (sqrt(berr2)/b)>RELSTATMAX ) continue; //if the rel stat unc on the background is >X%, continue
      if ( b<bprev*BINC ) continue; //more b than bin to the right (previous bin)

      if ( t_edge<0.8 )
        if ( bin_edge.back()-t_edge < 0.05 ) continue;
      if ( t_edge<0.6 )
        if ( bin_edge.back()-t_edge < 0.10 ) continue;
      if ( t_edge<0.4 )
        if ( bin_edge.back()-t_edge < 0.20 ) continue;

      //we have a new edge!
      bin_edge.push_back( m_h1->GetBinLowEdge( i ) );
      bin_s.push_back( s );
      bin_b.push_back( b );
      bin_serr.push_back( sqrt(serr2) );
      bin_berr.push_back( sqrt(berr2) );
      bprev=b;
      b=0; s=0; serr2=0; berr2=0;
    }
    //    lbin=bin_edge.size()-1;
    if (  fabs( bin_edge.back()-m_h1->GetBinLowEdge( 1 )  )>1e-5  ){ //replace lowest bin boarder with lower-most old bin border
      bin_edge.back()=m_h1->GetBinLowEdge( 1 );
      bin_s.back()+=s;
      bin_b.back()+=b;
      bin_serr.back()=sqrt( pow(bin_serr.back(),2)+serr2 );
      bin_berr.back()=sqrt( pow(bin_berr.back(),2)+berr2 );
    }

    //debugging: compare before/after:
    //    for (int i=1; i<=m_h1->GetNbinsX(); i++) 
    //      cout << m_h1->GetBinLowEdge(i) << " - " << m_h1->GetBinLowEdge(i+1) << ":\t" << m_h1->GetBinContent(i) << "\t" << m_h1->GetBinError(i)  << endl;

    //Create new histos
    int nbins=bin_edge.size()-1;
    float *array_edge=new float[nbins+1];
    for (int j=0; j<=nbins; j++) array_edge[j]=bin_edge.at(nbins-j);

    m_h1=new TH1F("s","",nbins,array_edge);
    m_h2=new TH1F("b","",nbins,array_edge);

    if (array_edge) delete[] array_edge;

    ////int border[]={1,2,3,4,5,6,7,8,9,10,11,12,13,16,15,17,14,18,19,20};
    //  int border[]={1,2,3,7,4,6,5,8,9,10,11,12,13,14,15,16,17,18,19,20};

    for (int j=0; j<nbins; j++){
      m_h1->SetBinContent(j+1,bin_s.at   (nbins-1-j) );
      m_h1->SetBinError(  j+1,bin_serr.at(nbins-1-j) );
      m_h2->SetBinContent(j+1,bin_b.at   (nbins-1-j) );
      m_h2->SetBinError(  j+1,bin_berr.at(nbins-1-j) );
      //      m_h1->SetBinContent(j+1,bin_s.at   (border[nbins-1-j]-1) );
      //      m_h1->SetBinError(  j+1,bin_serr.at(border[nbins-1-j]-1) );
      //      m_h2->SetBinContent(j+1,bin_b.at   (border[nbins-1-j]-1) );
      //      m_h2->SetBinError(  j+1,bin_berr.at(border[nbins-1-j]-1) );
    }

    //debugging: compare before/after:
    //    cout << "after" << endl;
    //    for (int i=1; i<=m_h1->GetNbinsX(); i++) 
    //      cout << m_h1->GetBinLowEdge(i) << " - " << m_h1->GetBinLowEdge(i+1) << ":\t" << m_h1->GetBinContent(i) << "\t" << m_h1->GetBinError(i)  << endl;

  }

  void calc() { 
    int nbins=m_h1->GetNbinsX();
    if ( nbins != m_h2->GetNbinsX() ){
      std::cerr << "ERROR: Not the same number of bins! Giving up..." << std::endl;
      return;
    }
    if ( fabs( m_h1->GetBinLowEdge(nbins+1) - m_h2->GetBinLowEdge(nbins+1) )>1e-6 ){
      std::cerr << "ERROR: Not the same range! Giving up..." << m_h1->GetBinLowEdge(nbins) << " vs " << m_h2->GetBinLowEdge(nbins) << std::endl;
      return;
    }

    //    const float br=1.0;
    float br=m_breg;
    //float br=1e-6;
    float s, b, bt, b_stat, b_syst, b_stat2, b_syst2, s_stat;
    m_ams[0]=0; m_ams_stat[0]=0;  m_ams_syst[0]=0; m_ams_syst_stat[0]=0; m_simple[0]=0; m_simple_syst[0]=0; m_simple_stat[0]=0; m_simple_syst_stat[0]=0; 
    float _ams_err[4]={0}; float _ams_stat_err[4]={0};  float _ams_syst_err[4]={0}; float _ams_syst_stat_err[4]={0}; float _simple_err[4]={0}; float _simple_syst_err[4]={0}; float _simple_stat_err[4]={0}; float _simple_syst_stat_err[4]={0}; 
    for (int ibin=1; ibin<=m_h1->GetNbinsX(); ibin++){
      s=m_h1->GetBinContent(ibin);
      b=m_h2->GetBinContent(ibin);
      s_stat=m_h1->GetBinError(ibin); //absolute unc
      b_stat=m_h2->GetBinError(ibin); //absolute unc
      if ( b_stat<0.5*sqrt(br) ) b_stat=0.5*sqrt(br);
      b_stat2=b_stat*b_stat;
      b_syst=b*m_rsys; //absolute unc
      if (b_syst<m_rsys*br ) b_syst=m_rsys*br;
      b_syst2=b_syst*b_syst;
      bt=b+br;

      m_simple[0]           += calc_simple2(s, bt);
      m_simple_stat[0]      += calc_simple2(s, bt, b_stat2);
      m_simple_syst[0]      += calc_simple2(s, bt, b_syst2);
      m_simple_syst_stat[0] += calc_simple2(s, bt, b_stat2+b_syst2);

      m_ams[0]              += calc_ams2(s, bt);
      m_ams_stat[0]         += calc_ams2(s, bt, b_stat2);
      m_ams_syst[0]         += calc_ams2(s, bt, b_syst2);
      m_ams_syst_stat[0]    += calc_ams2(s, bt, b_syst2+b_stat2);

      calc_simple2_err(_simple_err          , s, bt, s_stat, b_stat);
      calc_simple2_err(_simple_stat_err     , s, bt, s_stat, b_stat, b_stat2);
      calc_simple2_err(_simple_syst_err     , s, bt, s_stat, b_stat, b_syst2);
      calc_simple2_err(_simple_syst_stat_err, s, bt, s_stat, b_stat, b_syst2+b_stat2);

      calc_ams2_err(_ams_err          , s, bt, s_stat, b_stat);
      calc_ams2_err(_ams_stat_err     , s, bt, s_stat, b_stat, b_stat2);
      calc_ams2_err(_ams_syst_err     , s, bt, s_stat, b_stat, b_syst2);
      calc_ams2_err(_ams_syst_stat_err, s, bt, s_stat, b_stat, b_syst2+b_stat2);

      //      if ( ( m_h1->GetNbinsX()-ibin )<=3 ){
      //        cout << "Bin " << ibin << ":\t AMS=" << sqrt(calc_ams2(s, bt, b_syst2+b_stat2)) << "\t total=" << sqrt(m_ams_syst_stat[0]) << "\t s=" << s << " , b=" << b << endl;
      //        cout << "Bin " << ibin << ":\t AMS=" << sqrt(calc_simple2(s, bt, b_syst2+b_stat2)) << "\t total=" << sqrt(m_simple_syst_stat[0]) << endl;
      //      }

    }
    m_simple[0]=sqrt(m_simple[0]);
    m_simple_stat[0]=sqrt(m_simple_stat[0]);
    m_simple_syst[0]=sqrt(m_simple_syst[0]);
    m_simple_syst_stat[0]=sqrt(m_simple_syst_stat[0]);

    m_ams[0]=sqrt(m_ams[0]);
    m_ams_stat[0]=sqrt(m_ams_stat[0]);
    m_ams_syst[0]=sqrt(m_ams_syst[0]);
    m_ams_syst_stat[0]=sqrt(m_ams_syst_stat[0]);

    m_ams[1]= sqrt(   pow( sqrt(_ams_err[0])-m_ams[0], 2 )  + pow( sqrt(_ams_err[3])-m_ams[0], 2 ) );
    m_ams[2]= sqrt(   pow( sqrt(_ams_err[1])-m_ams[0], 2 )  + pow( sqrt(_ams_err[2])-m_ams[0], 2 ) );
    m_ams_stat[1]= sqrt(   pow( sqrt(_ams_stat_err[0])-m_ams_stat[0], 2 )  + pow( sqrt(_ams_stat_err[3])-m_ams_stat[0], 2 ) );
    m_ams_stat[2]= sqrt(   pow( sqrt(_ams_stat_err[1])-m_ams_stat[0], 2 )  + pow( sqrt(_ams_stat_err[2])-m_ams_stat[0], 2 ) );
    m_ams_syst[1]= sqrt(   pow( sqrt(_ams_syst_err[0])-m_ams_syst[0], 2 )  + pow( sqrt(_ams_syst_err[3])-m_ams_syst[0], 2 ) );
    m_ams_syst[2]= sqrt(   pow( sqrt(_ams_syst_err[1])-m_ams_syst[0], 2 )  + pow( sqrt(_ams_syst_err[2])-m_ams_syst[0], 2 ) );
    m_ams_syst_stat[1]= sqrt(   pow( sqrt(_ams_syst_stat_err[0])-m_ams_syst_stat[0], 2 )  + pow( sqrt(_ams_syst_stat_err[3])-m_ams_syst_stat[0], 2 ) );
    m_ams_syst_stat[2]= sqrt(   pow( sqrt(_ams_syst_stat_err[1])-m_ams_syst_stat[0], 2 )  + pow( sqrt(_ams_syst_stat_err[2])-m_ams_syst_stat[0], 2 ) );

    m_simple[1]= sqrt(   pow( sqrt(_simple_err[0])-m_simple[0], 2 )  + pow( sqrt(_simple_err[3])-m_simple[0], 2 ) );
    m_simple[2]= sqrt(   pow( sqrt(_simple_err[1])-m_simple[0], 2 )  + pow( sqrt(_simple_err[2])-m_simple[0], 2 ) );
    m_simple_stat[1]= sqrt(   pow( sqrt(_simple_stat_err[0])-m_simple_stat[0], 2 )  + pow( sqrt(_simple_stat_err[3])-m_simple_stat[0], 2 ) );
    m_simple_stat[2]= sqrt(   pow( sqrt(_simple_stat_err[1])-m_simple_stat[0], 2 )  + pow( sqrt(_simple_stat_err[2])-m_simple_stat[0], 2 ) );
    m_simple_syst[1]= sqrt(   pow( sqrt(_simple_syst_err[0])-m_simple_syst[0], 2 )  + pow( sqrt(_simple_syst_err[3])-m_simple_syst[0], 2 ) );
    m_simple_syst[2]= sqrt(   pow( sqrt(_simple_syst_err[1])-m_simple_syst[0], 2 )  + pow( sqrt(_simple_syst_err[2])-m_simple_syst[0], 2 ) );
    m_simple_syst_stat[1]= sqrt(   pow( sqrt(_simple_syst_stat_err[0])-m_simple_syst_stat[0], 2 )  + pow( sqrt(_simple_syst_stat_err[3])-m_simple_syst_stat[0], 2 ) );
    m_simple_syst_stat[2]= sqrt(   pow( sqrt(_simple_syst_stat_err[1])-m_simple_syst_stat[0], 2 )  + pow( sqrt(_simple_syst_stat_err[2])-m_simple_syst_stat[0], 2 ) );
  };

  float calc_simple2(float s, float b, float berr2=0){
    //    if ( (b+berr2)<1e-8 ) cout << "XX " << b << " + " << berr2 << endl;
    if (b<bmin) b=bmin;
    return pow( s/sqrt( b + berr2 ) , 2 );
  }

  float calc_ams2(float s, float b){
    if (b<bmin) b=bmin;
    return 2*(   (s+b) * log( 1 + s/b  ) - s  );
  }
  float calc_ams2(float s, float b, float berr2){
    if (b<bmin) b=bmin;
    if (berr2<0) return calc_ams2(s, b);
    return 2*(   (s+b) * log(  (s+b)*(b+berr2) / ( b*b + (s+b)*berr2 )  )    -  b*b/berr2 * log( 1 + berr2*s/( b*(b+berr2) )   )   );
  }

  void calc_ams2_err(float m_ams_err[4], float s, float b, float s_stat, float b_stat, float berr2=-1){
    m_ams_err[0]           += calc_ams2(s, b+b_stat, berr2);
    m_ams_err[1]           += calc_ams2(s, b-b_stat, berr2);
    m_ams_err[2]           += calc_ams2(s+s_stat, b, berr2);
    m_ams_err[3]           += calc_ams2(s-s_stat, b, berr2);
  }
  void calc_simple2_err(float m_simple_err[4], float s, float b, float s_stat, float b_stat, float berr2=0){
    m_simple_err[0]           += calc_simple2(s, b+b_stat, berr2);
    m_simple_err[1]           += calc_simple2(s, b-b_stat, berr2);
    m_simple_err[2]           += calc_simple2(s+s_stat, b, berr2);
    m_simple_err[3]           += calc_simple2(s-s_stat, b, berr2);
  }

  ClassDef(TAMS,0);

};

//#if !defined(__CINT__)
ClassImp(TAMS);
//#endif



#endif
