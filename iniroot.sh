#!bin/zsh

source /cvmfs/sft.cern.ch/lcg/external/gcc/4.8.4/x86_64-slc6-gcc48-opt/setup.sh
source /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/5.34.36/x86_64-slc6-gcc48-opt/root/bin/thisroot.sh

cp -r /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/5.34.36/x86_64-slc6-gcc48-opt/root/tmva ./
mv job_submission.py tmva/test/.
mv HH_TMVAClassification.C tmva/test/.

