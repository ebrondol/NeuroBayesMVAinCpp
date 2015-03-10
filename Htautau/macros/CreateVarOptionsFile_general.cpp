// g++ macros/CreateOptionsFile.cpp
// macros to create variables option files

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <map>
#include <cstdlib>
#include <sstream>

using namespace std;

std::string summaryFileName = "config/varOptionsFileList_total.txt";
std::string folderFileName = "config/varOptions/";

template <typename T>
string NumberToString ( T Number )
{
     ostringstream ss;
     ss << Number;
     return ss.str();
}


int main() {

  int totNumbOfFiles = 1;

  vector<string> jdeta_options;
//  jdeta_options.push_back("12");
  jdeta_options.push_back("14");
//  jdeta_options.push_back("15");
//  jdeta_options.push_back("23");
  jdeta_options.push_back("24");
//  jdeta_options.push_back("25");

  vector<string> lep_etacentrality_options;
//  lep_etacentrality_options.push_back("12");
  lep_etacentrality_options.push_back("14");
//  lep_etacentrality_options.push_back("15");
//  lep_etacentrality_options.push_back("23");
  lep_etacentrality_options.push_back("24");
//  lep_etacentrality_options.push_back("25");

  vector<string> met_centrality_options;
//  met_centrality_options.push_back("12");
  met_centrality_options.push_back("14");
//  met_centrality_options.push_back("15");
//  met_centrality_options.push_back("23");
  met_centrality_options.push_back("24");
//  met_centrality_options.push_back("25");

  vector<string> mvis_options;
//  mvis_options.push_back("12");
  mvis_options.push_back("14");
//  mvis_options.push_back("15");
//  mvis_options.push_back("23");
  mvis_options.push_back("24");
//  mvis_options.push_back("25");

  vector<string> pt_sum_options;
//  pt_sum_options.push_back("12");
  pt_sum_options.push_back("14");
//  pt_sum_options.push_back("15");
//  pt_sum_options.push_back("23");
  pt_sum_options.push_back("24");
//  pt_sum_options.push_back("25");

  vector<string> pt_tot_options;
//  pt_tot_options.push_back("12");
  pt_tot_options.push_back("14");
//  pt_tot_options.push_back("15");
//  pt_tot_options.push_back("23");
  pt_tot_options.push_back("24");
//  pt_tot_options.push_back("25");

  vector<string> svfit_mass_options;
//  svfit_mass_options.push_back("12");
  svfit_mass_options.push_back("14");
//  svfit_mass_options.push_back("15");
//  svfit_mass_options.push_back("23");
  svfit_mass_options.push_back("24");
//  svfit_mass_options.push_back("25");




  ofstream summary(summaryFileName.c_str());

  //ERICA::not proud of me
  for( vector<string>::iterator it = svfit_mass_options.begin(); it != svfit_mass_options.end(); ++it ) {
    for( vector<string>::iterator it2 = jdeta_options.begin(); it2 != jdeta_options.end(); ++it2 ) {
      for( vector<string>::iterator it3 = pt_tot_options.begin(); it3 != pt_tot_options.end(); ++it3 ) {
        for( vector<string>::iterator it4 = met_centrality_options.begin(); it4 != met_centrality_options.end(); ++it4 ) {
          for( vector<string>::iterator it5 = lep_etacentrality_options.begin(); it5 != lep_etacentrality_options.end(); ++it5 ) {
            for( vector<string>::iterator it6 = pt_sum_options.begin(); it6 != pt_sum_options.end(); ++it6 ) {
              for( vector<string>::iterator it7 = mvis_options.begin(); it7 != mvis_options.end(); ++it7 ) {

                string outputname;
                outputname += "_jdeta" + (*it);
                outputname += "lepetacentrality" + (*it2);
                outputname += "metcentrality" + (*it3);
                outputname += "mvis" + (*it4);
                outputname += "ptsum" + (*it5);
                outputname += "pttot" + (*it6);
                outputname += "svfitmass" + (*it7);
                summary << outputname << endl;

                string FileName = folderFileName + "optionList" + NumberToString(totNumbOfFiles);
                ofstream f(FileName.c_str());

                f << "STARTVARSET" << endl;
 	        f << "jdeta" <<  " " << (*it) << endl;
        	f << "lep_etacentrality" <<  " " << (*it2) << endl;
        	f << "met_centrality" << " " << (*it3) << endl;
        	f << "mvis" <<  " " << (*it4) << endl;
        	f << "pt_sum" <<  " " << (*it5) << endl;
        	f << "pt_tot" <<  " " << (*it6) << endl;
        	f << "svfit_mass" <<  " " << (*it7) << endl;
	        f << "ENDVARSET" << endl;

	        f.close();
		totNumbOfFiles++;

              }
            }
          }
        }
      }
    }
  }

  summary.close();
  cout << "All possible options in " << summaryFileName << endl;
  cout << "Option file lists in " << folderFileName << endl;

  return 0;


}
