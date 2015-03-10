// g++ macros/CreateOptionsFile.cpp
// macros to create option files

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <map>
#include <cstdlib>
#include <sstream>

using namespace std;

std::string summaryFileName = "config/optionsFileList_total2.txt";
std::string folderFileName = "config/options2/";

template <typename T>
string NumberToString ( T Number )
{
     ostringstream ss;
     ss << Number;
     return ss.str();
}


int main() {

  int totNumbOfFiles = 1;
  vector<string> DEF_PRE_options;
  DEF_PRE_options.push_back("112");
  DEF_PRE_options.push_back("412");
  DEF_PRE_options.push_back("612");
  DEF_PRE_options.push_back("812");

  vector<string> DEF_LOSS_options;
  DEF_LOSS_options.push_back("ENTROPY");

  vector<string> DEF_LEARNDIAG_options;
  DEF_LEARNDIAG_options.push_back("0");
  DEF_LEARNDIAG_options.push_back("1");

  vector<string> DEF_ITER_options;
  DEF_ITER_options.push_back("20");
  DEF_ITER_options.push_back("100");
  DEF_ITER_options.push_back("220");

  vector<string> DEF_REG_options;
  DEF_REG_options.push_back("ALL");
  DEF_REG_options.push_back("ARD");
  DEF_REG_options.push_back("ASR");
  DEF_REG_options.push_back("OFF");
  DEF_REG_options.push_back("REG");

  vector<string> DEF_METHOD_options;
  DEF_METHOD_options.push_back("BFGS");
  DEF_METHOD_options.push_back("NOBFGS");

  vector<string> DEF_SHAPE_options;
  DEF_SHAPE_options.push_back("INCL");

  ofstream summary(summaryFileName.c_str());

  //ERICA::not proud of me
  for( vector<string>::iterator it = DEF_PRE_options.begin(); it != DEF_PRE_options.end(); ++it ) {
    for( vector<string>::iterator it2 = DEF_LOSS_options.begin(); it2 != DEF_LOSS_options.end(); ++it2 ) {
      for( vector<string>::iterator it3 = DEF_LEARNDIAG_options.begin(); it3 != DEF_LEARNDIAG_options.end(); ++it3 ) {
        for( vector<string>::iterator it4 = DEF_ITER_options.begin(); it4 != DEF_ITER_options.end(); ++it4 ) {
          for( vector<string>::iterator it5 = DEF_REG_options.begin(); it5 != DEF_REG_options.end(); ++it5 ) {
            for( vector<string>::iterator it6 = DEF_METHOD_options.begin(); it6 != DEF_METHOD_options.end(); ++it6 ) {
              for( vector<string>::iterator it7 = DEF_SHAPE_options.begin(); it7 != DEF_SHAPE_options.end(); ++it7 ) {

                string outputname;
                outputname += "_TASKCLA";
                outputname += "_PRE" + (*it);
                outputname += "_LOSS" + (*it2);
                outputname += "_LEARN" + (*it3);
                outputname += "_SPEED1.0";
                outputname += "_ITER" + (*it4);
                outputname += "_REG" + (*it5);
                outputname += "_METH" + (*it6);
                outputname += "_SHAPE" + (*it7);
                summary << outputname << endl;

                string FileName = folderFileName + "optionList" + NumberToString(totNumbOfFiles);
                ofstream f(FileName.c_str());

                f << "Name of the output: " << endl;
                f << outputname << endl;
                f << "----------------" << endl;
                f << "STARTOPTIONSLIST" << endl;
                f << "DEF_TASK" << " " << "CLA" << endl;
 	        f << "DEF_PRE" <<  " " << (*it) << endl;
        	f << "DEF_LOSS" <<  " " << (*it2) << endl;
        	f << "DEF_LEARNDIAG" << " " << (*it3) << endl;
        	f << "DEF_ITER" <<  " " << (*it4) << endl;
        	f << "DEF_REG" <<  " " << (*it5) << endl;
        	f << "DEF_METHOD" <<  " " << (*it6) << endl;
        	f << "DEF_SHAPE" <<  " " << (*it7) << endl;
	        f << "ENDOPTIONSLIST" << endl;
                f << "----------------" << endl;

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
  cout << "Option file lists in " << folderFileName << endl;

  return 0;


}
