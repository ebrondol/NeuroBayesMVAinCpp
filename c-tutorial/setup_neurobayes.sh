#!/bin/bash
export NEUROBAYES=/afs/cern.ch/user/g/gedum/public/NeuroBayes/software
export HOST=`hostname`

export PHIT_LICENCE_PATH=/afs/cern.ch/user/g/gedum/public/NeuroBayes/licence

export LD_LIBRARY_PATH=$NEUROBAYES/lib:$LD_LIBRARY_PATH
export PATH=$NEUROBAYES/external:$PATH

