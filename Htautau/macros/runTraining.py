# macro to train the NeuroMVA with different options
import os
import timeit

start_time = timeit.default_timer()

currentDir = os.getcwd()+'/';
print currentDir;

numberOfConfigFile = 2170
executable = "./trainHtt "
#varFile = "config/varFileList "
varFile = "config/varFileList "
#varFile = "config/varFileList_complete "
inputFile = "input/tauSignalAndBackground.root "
#inputFile_sig = "config/inputFileList_Htt_train_sig "
#inputFile_bkg = "config/inputFileList_Htt_train_bkg "

#outScript = open("Job.sh","w");
#outScript = open("Job2.sh","w");
#outScript = open("Job3.sh","w");
#outScript = open("Job4.sh","w");
#outScript = open("Job5.sh","w");
#outScript = open("Job6.sh","w");
#outScript = open("Job7.sh","w");
outScript = open("Job8.sh","w");
outScript.write('#!/bin/bash');
outScript.write("\n"+'cd '+currentDir);

#  optionsFile = "config/options/optionList" + repr(numberOptionsFile) + " "
for numberOptionsFile in range(1,numberOfConfigFile):
  optionsFile = "config/options2/optionList" + repr(numberOptionsFile) + " "
  outScript.write("\n"+executable+varFile+optionsFile+inputFile);

print 'Closing out script';

elapsed = timeit.default_timer() - start_time
print elapsed

