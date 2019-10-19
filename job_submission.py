import os
import sys
#from ROOT import *
import random
import time
import datetime


def main(argv):
	channel = ''
	resType = ''

	for x in range(len(sys.argv)):
		if sys.argv[x] == '--resType':
			resType = sys.argv[x+1]
		if sys.argv[x] == '--channel':
			channel = sys.argv[x+1]

	doSub = False
	if '--launch' in sys.argv:
		doSub = True


	###--------------------------------------------------------------
	###        Set these Options Before Running					  ###
	###--------------------------------------------------------------

	# input directory for trees
	inputDirect = '/eos/cms/store/group/phys_higgs/HiggsExo/HH_bbZZ_bbllqq/dmorse/hhTrees/NTupleAnalyzerHH_hh_newEleBDTs_QuickTest_2018_07_07/SummaryFiles/'

	# combine low mass signals? 'YES' or 'NO'
	#doLowMComb = 'YES'
	# combine neighboring signal mass point? 'YES' or 'NO'
	doNeighbor = 'YES'

	# name of the Script to run >> HH_TMVAClassification.C
	csname = 'HH_TMVAClassification.C'

	# working directory name
	workdir = '/afs/cern.ch/work/a/ahortian/HH_bdt_tmva_test/'
	# output dir on the /store/user/ area
	out_eosdir = '/eos/cms/store/user/ahortian/HH_MVA_Batch/'

	#SignalM = ['260'] # use this for doLowMComb
	SignalM = ["260","270","300","350","400", "450", "500", "550", "600","650", "750", "800", "900", "1000"]
	#SignalM = ["900"]

	#myMethodList = ['BDT']
	#myMethodList = ['BDT,BDTG']
	myMethodList = ['BDTG01']

	###--------------------------------------------------------------
	repos_dir = 'HH_bdt_tmva'
	workdir = workdir + repos_dir + '/'

	sig_usage = {
		'260': { 'dn':'260', 'cn':'270', 'up':'300' },
		'270': { 'dn':'260', 'cn':'270', 'up':'300' },
		'300': { 'dn':'270', 'cn':'300', 'up':'350' },
		'350': { 'dn':'300', 'cn':'350', 'up':'400' },
		'400': { 'dn':'350', 'cn':'400', 'up':'450' },
		'450': { 'dn':'400', 'cn':'450', 'up':'500' },
		'500': { 'dn':'450', 'cn':'500', 'up':'550' },
		'550': { 'dn':'500', 'cn':'550', 'up':'600' },
		'600': { 'dn':'550', 'cn':'600', 'up':'650' },
		'650': { 'dn':'600', 'cn':'650', 'up':'750' },
		'750': { 'dn':'650', 'cn':'750', 'up':'800' },
		'800': { 'dn':'750', 'cn':'800', 'up':'900' },
		'900': { 'dn':'800', 'cn':'900', 'up':'1000' },
	   '1000': { 'dn':'800', 'cn':'900', 'up':'1000' },
	}

	strSigDn = ''
	strSigCn = ''
	strSigUp = ''

	dateTo = datetime.datetime.now().strftime("%Y_%m_%d_%H%M%S")
	# TODO: how to change the name of the weights dir in .C?


	##---------------------------------------------------------##
	##      Input Variables Used in Training 				   ##
	##---------------------------------------------------------##
	[variable_name, var_func_name] = getvariables(channel)

	##-----------------------------------------------------------


	# -----------------------------------------------------------
	# the following are vars to be added in N+1 test
	# -----------------------------------------------------------
	add_var_name =[""] # need at leat this one
	add_varfunc_name =[""] # need at leat this one
	# -----------------------------------------------------------
	# -----------------------------------------------------------
	# the following are vars to be excluded in N-1 test
	# -----------------------------------------------------------
	excludeVar = [""] # need at leat this one
	###				"DR_jj_Z",
	###				"DR_ele1ele2",
	# -----------------------------------------------------------

	variables_name_list_init = ""
	for i in range(len(variable_name)):
		variables_name_list_init += variable_name[i]
		if i < len(variable_name) -1 :variables_name_list_init += ','
	#print ' variables_name_list_init : ', variables_name_list_init

	vars_func_name_list_init = ""
	for i in range(len(var_func_name)):
		vars_func_name_list_init += var_func_name[i]
		if i < len(var_func_name) -1 :vars_func_name_list_init += ','
	vars_func_name_list_init = vars_func_name_list_init.replace("(", "\(")
	vars_func_name_list_init = vars_func_name_list_init.replace(")", "\)")
	#print ' vars_func_name_list_init : ', vars_func_name_list_init



	##-------- Setting config parameters for BTD AdaBoost ----------------------------------
	##-------- PAS version uses GradBoost instead of AdaBoost so this is not needed,
	##-------- i.e. set all param to ''
	##--------------------------------------------------------------------------------------
	NTrees = []
	MinNodeSize = []
	MaxDepth = []
	AdaBoostBeta = []
	SeparationType = []
	nCuts = []

	NTrees += ['']
	MinNodeSize += ['']
	MaxDepth += ['']
	AdaBoostBeta += ['']
	SeparationType += ['']
	nCuts += ['']

	# this is default config
	#NTrees += ['850']
	#MinNodeSize += ['2.5%']
	#MaxDepth += ['3']
	#AdaBoostBeta += ['0.5']
	#SeparationType += ['GiniIndex']
	#nCuts += ['20']

	#NTrees += ['850']
	#MinNodeSize += ['4.0%']
	#MaxDepth += ['3']
	#AdaBoostBeta += ['0.5']
	#SeparationType += ['GiniIndex']
	#nCuts += ['20']
	#
	#NTrees += ['850']
	#MinNodeSize += ['2.5%']
	#MaxDepth += ['3']
	#AdaBoostBeta += ['0.35']
	#SeparationType += ['GiniIndex']
	#nCuts += ['20']

	## this is the one that optimized M550
	#NTrees += ['850']
	#MinNodeSize += ['4.0%']
	#MaxDepth += ['3']
	#AdaBoostBeta += ['0.35']
	#SeparationType += ['GiniIndex']
	#nCuts += ['20']

#	NTrees += ['850']
#	MinNodeSize += ['5.0%']
#	MaxDepth += ['2']
#	AdaBoostBeta += ['0.35']
#	SeparationType += ['GiniIndex']
#	nCuts += ['20']

	##----------------------------------

	myOutRootDir = 'TMVA_outputRoot' + '_' + str(dateTo)
	myOutWgthDir = 'weights' + '_' + str(dateTo)
	mtmpdir = 'tmpjobs_' + str(dateTo)
	if doSub :
		os.system ('eos mkdir -p ' + out_eosdir)
		os.system ('eos mkdir -p ' + out_eosdir + '/' + myOutRootDir)
		os.system ('eos mkdir -p ' + out_eosdir + '/' + myOutWgthDir)
		os.system('mkdir ' + mtmpdir)
		os.system('mkdir weights')


	for sigmass in range(len(SignalM)) :
		
		if (doNeighbor == 'YES'):
			strSigDn = sig_usage[SignalM[sigmass]]['dn']
			strSigCn = sig_usage[SignalM[sigmass]]['cn']
			strSigUp = sig_usage[SignalM[sigmass]]['up']

		tagNameOpt = []
		for nopt in range(0, len(NTrees)):
			this_opt = 'NTrees'+NTrees[nopt]+'_'+'MinNodeSize'+MinNodeSize[nopt]+'_'+'MaxDepth'+MaxDepth[nopt]+'_'+'AdaBeta'+AdaBoostBeta[nopt]+'_'+'SepType'+SeparationType[nopt]+'_'+'nCuts'+nCuts[nopt]
			this_opt = this_opt.replace('.', 'p')
			this_opt = this_opt.replace('%', '')
			this_opt = this_opt.replace('_SepTypeGiniIndex', '')
			this_opt += '_M' + str(SignalM[sigmass])
			##print 'this_opt', this_opt
			tagNameOpt += [str(this_opt)]
		print ' tagNameOpt: ', tagNameOpt

		job = '#!/bin/bash\n'
		job += 'cd ' + workdir + '\n'
		job += 'source ' + workdir + 'iniroot.sh\n'
		job += 'cd ' + workdir + '/tmva/test\n'
		#job += 'cd ' + workdir + '/TMVA-v4.2.0/test\n'
		#job += 'source setup.sh /afs/cern.ch/user/a/ahortian/HH_WORK_Jul13/TMVA-v4.2.0\n'
		##print job


		methodTag = myMethodList[0]
		methodTag = methodTag.replace(',', '_')

		jind = 0
		for inp1 in range(len(add_var_name)):
			if not (add_var_name[inp1] == ''):
				variables_name_list = variables_name_list_init + ',' + add_var_name[inp1]
				vars_func_name_list = vars_func_name_list_init + ',' + add_varfunc_name[inp1].replace("(", "\(").replace(")", "\)")
			else:
				variables_name_list = variables_name_list_init
				vars_func_name_list = vars_func_name_list_init
			#print ' after adding variables_name_list : ', variables_name_list
			#print ' after adding vars_func_name_list : ', vars_func_name_list

			for tag in excludeVar:
				for opt in range(0, len(tagNameOpt)):
					
					timeTag = methodTag + '_' + resType + '_' + channel + '_' + tagNameOpt[opt] + '_' + str(dateTo)
				
					myOutputRootFile = out_eosdir + '/' + myOutRootDir + '/TMVA_' + tag + 'np1_' + add_var_name[inp1] + '_' + timeTag + '.root'
					
					com = 'root -b -q ' + csname + '\(\\\"' + myMethodList[0] + '\\\",\\\"' + myOutputRootFile + '\\\",\\\"' + tag + 'np1_' + add_var_name[inp1] + '\\\",\\\"' + timeTag + '\\\",\\\"' + str(NTrees[opt]) + '\\\",\\\"' + str(MinNodeSize[opt]) + '\\\",\\\"' + str(MaxDepth[opt]) + '\\\",\\\"' + str(AdaBoostBeta[opt]) + '\\\",\\\"' + str(SeparationType[opt]) + '\\\",\\\"' + str(nCuts[opt]) + '\\\",\\\"' + str(SignalM[sigmass]) + '\\\",\\\"' + variables_name_list + '\\\",\\\"' + vars_func_name_list + '\\\",\\\"' + inputDirect + '\\\",\\\"' + channel + '\\\",\\\"' + resType + '\\\",\\\"' + doNeighbor + '\\\",\\\"' + strSigDn + '\\\",\\\"' + strSigCn + '\\\",\\\"' + strSigUp + '\\\"\)'

					com +='\n\n'
					print '\n ... going to submit run command ==> ', com
					
					
					comxrdcp_wght = 'cp ' + workdir + '/tmva/test/weights/TMVAClassification_' + tag + 'np1_' + add_var_name[inp1] + '_' + timeTag + '*.*.*' + ' ' + out_eosdir + '/' + myOutWgthDir + '/.\n\n'
					comrm_wght =  'rm ' + workdir + '/tmva/test/weights/TMVAClassification_' + tag + 'np1_' + add_var_name[inp1] + '_' + timeTag + '*.*.*\n\n'
					
					jind += 1
					ajob = str(job)
					tjobname     = mtmpdir+'/job_' + tag + 'np1_' + add_var_name[inp1] + '_' + timeTag + '_job_' + str(jind) + '.sh'
					tjobname_out = mtmpdir+'/job_' + tag + 'np1_' + add_var_name[inp1] + '_' + timeTag + '_job_' + str(jind) + '.out'
					tjobname_err = mtmpdir+'/job_' + tag + 'np1_' + add_var_name[inp1] + '_' + timeTag + '_job_' + str(jind) + '.err'
					tjobname_log = mtmpdir+'/job_' + tag + 'np1_' + add_var_name[inp1] + '_' + timeTag + '_job_' + str(jind) + '.log'
					
					cdjobname = 'job_' + tag + 'np1_' + add_var_name[inp1] + '_' + timeTag + '_job_' + str(jind) + '.sub'
					cjob_to_write =  'executable  = ' + tjobname           + '\n'
					cjob_to_write += 'arguments   = $(ClusterId)$(ProcId)' + '\n'
					cjob_to_write += 'output      = ' + tjobname_out       + '\n'
					cjob_to_write += 'error       = ' + tjobname_err       + '\n'
					cjob_to_write += 'log         = ' + tjobname_log       + '\n'
					#cjob_to_write += '+JobFlavour = "workday"'             + '\n'
					cjob_to_write += '+JobFlavour = "tomorrow"'             + '\n'
					cjob_to_write += 'queue \n'

					if doSub :
						tjob = open(tjobname,'w')
						tjob.write(ajob+'\n\n')
						tjob.write(com)
						tjob.write(comxrdcp_wght)
						tjob.write(comrm_wght)
						tjob.close()
						os.system('chmod 755 '+tjobname)
					
						cjob = open(cdjobname,'w')
						cjob.write(cjob_to_write)
						cjob.close()
					else :
						print ' tjobname      :' , tjobname
						#print ' ajob          :' , ajob
						print ' com           :' , com
						print ' comxrdcp_wght :' , comxrdcp_wght
						print ' comrm_wght    :' , comrm_wght


					##	bsub = 'bsub -R "pool>15000" -q '+batchq+' -o /dev/null -e /dev/null -J '+tjobname+' < '+tjobname +' '
					#bsub = 'bsub -q 1nd -o ' +tjobname_out+ ' -e ' +tjobname_err+ ' -J ' +  tjobname + ' < ' + tjobname + ' '
					#print '\n', bsub, '\n\n'
					
					condor_sub = 'condor_submit ' + cdjobname
					print condor_sub + '\n'
					if doSub :
						os.system(condor_sub)
						os.system('sleep 1')



def getvariables(selectedchannel):
	## The order of these variables matter, need to be the same for all 4 lists:
	## variable_name_ee, var_func_name_ee, variable_name_uu, and var_func_name_uu
	
	variable_name_ee =[]
	variable_name_ee +=["Mbb_H","DR_bb_H"]
	variable_name_ee +=["Mjj_Z","DR_jj_Z"]
	variable_name_ee +=["M_ee","DR_ele1ele2"]
	variable_name_ee +=["DR_ee_bb_H"]
	variable_name_ee +=["DR_e1Hj1","DR_e1Hj2","DR_e2Hj1","DR_e2Hj2"]
	variable_name_ee +=["DR_ee_jj_Z"]
	variable_name_ee +=["DR_e1Zj1","DR_e1Zj2","DR_e2Zj1","DR_e2Zj2"]
	variable_name_ee +=["cosThetaStarEle","cosTheta_hbb_ee","cosTheta_zee_hzz"]
	variable_name_ee +=["DPhi_ele1met"]
	variable_name_ee +=["phi1_ee","phi1_zjj_ee"]
	
	var_func_name_ee =[]
	var_func_name_ee +=["Mbb_H","DR_bb_H"]
	var_func_name_ee +=["Mjj_Z","DR_jj_Z"]
	var_func_name_ee +=["M_ee","DR_ele1ele2"]
	var_func_name_ee +=["DR_ee_bb_H"]
	var_func_name_ee +=["DR_e1Hj1","DR_e1Hj2","DR_e2Hj1","DR_e2Hj2"]
	var_func_name_ee +=["DR_ee_jj_Z"]
	var_func_name_ee +=["DR_e1Zj1","DR_e1Zj2","DR_e2Zj1","DR_e2Zj2"]
	var_func_name_ee +=["abs(cosThetaStarEle)","abs(cosTheta_hbb_ee)","abs(cosTheta_zee_hzz)"]
	var_func_name_ee +=["abs(DPhi_ele1met)"]
	var_func_name_ee +=["abs(phi1_ee)","abs(phi1_zjj_ee)"]
	
	# total of 50 vars
	variable_name_uu =[]
	variable_name_uu +=["Mbb_H","DR_bb_H"]
	variable_name_uu +=["Mjj_Z","DR_jj_Z"]
	variable_name_uu +=["M_uu","DR_muon1muon2"]
	variable_name_uu +=["DR_uu_bb_H"]
	variable_name_uu +=["DR_u1Hj1","DR_u1Hj2","DR_u2Hj1","DR_u2Hj2"]
	variable_name_uu +=["DR_uu_jj_Z"]
	variable_name_uu +=["DR_u1Zj1","DR_u1Zj2","DR_u2Zj1","DR_u2Zj2"]
	variable_name_uu +=["cosThetaStarMu","cosTheta_hbb_uu","cosTheta_zuu_hzz"]
	variable_name_uu +=["DPhi_muon1met"]
	variable_name_uu +=["phi1_uu","phi1_zjj_uu"]
	
	var_func_name_uu =[]
	var_func_name_uu +=["Mbb_H","DR_bb_H"]
	var_func_name_uu +=["Mjj_Z","DR_jj_Z"]
	var_func_name_uu +=["M_uu","DR_muon1muon2"]
	var_func_name_uu +=["DR_uu_bb_H"]
	var_func_name_uu +=["DR_u1Hj1","DR_u1Hj2","DR_u2Hj1","DR_u2Hj2"]
	var_func_name_uu +=["DR_uu_jj_Z"]
	var_func_name_uu +=["DR_u1Zj1","DR_u1Zj2","DR_u2Zj1","DR_u2Zj2"]
	var_func_name_uu +=["abs(cosThetaStarMu)","abs(cosTheta_hbb_uu)","abs(cosTheta_zuu_hzz)"]
	var_func_name_uu +=["abs(DPhi_muon1met)"]
	var_func_name_uu +=["abs(phi1_uu)","abs(phi1_zjj_uu)"]
	
	if selectedchannel == 'ee' :
		thisvaraiblename = variable_name_ee[:]
		thisvarfunc_name = var_func_name_ee[:]
	if selectedchannel == 'uu' :
		thisvaraiblename = variable_name_uu[:]
		thisvarfunc_name = var_func_name_uu[:]
	
	return [thisvaraiblename,thisvarfunc_name]


# This part is to execute the function 'main'
if __name__ == "__main__":
	main(sys.argv[1:])

