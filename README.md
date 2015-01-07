Instruction for NeuroBayes MVA

Follow C/C++ Tutorial @ http://neurobayes.phi-t.de/index.php/tutorials/cc

-----------------------------------------------------------
First run - Basic setup :
	doc/ contains all the documents needed for the analysis (in particular look @ HowToCpp.pdf and NeuroBayes-HowTo.pdf)
	running the analysis tools with:
		root -l -q -b $NEUROBAYES/external/asciiToRoot.C
		root -l -q -b $NEUROBAYES/external/analysis.C'("ahist.txt","analysis.pdf",1)'


-----------------------------------------------------------
Second Run - global PreproFlag and BFGS :
	- qual è la differenza tra i due pdf di output?

-----------------------------------------------------------
Third run - individual preprocessing flags and zero iterations :

-----------------------------------------------------------
Questions:
	in HowToCpp.pdf 
	1. where is produced the The Teacher text output? in nb_teacher_train1.log
	2. correl_signi_cmap method? prodotto in src/train2.cc
	3. what is the Expert?


