# macro to train the NeuroMVA with different options
import os
import timeit

start_time = timeit.default_timer()

currentDir = os.getcwd()+'/';
print currentDir;


executable = "./trainHtt "
varFile = "config/varFileList "
inputFile_sig = "config/inputFileList_Htt_train_sig "
inputFile_bkg = "config/inputFileList_Htt_train_bkg "

outScript = open("Job.sh","w");
outScript.write('#!/bin/bash');
outScript.write("\n"+'cd '+currentDir);

for numberOptionsFile in range(1,730):
  optionsFile = "config/options/optionList" + repr(numberOptionsFile) + " "
  outScript.write("\n"+executable+varFile+optionsFile+inputFile_sig+inputFile_bkg);

print 'Closing out script';

#command = "sh Job.sh"
#os.system(command)

elapsed = timeit.default_timer() - start_time
print elapsed

