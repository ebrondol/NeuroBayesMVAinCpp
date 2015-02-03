Instruction for NeuroBayes MVA

Follow C/C++ Tutorial @ http://neurobayes.phi-t.de/index.php/tutorials/cc

-----------------------------------------------------------
Export libraries in .bashrc
export NEUROBAYES=/afs/hephy.at/project/cms/neurobayes/3.16.0/x86_64-slc6-gcc44-opt
export LD_LIBRARY_PATH=$NEUROBAYES/lib:$LD_LIBRARY_PATH
export PATH=$NEUROBAYES/external:$PATHexport PATH=$NEUROBAYES/external:$PATH

-----------------------------------------------------------
First run - Basic setup :
	doc/ contains all the documents needed for the analysis (in particular look @ HowToCpp.pdf and NeuroBayes-HowTo.pdf)
	running the analysis tools with:
		root -l -q -b $NEUROBAYES/external/asciiToRoot.C
		root -l -q -b $NEUROBAYES/external/analysis.C'("ahist.txt","analysis.pdf",1)'


-----------------------------------------------------------
Second Run - global PreproFlag and BFGS :
	- qual è la differenza tra i due pdf di output? Vengono utilizzate solo le var utili. (Look last page)

-----------------------------------------------------------
Third run - individual preprocessing flags and zero iterations :
		root -l -q -b $NEUROBAYES/external/analysis.C'("ahist.txt","analysis.pdf",1, "correl_signi.txt")'

-----------------------------------------------------------
Running the Expert - using the Expertise :
	We will use this tool to compare the discriminator distribution for the different trains.
	Before to run we need to create test data.
		make expert
		make testdata
		./expert train∗.nb
	It will create evaluationsPlots.root cointaining the TH1F comparisons.
	Have a look at them with the following macro!
		root -l macros/showHistos.C+






Instructions:
  g++ macros/CreateOptionsFile.cpp	#create options list
  ./a.out
  python macros/runTraining.py		#create a Job to run the trining with all the options
  make trainHtt				#create executable to train the MVA
  sh Job.sh				#run NeuroBayesMVA with all different options
  python macros/producePlots.py		#produce analysis.pdf plots
  ./test results/trainHtt_*expertise*   #create results/HttEvaluationsPlots.root 





-----------------------------------------------------------
Questions:
	in HowToCpp.pdf 
	1. where is produced the The Teacher text output? in nb_teacher_train1.log
	2. correl_signi_cmap method? prodotto in src/train2.cc
	3. what is the Expert? Testing data after training.



activate
