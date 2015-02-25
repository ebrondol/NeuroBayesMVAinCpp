#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <cstdlib>

#include "NeuroBayesTeacher.hh"

using namespace std;

void ReadMap(std::map<string,int> mymap){

  for (std::map<string,int>::iterator it = mymap.begin(); it != mymap.end(); ++it)
    std::cout << it->first << " => " << it->second << '\n';

  return;
}

void ReadVec(std::vector<TString> myvec){

  for (unsigned int it = 0; it != myvec.size(); ++it)
    std::cout << myvec.at(it) << '\n';

  return;
}

std::map<string, int> readVarFile(string ConfigFileName, bool debug = false){

  ifstream codegen(ConfigFileName.c_str(), ifstream::in);
  map<string, int> VarProProFlagsMap;
 
  if(codegen.bad()) {
    cout << "PseudoCodegen-file " << ConfigFileName << " not found " << endl;
    return VarProProFlagsMap;
  } else {
    if(debug) cout << "Reading Pseudocodegenfile in " << ConfigFileName << endl;
  }

  string buffer;
  getline(codegen,buffer); 		//STARTVARSET 
  if(debug) cout << buffer << endl;

  while(true) {

    getline(codegen,buffer);
    unsigned int name_end = buffer.find(" ");
    if( buffer.substr(0, name_end ) == "ENDVARSET") {
      break;
    }

    if(name_end < buffer.size() ) {

      string flag = buffer.substr(name_end, buffer.size());
      VarProProFlagsMap[buffer.substr(0, name_end )] = atoi(flag.c_str());
    } else {
      VarProProFlagsMap[buffer.substr(0, name_end )] = 0;
    }


  }
    
  if(debug) ReadMap(VarProProFlagsMap);

  return VarProProFlagsMap;
}

std::vector<TString> readInputFile(string ConfigFileName, bool debug = false){

  ifstream codegen(ConfigFileName.c_str(), ifstream::in);
  vector<TString> FileNames;

  if(codegen.bad()) {
    cout << "PseudoCodegen-file " << ConfigFileName << " not found " << endl;
    return FileNames;
  } else {
    if(debug) cout << "Reading Pseudocodegenfile in " << ConfigFileName << endl;
  }

  string buffer;
  getline(codegen,buffer);              //STARTINPUTFILESLIST
  if(debug) cout << buffer << endl;

  while(true) {

    getline(codegen,buffer);
    unsigned int name_end = buffer.find(" ");
    if( buffer.substr(0, name_end ) == "ENDINPUTFILELIST") {
      break;
    }

    TString name = buffer.substr(0, name_end );
    FileNames.push_back(name);

  }

  if(debug) ReadVec(FileNames);

  return FileNames;
}


string readStringOptionFromFile(string ConfigFileName, string ChoosenOption, bool debug = false){

  ifstream codegen(ConfigFileName.c_str(), ifstream::in);
  string Value;

  if(codegen.bad()) {
    cout << "PseudoCodegen-file " << ConfigFileName << " not found " << endl;
    return Value;
  } else {
    if(debug) cout << "Reading Pseudocodegenfile in " << ConfigFileName << endl;
  }

  string buffer;
  getline(codegen,buffer);              //STARTOPTIONSLIST
  if(debug) cout << buffer << endl;

  while(true) {

    getline(codegen,buffer);
    unsigned int name_end = buffer.find(" ");
    string Option = buffer.substr(0, name_end ); 

    if( Option == "ENDOPTIONSLIST") {
      break;
    } 

    if( Option == ChoosenOption ) {
      Value = buffer.substr(name_end+1, buffer.size());
      if(debug) cout << "Read var: " << Option << " with value: " << Value <<endl;
      return Value;
    }
  }
      
  return Value;
}


string DefineNBFeatures(NeuroBayesTeacher* nb, string optionFile){

  string OptionValue;
  const char * OptionValue_char;
  int OptionValue_int;
  string outputname;

  // kind of classification
  OptionValue = readStringOptionFromFile(optionFile, "DEF_TASK");
  OptionValue_char = OptionValue.c_str();
  nb->NB_DEF_TASK(OptionValue_char);
  outputname += "_TASK" + OptionValue;

  // preprocessing global flag
  OptionValue = readStringOptionFromFile(optionFile, "DEF_PRE");
  OptionValue_int = atoi(OptionValue.c_str());
  nb->NB_DEF_PRE(OptionValue_int);
  outputname += "_PRE" + OptionValue;

  // energy loss function : ENTROPY, QUADRATIC
  OptionValue = readStringOptionFromFile(optionFile, "DEF_LOSS");
  OptionValue_char = OptionValue.c_str();
  nb->NB_DEF_LOSS(OptionValue_char);
  outputname += "_LOSS" + OptionValue;

  // include distance between the purity and the diagonal
  OptionValue = readStringOptionFromFile(optionFile, "DEF_LEARNDIAG");
  OptionValue_int = atoi(OptionValue.c_str());
  nb->NB_DEF_LEARNDIAG(OptionValue_int);
  outputname += "_LEARN" + OptionValue;

  // weight update after n events
  //nb->NB_DEF_EPOCH(200); 

  // multiplicative factor to enhance global learning speed
  //  OptionValue = readStringOptionFromFile(optionFile, "DEF_SPEED");
  //  ERICA::FIXME the funcion stof does not work.
  //  OptionValue_float = stof(OptionValue);
  nb->NB_DEF_SPEED(1.0);
  outputname += "_SPEED1.0";

  // number of training iteration
  OptionValue = readStringOptionFromFile(optionFile, "DEF_ITER");
  OptionValue_int = atoi(OptionValue.c_str());
  nb->NB_DEF_ITER(OptionValue_int);
  outputname += "_ITER" + OptionValue;

  // regolarization scheme choosen : OFF, REG, ARD, ASR, ALL
  OptionValue = readStringOptionFromFile(optionFile, "DEF_REG");
  OptionValue_char = OptionValue.c_str();
  nb->NB_DEF_REG(OptionValue_char);
  outputname += "_REG" + OptionValue;

  // using or not the BFGS algo : BFGS, NOBFGS
  OptionValue = readStringOptionFromFile(optionFile, "DEF_METHOD");
  OptionValue_char = OptionValue.c_str();
  nb->NB_DEF_METHOD(OptionValue_char);
  outputname += "_METH" + OptionValue;

  // direct connections between input and output layer : OFF, INC, DIAG, MARGINAL 
  OptionValue = readStringOptionFromFile(optionFile, "DEF_SHAPE");
  OptionValue_char = OptionValue.c_str();
  nb->NB_DEF_SHAPE(OptionValue_char);
  outputname += "_SHAPE" + OptionValue;

  return outputname;

}

