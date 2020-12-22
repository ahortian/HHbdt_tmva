# HHbdt_tmva
- Boosted Decision Trees Classifier for diHiggs searches in bbZ(ll)Z(jj) channel
- The software used is the Toolkit for Multivariate Data Analysis with ROOT (TMVA)

		- now using version that is included in ROOT release 5.34.36 
		- will need to update to the latest version with ROOT6

## Instruction
#### (1) Set up
	git clone git@github.com:ahortian/HHbdt_tmva.git
	source iniroot.sh
	cd tmva/test

#### (2) Run for training
	python job_submission.py --resType HHres --channel uu --launch

#### (3) Options
	--resType : HHres, HHBulkGrav
	--channel : uu, ee
	--launch  : this demands the program to submit jobs to condor

#### (4) Other hard-coded variables need to be customized
	inputDirect : directory containing input ROOT:TTree files 
	workdir     : current working directory in which this repository is cloned 
	out_eosdir  : directory where output files will be stored
	myMethodList : currently uses ['BDTG01'] which is the Gradient Boosted DT. To compare several methods, use e.g. ['BDT,BDTG'] 

#### (5) Investigate output by running
	root -l TMVAGui.C\(\"your_output_file.root\"\)
	
#### (6) Make plots without using GUI
	cd ploting_pack
	python plot_tmva.py -o plot_sp0_ee -i 2019_01_16_213101
	
	# Here the plot_sp0_ee is the directory the created plots will be in. 
	# The 2019_01_16_213101 is the timestamp of the condor jobs you have run.
