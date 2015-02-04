# macro to produce analysis.pdf for different options
import os

f = open('config/optionsFileList_total.txt')
lines = f.readlines()
f.close()
 
for line in lines:
  lineSplit = line.split("\n")
  ahistoFile = "results/trainHtt" + lineSplit[0] + "_ahist.txt"
  analysFile = "results/trainHtt" + lineSplit[0] + "_analysis.pdf"
  correlFile = "results/trainHtt" + lineSplit[0] + "_correl_signi.txt"
  print "Creating analysis pdf for " + ahistoFile
  command = "root -l -q -b macros/analysis.C'("
  command += "\"" + ahistoFile + "\","
  command += "\"" + analysFile + "\",1,"
  command += "\"" + correlFile + "\")'"
  os.system(command)

print "End"

#root -l -q -b macros/analysis.C'("results_old/trainHtt_TASKCLA_PRE610_LOSSENTROPY_LEARN0_SPEED1.0_ITER220_REGOFF_METHBFGS_SHAPEINCL_ahist.txt","analysis.pdf",1, "results_old/trainHtt_TASKCLA_PRE610_LOSSENTROPY_LEARN0_SPEED1.0_ITER220_REGOFF_METHBFGS_SHAPEINCL_correl_signi.txt")'
