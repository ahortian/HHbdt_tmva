#from ROOT import *
import os
import sys
import time
import datetime

##---------- command ----------##
# python plot_tmva.py -o plot_sp2_uu_BDT_BDTG -i 2019_02_15_222355
# python plot_tmva.py -o plot_sp2_ee_BDT_BDTG -i 2019_02_15_221357
##----------------------------------------

#root -l TMVAGui.C\(\"/eos/cms/store/user/ahortian/HH_MVA_Batch/TMVA_outputRoot_2019_02_27_033648_test/TMVA_np1__BDTG_HHres_uu_NTrees850_MinNodeSize5p0_MaxDepth2_AdaBeta0p35_nCuts20_M1000_2019_02_27_033648.root\"\)
#
#root -b -q correlationscatters.C\(\"/eos/cms/store/user/ahortian/HH_MVA_Batch/TMVA_outputRoot_2019_02_27_033648_test/TMVA_np1__BDTG_HHres_uu_NTrees850_MinNodeSize5p0_MaxDepth2_AdaBeta0p35_nCuts20_M1000_2019_02_27_033648.root\",\"DR_u1Hj2\",\"InputVariables_Id\"\)
#
#root -b -q BDTControlPlots.C\(\"/eos/cms/store/user/ahortian/HH_MVA_Batch/TMVA_outputRoot_2019_02_27_083838/TMVA_np1__BDTG_BDTG01_BDTG02_BDTG03_BDTG04_BDTG05_BDTG06_BDTG07_BDTG08_BDTG09_HHres_uu_NTrees850_MinNodeSize5p0_MaxDepth2_AdaBeta0p35_nCuts20_M400_2019_02_27_083838.root\"\)
#
##----------------------------------------

#ROOT.gStyle.SetOptStat(0)
#ROOT.gROOT.SetBatch(1) # no window pop up when drawing

inftag = ''
outdir = ''
domass = ''
for x in range(len(sys.argv)):
	if sys.argv[x] == '-i':
		inftag = sys.argv[x+1]
	if sys.argv[x] == '-o':
		outdir = sys.argv[x+1]
	if sys.argv[x] == '-m':
		domass = sys.argv[x+1]

_mass = ["260","270","300","350","400", "450", "500",  "550",  "600",  "650", "750", "800", "900", "1000"]

chan = 'uu'
if (outdir.find('ee') >= 0):
	chan = 'ee'

tagbdtg = 'BDT_'
if (outdir.find('BDT_BDTG') >= 0):
	tagbdtg = 'BDT_BDTG_'
elif (outdir.find('BDTG') >= 0):
	tagbdtg = 'BDTG_'

resType = 'HHres'
if (outdir.find('sp2') >= 0):
	resType = 'HHBulkGrav'

for im in range(len(_mass)):
	if domass != '':
		if _mass[im] != domass: continue
	out_directory = outdir + _mass[im]
	os.system('mkdir ' + out_directory)

	inf = '/eos/cms/store/user/ahortian/HH_MVA_Batch/TMVA_outputRoot_' + inftag + '/'
	#inf += 'TMVA_np1__BDT_HHres_' + chan+ '_NTrees850_MinNodeSize4p0_MaxDepth3_AdaBeta0p35_nCuts20_M' + _mass[im] + '_' + inftag + '.root'
	#inf += 'TMVA_np1__' + tagbdtg + resType + '_'+ chan+ '_NTrees850_MinNodeSize4p0_MaxDepth3_AdaBeta0p35_nCuts20_M' + _mass[im] + '_' + inftag + '.root'
	#inf += 'TMVA_np1__' + tagbdtg + resType + '_'+ chan+ '_NTrees850_MinNodeSize5p0_MaxDepth2_AdaBeta0p35_nCuts20_M' + _mass[im] + '_' + inftag + '.root'
	#inf += 'TMVA_np1__BDTG_BDTG01_' + resType + '_'+ chan+ '_NTrees850_MinNodeSize5p0_MaxDepth2_AdaBeta0p35_nCuts20_M' + _mass[im] + '_' + inftag + '.root'
	#inf += 'TMVA_np1__BDTG_BDTG01_' + resType + '_'+ chan+ '_NTrees_MinNodeSize_MaxDepth_AdaBeta_SepType_nCuts_M' + _mass[im] + '_' + inftag + '.root'
	inf += 'TMVA_np1__BDTG_' + resType + '_'+ chan+ '_NTrees_MinNodeSize_MaxDepth_AdaBeta_SepType_nCuts_M' + _mass[im] + '_' + inftag + '.root'



	# Input variables (training sample)
	com_variables = ' root -b -q myvariables.C\(\\"' + inf + '\\",\\"' + out_directory +'\\"\) '
	print ' running command ', com_variables
	os.system(com_variables)

	# Input Variable Linear Correlation Coefficients
	com_linear_corr = ' root -b -q mycorrelations.C\(\\"' + inf + '\\",\\"' + out_directory +'\\"\) '
	print ' running command ', com_linear_corr
	os.system(com_linear_corr)


	# Classifier Output Distributions (test and training samples superimposed)
		# make sure I add code for removing negative bins
	com_mvas = ' root -b -q mymvas.C\(\\"' + inf + '\\",\\"' + out_directory +'\\"\) '
	print ' running command ', com_mvas
	os.system(com_mvas)

	# Classifier Background Rejection vs Signal Efficiency (ROC curve)
	com_efficiencies = ' root -b -q myefficiencies.C\(\\"' + inf + '\\",\\"' + out_directory +'\\"\) '
	print ' running command ', com_efficiencies
	os.system(com_efficiencies)


# Next read ROC values from OUT files
