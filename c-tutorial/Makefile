ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLDFLAGS   := $(shell root-config --ldflags)
ROOTLIBS     := $(shell root-config --libs)

########################################################################################
# explicitly written these variables are:
# ROOTCFLAGS   := -pthread -I/usr/local/cern/root//include
# ROOTLIBS     := -L/usr/local/cern/root//lib -lCore -lCint -lHist \
#	-lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix \
#	-lPhysics -pthread -lm -ldl -rdynamic
########################################################################################

# set the compiler options
CXX           = g++
CXXFLAGS      = -g -O  -fPIC -Wall #set compiler options
                                   #-g compile with debug information
                                   #-O optimize
                                   #-Wall show warnings for everything
                                   #-fPIC compile source file as sharable object                                              
CXXOUT	    = -o 				   
# set the linker options
LD            = gcc
LDFLAGS       = -O $(ROOTLDFLAGS)
SOFLAGS       = -shared

#NeuroBayes (R) libraries
NBLIBS        =  -L$(NEUROBAYES)/lib 

#######################
CXXFLAGS      += $(ROOTCFLAGS) -I. -I$(NEUROBAYES)/include -Iinc 
LIBS          = $(ROOTLIBS) $(NBLIBS) -lNeuroBayesTeacherCPP 

default : train_simplest train1 train2  

clean:
	@rm -rf src/*.o train* expert

src/%.o: src/%.cc
	@$(CXX) $(CXXFLAGS) $(CXXOUT)$@ -c $<

train%: src/train%.o  
	@$(LD) $(LDFLAGS) $^ $(LIBS) -o $@
	
expert: src/expert.cc	
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(ROOTLIBS) $(NBLIBS) -lNeuroBayesExpertCPP $(CXXOUT)$@  $<

totalyclean:
	rm -rf train1_expertise.nb train2_expertise.nb train2_iter_expertise.nb rescue.nb teacherHistos.root
	rm -rf ahist.txt  analysis.ps cmaplog.txt cmaplog2.txt correl_signi.* nb_teacher.log 
	rm -rf evaluationsPlots.root
	rm -rf */*.d */*.so
	rm -rf data/*.root

trainingdata: 
	@root -l -q data/gen_trainfiles.cc+
	@mv Trainings_Data.root data

testdata: 
	@root -l -q data/gen_trainfiles.cc+
	@mv Trainings_Data.root data/testData.root

runall: trainingdata train_simplest train1 train2
	./train_simplest
	./train1 
	./train2
	./train2 1
