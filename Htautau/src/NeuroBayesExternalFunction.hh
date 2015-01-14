#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <cstdlib>

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

