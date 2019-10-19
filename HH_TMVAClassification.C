// @(#)root/tmva $Id$
/**********************************************************************************
 * Project   : TMVA - a ROOT-integrated toolkit for multivariate data analysis    *
 * Package   : TMVA                                                               *
 * Root Macro: TMVAClassification                                                 *
 *                                                                                *
 * This macro provides examples for the training and testing of the               *
 * TMVA classifiers.                                                              *
 *                                                                                *
 * As input data is used a toy-MC sample consisting of four Gaussian-distributed  *
 * and linearly correlated input variables.                                       *
 *                                                                                *
 * The methods to be used can be switched on and off by means of booleans, or     *
 * via the prompt command, for example:                                           *
 *                                                                                *
 *    root -l ./TMVAClassification.C\(\"Fisher,Likelihood\"\)                     *
 *                                                                                *
 * (note that the backslashes are mandatory)                                      *
 * If no method given, a default set of classifiers is used.                      *
 *                                                                                *
 * The output file "TMVA.root" can be analysed with the use of dedicated          *
 * macros (simply say: root -l <macro.C>), which can be conveniently              *
 * invoked through a GUI that will appear at the end of the run of this macro.    *
 * Launch the GUI via the command:                                                *
 *                                                                                *
 *    root -l ./TMVAGui.C                                                         *
 *                                                                                *
 **********************************************************************************/

#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"

#include "TMVAGui.C"

#if not defined(__CINT__) || defined(__MAKECINT__)
// needs to be included when makecint runs (ACLIC)
#include "TMVA/Factory.h"
#include "TMVA/Tools.h"
#endif

void HH_TMVAClassification( TString myMethodList = "", TString myOutputRootFile = "", TString excVarTag = "", TString timeTag = "",
						TString NTreesStr = "", TString MinNodeSizeStr = "", TString MaxDepthStr = "", TString AdaBoostBetaStr = "", TString SeparationTypeStr = "", TString nCutsStr = "",
						TString SignalM = "",
						TString variables_name_list = "", TString vars_func_name_list = "",
						TString inputDirect = "",
						TString channel = "",
						TString resType = "",
						TString doNeighbor = "",
						TString strSigDn = "",
						TString strSigCn = "",
						TString strSigUp = ""
						)
{
   // The explicit loading of the shared libTMVA is done in TMVAlogon.C, defined in .rootrc
   // if you use your private .rootrc, or run from a different directory, please copy the
   // corresponding lines from .rootrc

   // methods to be processed can be given as an argument; use format:
   //
   // mylinux~> root -l TMVAClassification.C\(\"myMethod1,myMethod2,myMethod3\"\)
   //
   // if you like to use a method via the plugin mechanism, we recommend using
   //
   // mylinux~> root -l TMVAClassification.C\(\"P_myMethod\"\)
   // (an example is given for using the BDT as plugin (see below),
   // but of course the real application is when you write your own
   // method based)

   //---------------------------------------------------------------
   // This loads the library
   TMVA::Tools::Instance();
   // increase the limit on the maximum operation
   TFormula::SetMaxima(10000,1000,10000);
   // increase the # of variables limit allowed to make scatter plots (default is 20)
   (TMVA::gConfig().GetVariablePlotting()).fMaxNumOfAllowedVariablesForScatterPlots = 30;

   // Default MVA methods to be trained + tested
   std::map<std::string,int> Use;

   // --- Cut optimisation
   Use["Cuts"]            = 1;
   Use["CutsD"]           = 1;
   Use["CutsPCA"]         = 0;
   Use["CutsGA"]          = 0;
   Use["CutsSA"]          = 0;
   // 
   // --- 1-dimensional likelihood ("naive Bayes estimator")
   Use["Likelihood"]      = 1;
   Use["LikelihoodD"]     = 0; // the "D" extension indicates decorrelated input variables (see option strings)
   Use["LikelihoodPCA"]   = 1; // the "PCA" extension indicates PCA-transformed input variables (see option strings)
   Use["LikelihoodKDE"]   = 0;
   Use["LikelihoodMIX"]   = 0;
   //
   // --- Mutidimensional likelihood and Nearest-Neighbour methods
   Use["PDERS"]           = 1;
   Use["PDERSD"]          = 0;
   Use["PDERSPCA"]        = 0;
   Use["PDEFoam"]         = 1;
   Use["PDEFoamBoost"]    = 0; // uses generalised MVA method boosting
   Use["KNN"]             = 1; // k-nearest neighbour method
   //
   // --- Linear Discriminant Analysis
   Use["LD"]              = 1; // Linear Discriminant identical to Fisher
   Use["Fisher"]          = 0;
   Use["FisherG"]         = 0;
   Use["BoostedFisher"]   = 0; // uses generalised MVA method boosting
   Use["HMatrix"]         = 0;
   //
   // --- Function Discriminant analysis
   Use["FDA_GA"]          = 1; // minimisation of user-defined function using Genetics Algorithm
   Use["FDA_SA"]          = 0;
   Use["FDA_MC"]          = 0;
   Use["FDA_MT"]          = 0;
   Use["FDA_GAMT"]        = 0;
   Use["FDA_MCMT"]        = 0;
   //
   // --- Neural Networks (all are feed-forward Multilayer Perceptrons)
   Use["MLP"]             = 0; // Recommended ANN
   Use["MLPBFGS"]         = 0; // Recommended ANN with optional training method
   Use["MLPBNN"]          = 1; // Recommended ANN with BFGS training method and bayesian regulator
   Use["CFMlpANN"]        = 0; // Depreciated ANN from ALEPH
   Use["TMlpANN"]         = 0; // ROOT's own ANN
   //
   // --- Support Vector Machine 
   Use["SVM"]             = 1;
   // 
   // --- Boosted Decision Trees
   Use["BDT"]             = 1; // uses Adaptive Boost
	Use["BDT01"]             = 0; // uses Adaptive Boost
	Use["BDT02"]             = 0; // uses Adaptive Boost
	Use["BDT03"]             = 0; // uses Adaptive Boost
	Use["BDT04"]             = 0; // uses Adaptive Boost
	Use["BDT05"]             = 0; // uses Adaptive Boost
	
   Use["BDTG"]            = 0; // uses Gradient Boost
	Use["BDTG01"]            = 0; // uses Gradient Boost
	Use["BDTG02"]            = 0; // uses Gradient Boost
	Use["BDTG03"]            = 0; // uses Gradient Boost
	Use["BDTG04"]            = 0; // uses Gradient Boost
	Use["BDTG05"]            = 0; // uses Gradient Boost
	Use["BDTG06"]            = 0;
	Use["BDTG07"]            = 0;
	Use["BDTG08"]            = 0;
	Use["BDTG09"]            = 0;
	
   Use["BDTB"]            = 0; // uses Bagging
   Use["BDTD"]            = 0; // decorrelation + Adaptive Boost
   Use["BDTF"]            = 0; // allow usage of fisher discriminant for node splitting 
   // 
   // --- Friedman's RuleFit method, ie, an optimised series of cuts ("rules")
   Use["RuleFit"]         = 1;
   // ---------------------------------------------------------------

   bool isSigMComb = false;
   if (doNeighbor == "YES") isSigMComb = true;
   std::cout << std::endl;
   std::cout << "==> Start TMVAClassification"      << std::endl;
   std::cout << "          channel: " << channel    << std::endl;
   std::cout << "          resType: " << resType    << std::endl;
   std::cout << "      doNeighbor?: " << doNeighbor << " " << isSigMComb << std::endl;

   // Select methods (don't look at this code - not of interest)
   if (myMethodList != "") {
      for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) it->second = 0;

      std::vector<TString> mlist = TMVA::gTools().SplitString( myMethodList, ',' );
      for (UInt_t i=0; i<mlist.size(); i++) {
         std::string regMethod(mlist[i]);

         if (Use.find(regMethod) == Use.end()) {
            std::cout << "Method \"" << regMethod << "\" not known in TMVA under this name. Choose among the following:" << std::endl;
            for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) std::cout << it->first << " ";
            std::cout << std::endl;
            return;
         }
         Use[regMethod] = 1;
      }
   }
	
	NTreesStr = ":NTrees=" + NTreesStr;
	MinNodeSizeStr = ":MinNodeSize=" + MinNodeSizeStr;
	MaxDepthStr = ":MaxDepth=" + MaxDepthStr;
	AdaBoostBetaStr = ":AdaBoostBeta=" + AdaBoostBetaStr;
	SeparationTypeStr = ":SeparationType=" + SeparationTypeStr;
	nCutsStr = ":nCuts=" + nCutsStr;
	
	TString ParaOptStr = "!H:!V" + NTreesStr + MinNodeSizeStr + MaxDepthStr + ":BoostType=AdaBoost:DoBoostMonitor" + AdaBoostBetaStr + SeparationTypeStr + nCutsStr;
	std::cout << " BDT option : " << ParaOptStr << std::endl;
	

   // --------------------------------------------------------------------------------------------------

   // --- Here the preparation phase begins

   // Create a ROOT output file where TMVA will store ntuples, histograms, etc.
   //TString outfileName( "TMVA.root" );
	//TString outfileName( "/afs/cern.ch/user/a/ahortian/HH_WORK_Jul13/TMVA-v4.2.0/test/TMVA_outputRoot/" );
	TString outfileName("");
	outfileName += myOutputRootFile;
	TFile* outputFile = TFile::Open( outfileName, "RECREATE" );

   // Create the factory object. Later you can choose the methods
   // whose performance you'd like to investigate. The factory is 
   // the only TMVA object you have to interact with
   //
   // The first argument is the base of the name of all the
   // weightfiles in the directory weight/
   //
   // The second argument is the output file for the training results
   // All TMVA output can be suppressed by removing the "!" (not) in
   // front of the "Silent" argument in the option string
   // TMVA::Factory *factory = new TMVA::Factory("MVAanalysis", outputFile, "!V");  // this willl not do Transformations
    //TMVA::Factory *factory = new TMVA::Factory( "TMVAClassification", outputFile,"!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification" );
	TString outWeightName = "TMVAClassification_";
	if (excVarTag != ""){
		outWeightName += excVarTag;
	}
	outWeightName += "_" + timeTag;
	std::cout << "==> output weight filename is: " << outWeightName << " var to be excluded is : " << excVarTag << std::endl;
	TMVA::Factory *factory = new TMVA::Factory( outWeightName, outputFile,
											   "!V:!Silent:Color:DrawProgressBar:Transformations=I:AnalysisType=Classification" );

   // If you wish to modify default settings
   // (please check "src/Config.h" to see all available global options)
   //    (TMVA::gConfig().GetVariablePlotting()).fTimesRMS = 8.0;
   //    (TMVA::gConfig().GetIONames()).fWeightFileDir = "myWeightDirectory";

   // Define the input variables that shall be used for the MVA training
   // note that you may also use variable expressions, such as: "3*var1/var2*abs(var3)"
   // [all types of expressions that can also be parsed by TTree::Draw( "expression" )]
	//   factory->AddVariable( "myvar1 := var1+var2", 'F' );
	//   factory->AddVariable( "myvar2 := var1-var2", "Expression 2", "", 'F' );
	//   factory->AddVariable( "var3",                "Variable 3", "units", 'F' );
	//   factory->AddVariable( "var4",                "Variable 4", "units", 'F' );
	
	std::vector<TString> variable_name = TMVA::gTools().SplitString( variables_name_list, ',' );
	std::vector<TString> var_func_name = TMVA::gTools().SplitString( vars_func_name_list, ',' );
	int nVarUsed = variable_name.size();
	for (int vth=0; vth<nVarUsed; vth++){
		if (variable_name[vth] == excVarTag) continue;
		cout << "  Using variable : " << vth << " " << var_func_name[vth] << "  " << variable_name[vth] << endl;
		factory->AddVariable(var_func_name[vth], variable_name[vth], 'F');
	}
	
   // You can add so-called "Spectator variables", which are not used in the MVA training,
   // but will appear in the final "TestTree" produced by TMVA. This TestTree will contain the
   // input variables, the response values of all trained MVAs, and the spectator variables
   //factory->AddSpectator( "spec1 := Mbb_H",  "Spectator 1", "units", 'F' );
   //factory->AddSpectator( "spec2 := Mbb_H",  "Spectator 2", "units", 'F' );

//   // Read training and test data
//   // (it is also possible to use ASCII format as input -> see TMVA Users Guide)
//   TString fname = "./tmva_class_example.root";
//   
//   if (gSystem->AccessPathName( fname ))  // file does not exist in local directory
//      gSystem->Exec("wget http://root.cern.ch/files/tmva_class_example.root");
//	TFile *input = TFile::Open( fname );

	
	TString fnameBgd_DY = inputDirect + "ZJets.root";
	TString fnameBgd_TT = inputDirect + "TTBar.root";
	TString fnameBgd_WJ = inputDirect + "WJets.root";
	TString fnameBgd_ST = inputDirect + "SingleTop.root";
	TString fnameBgd_SM = inputDirect + "SMHiggs.root";
	TString fnameBgd_DB = inputDirect + "DiBoson.root";
	//TString fnameBgd_QC = "QCDMu.root";
	
	TString fnameSig = inputDirect + resType + SignalM + ".root";
	TString fnameSigDn = inputDirect + resType + strSigDn + ".root";
	TString fnameSigCn = inputDirect + resType + strSigCn + ".root";
	TString fnameSigUp = inputDirect + resType + strSigUp + ".root";

	TFile *inputBgd_DY = TFile::Open( fnameBgd_DY );
	TFile *inputBgd_TT = TFile::Open( fnameBgd_TT );
	TFile *inputBgd_WJ = TFile::Open( fnameBgd_WJ );
	TFile *inputBgd_ST = TFile::Open( fnameBgd_ST );
	TFile *inputBgd_SM = TFile::Open( fnameBgd_SM );
	TFile *inputBgd_DB = TFile::Open( fnameBgd_DB );
	//TFile *inputBgd_QC = TFile::Open( fnameBgd_QC );
	std::cout << "--- TMVAClassification       : Using input background file: " << inputBgd_DY->GetName() << std::endl;
	std::cout << "--- TMVAClassification       : Using input background file: " << inputBgd_TT->GetName() << std::endl;
	std::cout << "--- TMVAClassification       : Using input background file: " << inputBgd_WJ->GetName() << std::endl;
	std::cout << "--- TMVAClassification       : Using input background file: " << inputBgd_ST->GetName() << std::endl;
	std::cout << "--- TMVAClassification       : Using input background file: " << inputBgd_SM->GetName() << std::endl;
	std::cout << "--- TMVAClassification       : Using input background file: " << inputBgd_DB->GetName() << std::endl;
	//std::cout << "--- TMVAClassification       : Using input background file: " << inputBgd_QC->GetName() << std::endl;
	// --- Register the training and test trees
	TTree *backgroundDY = (TTree*)inputBgd_DY->Get("PhysicalVariables");
	TTree *backgroundTT = (TTree*)inputBgd_TT->Get("PhysicalVariables");
	TTree *backgroundWJ = (TTree*)inputBgd_WJ->Get("PhysicalVariables");
	TTree *backgroundST = (TTree*)inputBgd_ST->Get("PhysicalVariables");
	TTree *backgroundSM = (TTree*)inputBgd_SM->Get("PhysicalVariables");
	TTree *backgroundDB = (TTree*)inputBgd_DB->Get("PhysicalVariables");
	//TTree *backgroundQC = (TTree*)inputBgd_QC->Get("PhysicalVariables");

	if (!isSigMComb) {
		TFile *inputSig = TFile::Open( fnameSig );
		std::cout << "--- TMVAClassification       : Using input signal     file: " << inputSig->GetName() << std::endl;
		TTree *signal       = (TTree*)inputSig->Get("PhysicalVariables");
	}
	else{
		TFile *inputSigDn = TFile::Open( fnameSigDn );
		TFile *inputSigCn = TFile::Open( fnameSigCn );
		TFile *inputSigUp = TFile::Open( fnameSigUp );
		std::cout << "--- Combining low mass signal samples ---" << std::endl;
		std::cout << "---   TMVAClassification       : Using input signal     file: " << inputSigDn->GetName() << std::endl;
		std::cout << "---   TMVAClassification       : Using input signal     file: " << inputSigCn->GetName() << std::endl;
		std::cout << "---   TMVAClassification       : Using input signal     file: " << inputSigUp->GetName() << std::endl;
		TTree *signalDn    = (TTree*)inputSigDn->Get("PhysicalVariables");
		TTree *signalCn    = (TTree*)inputSigCn->Get("PhysicalVariables");
		TTree *signalUp    = (TTree*)inputSigUp->Get("PhysicalVariables");
	}
	
	
   // global event weights per tree (see below for setting event-wise weights)
   //Double_t signalWeight = (10000/0.00151076) * (0.00119589/299998);
   //Double_t bgdDY_Weight = (10000) * (1/19096784.);
   //Double_t bgdTT_Weight = (10000) * (1/3488047.); //I do not know the ori_num of ttbar events but I scale it from ntuple_v2-3-6
	// You can add an arbitrary number of signal or background trees
	//factory->AddSignalTree    ( signal,       signalWeight );
	//factory->AddBackgroundTree( backgroundDY, bgdDY_Weight );
	//factory->AddBackgroundTree( backgroundTT, bgdTT_Weight );
	
//	Rz_data_muon  = [1.144, 0.006]
//	Rtt_data_muon = [0.908, 0.013]
//	Rz_data_electron  = [1.246, 0.008]
//	Rtt_data_electron = [0.975, 0.016]
	Double_t bgdDY_Weight = 1.0;
	Double_t bgdTT_Weight = 1.0;
	if (channel == "uu"){
		bgdDY_Weight = 1.144;
		bgdTT_Weight = 0.906;
	}
	else if (channel == "ee"){
		bgdDY_Weight = 1.246;
		bgdTT_Weight = 0.969;
	}
	
	
   // You can add an arbitrary number of signal or background trees
	if (!isSigMComb) {
		factory->AddSignalTree    ( signal,       1.0 );
	}
	else{
		factory->AddSignalTree    ( signalDn,       1.0 );
		factory->AddSignalTree    ( signalCn,       1.0 );
		factory->AddSignalTree    ( signalUp,       1.0 );
	}
	factory->AddBackgroundTree( backgroundDY, bgdDY_Weight );
	factory->AddBackgroundTree( backgroundTT, bgdTT_Weight );
	factory->AddBackgroundTree( backgroundWJ, 1.0 );
	factory->AddBackgroundTree( backgroundST, 1.0 );
	factory->AddBackgroundTree( backgroundSM, 1.0 );
	factory->AddBackgroundTree( backgroundDB, 1.0 );
	//////factory->AddBackgroundTree( backgroundQC, 1.0 );
	
   // To give different trees for training and testing, do as follows:
   //    factory->AddSignalTree( signalTrainingTree, signalTrainWeight, "Training" );
   //    factory->AddSignalTree( signalTestTree,     signalTestWeight,  "Test" );
   
   // Use the following code instead of the above two or four lines to add signal and background
   // training and test events "by hand"
   // NOTE that in this case one should not give expressions (such as "var1+var2") in the input
   //      variable definition, but simply compute the expression before adding the event
   //
   //     // --- begin ----------------------------------------------------------
   //     std::vector<Double_t> vars( 4 ); // vector has size of number of input variables
   //     Float_t  treevars[4], weight;
   //     
   //     // Signal
   //     for (UInt_t ivar=0; ivar<4; ivar++) signal->SetBranchAddress( Form( "var%i", ivar+1 ), &(treevars[ivar]) );
   //     for (UInt_t i=0; i<signal->GetEntries(); i++) {
   //        signal->GetEntry(i);
   //        for (UInt_t ivar=0; ivar<4; ivar++) vars[ivar] = treevars[ivar];
   //        // add training and test events; here: first half is training, second is testing
   //        // note that the weight can also be event-wise
   //        if (i < signal->GetEntries()/2.0) factory->AddSignalTrainingEvent( vars, signalWeight );
   //        else                              factory->AddSignalTestEvent    ( vars, signalWeight );
   //     }
   //   
   //     // Background (has event weights)
   //     background->SetBranchAddress( "weight", &weight );
   //     for (UInt_t ivar=0; ivar<4; ivar++) background->SetBranchAddress( Form( "var%i", ivar+1 ), &(treevars[ivar]) );
   //     for (UInt_t i=0; i<background->GetEntries(); i++) {
   //        background->GetEntry(i);
   //        for (UInt_t ivar=0; ivar<4; ivar++) vars[ivar] = treevars[ivar];
   //        // add training and test events; here: first half is training, second is testing
   //        // note that the weight can also be event-wise
   //        if (i < background->GetEntries()/2) factory->AddBackgroundTrainingEvent( vars, backgroundWeight*weight );
   //        else                                factory->AddBackgroundTestEvent    ( vars, backgroundWeight*weight );
   //     }
         // --- end ------------------------------------------------------------
   //
   // --- end of tree registration 

	//----------------------------------------------------------------------
	//----------------------------------------------------------------------
	//            Muon Channel: Weight and Selection Setting
	//----------------------------------------------------------------------
	//No longer using HLT SF
	TString doubleMuonHLT = "*(1.0)";

	TString trackerHIP1 = "*(0.991237*(Eta_muon1>-2.4)*(Eta_muon1<-2.1)+0.994853*(Eta_muon1>-2.1)*(Eta_muon1<-1.6)+0.996413*(Eta_muon1>-1.6)*(Eta_muon1<-1.2)+0.997157*(Eta_muon1>-1.2)*(Eta_muon1<-0.9)+0.997512*(Eta_muon1>-0.9)*(Eta_muon1<-0.6)+0.99756*(Eta_muon1>-0.6)*(Eta_muon1<-0.3)+0.996745*(Eta_muon1>-0.3)*(Eta_muon1<-0.2)+0.996996*(Eta_muon1>-0.2)*(Eta_muon1<0.2)+0.99772*(Eta_muon1>0.2)*(Eta_muon1<0.3)+0.998604*(Eta_muon1>0.3)*(Eta_muon1<0.6)+0.998321*(Eta_muon1>0.6)*(Eta_muon1<0.9)+0.997682*(Eta_muon1>0.9)*(Eta_muon1<1.2)+0.995252*(Eta_muon1>1.2)*(Eta_muon1<1.6)+0.994919*(Eta_muon1>1.6)*(Eta_muon1<2.1)+0.987334*(Eta_muon1>2.1)*(Eta_muon1<2.4) )";
	
	TString trackerHIP2 = "*(0.991237*(Eta_muon2>-2.4)*(Eta_muon2<-2.1)+0.994853*(Eta_muon2>-2.1)*(Eta_muon2<-1.6)+0.996413*(Eta_muon2>-1.6)*(Eta_muon2<-1.2)+0.997157*(Eta_muon2>-1.2)*(Eta_muon2<-0.9)+0.997512*(Eta_muon2>-0.9)*(Eta_muon2<-0.6)+0.99756*(Eta_muon2>-0.6)*(Eta_muon2<-0.3)+0.996745*(Eta_muon2>-0.3)*(Eta_muon2<-0.2)+0.996996*(Eta_muon2>-0.2)*(Eta_muon2<0.2)+0.99772*(Eta_muon2>0.2)*(Eta_muon2<0.3)+0.998604*(Eta_muon2>0.3)*(Eta_muon2<0.6)+0.998321*(Eta_muon2>0.6)*(Eta_muon2<0.9)+0.997682*(Eta_muon2>0.9)*(Eta_muon2<1.2)+0.995252*(Eta_muon2>1.2)*(Eta_muon2<1.6)+0.994919*(Eta_muon2>1.6)*(Eta_muon2<2.1)+0.987334*(Eta_muon2>2.1)*(Eta_muon2<2.4) )";
	
	TString doubleMuonIdAndIsoScale = "*((0.542239493)*medID2016mu1*((abs(Eta_muon1)<0.9)*(0.987*(Pt_muon1<25)+0.987*(Pt_muon1>25)*(Pt_muon1<30)+0.998*(Pt_muon1>30)*(Pt_muon1<40)+0.992*(Pt_muon1>40)*(Pt_muon1<50)+0.985*(Pt_muon1>50)*(Pt_muon1<60)+1.010*(Pt_muon1>60))+(abs(Eta_muon1)>0.9)*(abs(Eta_muon1)<1.2)*(0.975*(Pt_muon1<25)+0.969*(Pt_muon1>25)*(Pt_muon1<30)+0.979*(Pt_muon1>30)*(Pt_muon1<40)+0.983*(Pt_muon1>40)*(Pt_muon1<50)+0.982*(Pt_muon1>50)*(Pt_muon1<60)+0.981*(Pt_muon1>60))+(abs(Eta_muon1)>1.2)*(abs(Eta_muon1)<2.1)*(0.986*(Pt_muon1<25)+0.982*(Pt_muon1>25)*(Pt_muon1<30)+0.987*(Pt_muon1>30)*(Pt_muon1<40)+0.991*(Pt_muon1>40)*(Pt_muon1<50)+0.985*(Pt_muon1>50)*(Pt_muon1<60)+0.993*(Pt_muon1>60))+(abs(Eta_muon1)>2.1)*(abs(Eta_muon1)<2.4)*(0.945*(Pt_muon1<25)+0.942*(Pt_muon1>25)*(Pt_muon1<30)+0.940*(Pt_muon1>30)*(Pt_muon1<40)+0.943*(Pt_muon1>40)*(Pt_muon1<50)+0.941*(Pt_muon1>50)*(Pt_muon1<60)+0.942*(Pt_muon1>60))) + (0.457760507)*medIDmu1*((abs(Eta_muon1)<0.9)*(0.993*(Pt_muon1<25)+0.994*(Pt_muon1>25)*(Pt_muon1<30)+1.000*(Pt_muon1>30)*(Pt_muon1<40)+0.998*(Pt_muon1>40)*(Pt_muon1<50)+0.993*(Pt_muon1>50)*(Pt_muon1<60)+1.000*(Pt_muon1>60))+(abs(Eta_muon1)>0.9)*(abs(Eta_muon1)<1.2)*(0.998*(Pt_muon1<25)+0.999*(Pt_muon1>25)*(Pt_muon1<30)+0.998*(Pt_muon1>30)*(Pt_muon1<40)+0.997*(Pt_muon1>40)*(Pt_muon1<50)+0.993*(Pt_muon1>50)*(Pt_muon1<60)+0.999*(Pt_muon1>60))+(abs(Eta_muon1)>1.2)*(abs(Eta_muon1)<2.1)*(0.998*(Pt_muon1<25)+0.999*(Pt_muon1>25)*(Pt_muon1<30)+0.999*(Pt_muon1>30)*(Pt_muon1<40)+0.996*(Pt_muon1>40)*(Pt_muon1<50)+0.991*(Pt_muon1>50)*(Pt_muon1<60)+0.995*(Pt_muon1>60))+(abs(Eta_muon1)>2.1)*(abs(Eta_muon1)<2.4)*(0.984*(Pt_muon1<25)+0.983*(Pt_muon1>25)*(Pt_muon1<30)+0.975*(Pt_muon1>30)*(Pt_muon1<40)+0.975*(Pt_muon1>40)*(Pt_muon1<50)+0.970*(Pt_muon1>50)*(Pt_muon1<60)+0.971*(Pt_muon1>60))))*((0.542239493)*medID2016mu2*((abs(Eta_muon2)<0.9)*(0.987*(Pt_muon2<25)+0.987*(Pt_muon2>25)*(Pt_muon2<30)+0.998*(Pt_muon2>30)*(Pt_muon2<40)+0.992*(Pt_muon2>40)*(Pt_muon2<50)+0.985*(Pt_muon2>50)*(Pt_muon2<60)+1.010*(Pt_muon2>60))+(abs(Eta_muon2)>0.9)*(abs(Eta_muon2)<1.2)*(0.975*(Pt_muon2<25)+0.969*(Pt_muon2>25)*(Pt_muon2<30)+0.979*(Pt_muon2>30)*(Pt_muon2<40)+0.983*(Pt_muon2>40)*(Pt_muon2<50)+0.982*(Pt_muon2>50)*(Pt_muon2<60)+0.981*(Pt_muon2>60))+(abs(Eta_muon2)>1.2)*(abs(Eta_muon2)<2.1)*(0.986*(Pt_muon2<25)+0.982*(Pt_muon2>25)*(Pt_muon2<30)+0.987*(Pt_muon2>30)*(Pt_muon2<40)+0.991*(Pt_muon2>40)*(Pt_muon2<50)+0.985*(Pt_muon2>50)*(Pt_muon2<60)+0.993*(Pt_muon2>60))+(abs(Eta_muon2)>2.1)*(abs(Eta_muon2)<2.4)*(0.945*(Pt_muon2<25)+0.942*(Pt_muon2>25)*(Pt_muon2<30)+0.940*(Pt_muon2>30)*(Pt_muon2<40)+0.943*(Pt_muon2>40)*(Pt_muon2<50)+0.941*(Pt_muon2>50)*(Pt_muon2<60)+0.942*(Pt_muon2>60))) + (0.457760507)*medIDmu2*((abs(Eta_muon2)<0.9)*(0.993*(Pt_muon2<25)+0.994*(Pt_muon2>25)*(Pt_muon2<30)+1.000*(Pt_muon2>30)*(Pt_muon2<40)+0.998*(Pt_muon2>40)*(Pt_muon2<50)+0.993*(Pt_muon2>50)*(Pt_muon2<60)+1.000*(Pt_muon2>60))+(abs(Eta_muon2)>0.9)*(abs(Eta_muon2)<1.2)*(0.998*(Pt_muon2<25)+0.999*(Pt_muon2>25)*(Pt_muon2<30)+0.998*(Pt_muon2>30)*(Pt_muon2<40)+0.997*(Pt_muon2>40)*(Pt_muon2<50)+0.993*(Pt_muon2>50)*(Pt_muon2<60)+0.999*(Pt_muon2>60))+(abs(Eta_muon2)>1.2)*(abs(Eta_muon2)<2.1)*(0.998*(Pt_muon2<25)+0.999*(Pt_muon2>25)*(Pt_muon2<30)+0.999*(Pt_muon2>30)*(Pt_muon2<40)+0.996*(Pt_muon2>40)*(Pt_muon2<50)+0.991*(Pt_muon2>50)*(Pt_muon2<60)+0.995*(Pt_muon2>60))+(abs(Eta_muon2)>2.1)*(abs(Eta_muon2)<2.4)*(0.984*(Pt_muon2<25)+0.983*(Pt_muon2>25)*(Pt_muon2<30)+0.975*(Pt_muon2>30)*(Pt_muon2<40)+0.975*(Pt_muon2>40)*(Pt_muon2<50)+0.970*(Pt_muon2>50)*(Pt_muon2<60)+0.971*(Pt_muon2>60))))";
	
	//TString bTag1SFloose = "*(1-(1-(CMVA_bjet1>-0.5884)*Hjet1BsfLoose)*(1-(CMVA_bjet2>-0.5884)*Hjet2BsfLoose)*(1-(CMVA_Zjet1>-0.5884)*Zjet1BsfLoose)*(1-(CMVA_Zjet2>-0.5884)*Zjet2BsfLoose))";
	TString bTag1SFmedium = "*(1-(1-(CMVA_bjet1>0.4432)*Hjet1BsfMedium)*(1-(CMVA_bjet2>0.4432)*Hjet2BsfMedium)*(1-(CMVA_Zjet1>0.4432)*Zjet1BsfMedium)*(1-(CMVA_Zjet2>0.4432)*Zjet2BsfMedium))";
	//TString bTagPreselSF = bTag1SFloose;
	TString bTagFinalSF  = bTag1SFmedium;
	
	//TString bTagsel1loose = "*(((CMVA_bjet1>-0.5884)+(CMVA_bjet2>-0.5884)+(CMVA_Zjet1>-0.5884)+(CMVA_Zjet2>-0.5884))>0)";
	TString bTagsel1medium = "*(((CMVA_bjet1>0.4432)+(CMVA_bjet2>0.4432)+(CMVA_Zjet1>0.4432)+(CMVA_Zjet2>0.4432))>0)";
	//TString bTagPresel   = bTagsel1loose;
	TString bTagFinalsel = bTagsel1medium;
	
	//# This is the set of event filters used
	TString passfilter =  "*(passDataCert*passPrimaryVertex*(GoodVertexCount>=1))";
	passfilter += "*(passHBHENoiseFilter*passHBHENoiseIsoFilter)";
	passfilter += "*(passBadEESuperCrystal*passEcalDeadCellTP)";
	passfilter += "*(passBeamHalo2016)";
	passfilter += "*(passBadMuon*passBadChargedHadron)";

	TString metCut;
	if (SignalM == "260" || SignalM == "270" || SignalM == "300") {metCut = "*(Pt_miss<40)";}
	else if (SignalM == "350" || SignalM == "400" || SignalM == "450" || SignalM == "500" || SignalM == "550" || SignalM == "600") {metCut = "*(Pt_miss<75)";}
	else if (SignalM == "650" || SignalM == "750" || SignalM == "800" || SignalM == "900" || SignalM == "1000") {metCut = "*(Pt_miss<100)";}
	
	
	TString NormalWeightMuon = "35860.066*weight_central" + trackerHIP1 + trackerHIP2 + doubleMuonHLT + doubleMuonIdAndIsoScale + bTagFinalSF;
	TString preselection_Muon_nos = "((pass_HLT_Mu17_Mu8)*(Pt_muon1>20)*(Pt_muon2>10)*(Pt_Hjet1>20)*(Pt_Hjet2>20)*(Pt_Zjet1>20)*(Pt_Zjet2>20)*(M_uu>15)*(isMuonEvent)*(1-isElectronEvent))";
	TString preselectionMuon = preselection_Muon_nos + bTagFinalsel + "*(Charge_muon1*Charge_muon2 < 0)" + metCut + passfilter;
	//----------------------------------------------------------------------
	
	
	//----------------------------------------------------------------------
	//----------------------------------------------------------------------
	//            Electron Channel: Weight and Selection Setting
	//----------------------------------------------------------------------
	TString eleRECOScale = "*((1.3176*(Eta_ele1>-2.5)*(Eta_ele1<-2.45)+1.11378*(Eta_ele1>-2.45)*(Eta_ele1<-2.4)+1.02463*(Eta_ele1>-2.4)*(Eta_ele1<-2.3)+1.01364*(Eta_ele1>-2.3)*(Eta_ele1<-2.2)+1.00728*(Eta_ele1>-2.2)*(Eta_ele1<-2)+0.994819*(Eta_ele1>-2)*(Eta_ele1<-1.8)+0.994786*(Eta_ele1>-1.8)*(Eta_ele1<-1.63)+0.991632*(Eta_ele1>-1.63)*(Eta_ele1<-1.566)+0.963129*(Eta_ele1>-1.566)*(Eta_ele1<-1.444)+0.989701*(Eta_ele1>-1.444)*(Eta_ele1<-1.2)+0.985656*(Eta_ele1>-1.2)*(Eta_ele1<-1)+0.981595*(Eta_ele1>-1)*(Eta_ele1<-0.6)+0.984678*(Eta_ele1>-0.6)*(Eta_ele1<-0.4)+0.981614*(Eta_ele1>-0.4)*(Eta_ele1<-0.2)+0.980433*(Eta_ele1>-0.2)*(Eta_ele1<0)+0.984552*(Eta_ele1>0)*(Eta_ele1<0.2)+0.988764*(Eta_ele1>0.2)*(Eta_ele1<0.4)+0.987743*(Eta_ele1>0.4)*(Eta_ele1<0.6)+0.987743*(Eta_ele1>0.6)*(Eta_ele1<1)+0.987743*(Eta_ele1>1)*(Eta_ele1<1.2)+0.98768*(Eta_ele1>1.2)*(Eta_ele1<1.444)+0.967598*(Eta_ele1>1.444)*(Eta_ele1<1.566)+0.989627*(Eta_ele1>1.566)*(Eta_ele1<1.63)+0.992761*(Eta_ele1>1.63)*(Eta_ele1<1.8)+0.991761*(Eta_ele1>1.8)*(Eta_ele1<2)+0.99794*(Eta_ele1>2)*(Eta_ele1<2.2)+1.00104*(Eta_ele1>2.2)*(Eta_ele1<2.3)+0.989507*(Eta_ele1>2.3)*(Eta_ele1<2.4)+0.970519*(Eta_ele1>2.4)*(Eta_ele1<2.45)+0.906667*(Eta_ele1>2.45)*(Eta_ele1<2.5))";
	eleRECOScale += "*(1.3176*(Eta_ele2>-2.5)*(Eta_ele2<-2.45)+1.11378*(Eta_ele2>-2.45)*(Eta_ele2<-2.4)+1.02463*(Eta_ele2>-2.4)*(Eta_ele2<-2.3)+1.01364*(Eta_ele2>-2.3)*(Eta_ele2<-2.2)+1.00728*(Eta_ele2>-2.2)*(Eta_ele2<-2)+0.994819*(Eta_ele2>-2)*(Eta_ele2<-1.8)+0.994786*(Eta_ele2>-1.8)*(Eta_ele2<-1.63)+0.991632*(Eta_ele2>-1.63)*(Eta_ele2<-1.566)+0.963129*(Eta_ele2>-1.566)*(Eta_ele2<-1.444)+0.989701*(Eta_ele2>-1.444)*(Eta_ele2<-1.2)+0.985656*(Eta_ele2>-1.2)*(Eta_ele2<-1)+0.981595*(Eta_ele2>-1)*(Eta_ele2<-0.6)+0.984678*(Eta_ele2>-0.6)*(Eta_ele2<-0.4)+0.981614*(Eta_ele2>-0.4)*(Eta_ele2<-0.2)+0.980433*(Eta_ele2>-0.2)*(Eta_ele2<0)+0.984552*(Eta_ele2>0)*(Eta_ele2<0.2)+0.988764*(Eta_ele2>0.2)*(Eta_ele2<0.4)+0.987743*(Eta_ele2>0.4)*(Eta_ele2<0.6)+0.987743*(Eta_ele2>0.6)*(Eta_ele2<1)+0.987743*(Eta_ele2>1)*(Eta_ele2<1.2)+0.98768*(Eta_ele2>1.2)*(Eta_ele2<1.444)+0.967598*(Eta_ele2>1.444)*(Eta_ele2<1.566)+0.989627*(Eta_ele2>1.566)*(Eta_ele2<1.63)+0.992761*(Eta_ele2>1.63)*(Eta_ele2<1.8)+0.991761*(Eta_ele2>1.8)*(Eta_ele2<2)+0.99794*(Eta_ele2>2)*(Eta_ele2<2.2)+1.00104*(Eta_ele2>2.2)*(Eta_ele2<2.3)+0.989507*(Eta_ele2>2.3)*(Eta_ele2<2.4)+0.970519*(Eta_ele2>2.4)*(Eta_ele2<2.45)+0.906667*(Eta_ele2>2.45)*(Eta_ele2<2.5)))";
	
	TString doubleElectronHLTAndId = "*(ele1hltSF*ele2hltSF*ele1IDandIsoSF*ele2IDandIsoSF)";
	
	
	TString NormalWeightElectron = "35860.066*weight_central" + bTagFinalSF + eleRECOScale + doubleElectronHLTAndId;
	TString preselection_Electron_nos = "((pass_HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ)*(Pt_ele1>25)*(Pt_ele2>15)*(Pt_Hjet1>20)*(Pt_Hjet2>20)*(Pt_Zjet1>20)*(Pt_Zjet2>20)*(M_ee>15)*(isElectronEvent)*(1-isMuonEvent))";
	TString preselectionElectron = preselection_Electron_nos + bTagFinalsel + "*(Charge_ele1*Charge_ele2 < 0)" + metCut + passfilter;
	//----------------------------------------------------------------------

	
	TString NormalWeightSelect = "" ;
	TString preselectionSelect = "" ;
	if (channel == "uu"){
		NormalWeightSelect = NormalWeightMuon;
		preselectionSelect = preselectionMuon;
	}
	else if (channel == "ee"){
		NormalWeightSelect = NormalWeightElectron;
		preselectionSelect = preselectionElectron;
	}
	else{
		std::cout << " Specified Channel is not correct ! " << std::endl;
	}
	std::cout << " ==> NormalWeight used is : " << std::endl;
	std::cout << "   : " << NormalWeightSelect << std::endl;
	std::cout << " ==> Selection Cuts used is : " << std::endl;
	std::cout << "   : " << preselectionSelect << std::endl;

	
   // Set individual event weights (the variables must exist in the original TTree)
   //    for signal    : factory->SetSignalWeightExpression    ("weight1*weight2");
   //    for background: factory->SetBackgroundWeightExpression("weight1*weight2");
   factory->SetSignalWeightExpression    ( NormalWeightSelect );
   factory->SetBackgroundWeightExpression( NormalWeightSelect );
	
   // Apply additional cuts on the signal and background samples (can be different)
   TCut mycuts = preselectionSelect; // for example: TCut mycuts = "abs(var1)<0.5 && abs(var2-0.5)<1";
   TCut mycutb = preselectionSelect; // for example: TCut mycutb = "abs(var1)<0.5";

   // Tell the factory how to use the training and testing events
   //
   // If no numbers of events are given, half of the events in the tree are used 
   // for training, and the other half for testing:
   //    factory->PrepareTrainingAndTestTree( mycut, "SplitMode=random:!V" );
   // To also specify the number of testing events, use:
   //    factory->PrepareTrainingAndTestTree( mycut,
   //                                         "NSigTrain=3000:NBkgTrain=3000:NSigTest=3000:NBkgTest=3000:SplitMode=Random:!V" );
//  factory->PrepareTrainingAndTestTree( mycuts, mycutb,
//                                        "nTrain_Signal=0:nTrain_Background=0:SplitMode=Random:NormMode=NumEvents:!V" );
	factory->PrepareTrainingAndTestTree( mycuts, mycutb,
										"!V:nTrain_Signal=0:nTrain_Background=0:SplitMode=Random:NormMode=EqualNumEvents:SplitSeed=100" );

	// AH: Is NormMode=NumEvents actually the effective weighted events ? ==> Yes.
	

   // ---- Book MVA methods
   //
   // Please lookup the various method configuration options in the corresponding cxx files, eg:
   // src/MethoCuts.cxx, etc, or here: http://tmva.sourceforge.net/optionRef.html
   // it is possible to preset ranges in the option string in which the cut optimisation should be done:
   // "...:CutRangeMin[2]=-1:CutRangeMax[2]=1"...", where [2] is the third input variable

   // Cut optimisation
   if (Use["Cuts"])
      factory->BookMethod( TMVA::Types::kCuts, "Cuts",
                           "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart" );

   if (Use["CutsD"])
      factory->BookMethod( TMVA::Types::kCuts, "CutsD",
                           "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart:VarTransform=Decorrelate" );

   if (Use["CutsPCA"])
      factory->BookMethod( TMVA::Types::kCuts, "CutsPCA",
                           "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart:VarTransform=PCA" );

   if (Use["CutsGA"])
      factory->BookMethod( TMVA::Types::kCuts, "CutsGA",
                           "H:!V:FitMethod=GA:CutRangeMin[0]=-10:CutRangeMax[0]=10:VarProp[1]=FMax:EffSel:Steps=30:Cycles=3:PopSize=400:SC_steps=10:SC_rate=5:SC_factor=0.95" );

   if (Use["CutsSA"])
      factory->BookMethod( TMVA::Types::kCuts, "CutsSA",
                           "!H:!V:FitMethod=SA:EffSel:MaxCalls=150000:KernelTemp=IncAdaptive:InitialTemp=1e+6:MinTemp=1e-6:Eps=1e-10:UseDefaultScale" );

   // Likelihood ("naive Bayes estimator")
   if (Use["Likelihood"])
      factory->BookMethod( TMVA::Types::kLikelihood, "Likelihood",
                           "H:!V:TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmoothBkg[1]=10:NSmooth=1:NAvEvtPerBin=50" );

   // Decorrelated likelihood
   if (Use["LikelihoodD"])
      factory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodD",
                           "!H:!V:TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmooth=5:NAvEvtPerBin=50:VarTransform=Decorrelate" );

   // PCA-transformed likelihood
   if (Use["LikelihoodPCA"])
      factory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodPCA",
                           "!H:!V:!TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmooth=5:NAvEvtPerBin=50:VarTransform=PCA" ); 

   // Use a kernel density estimator to approximate the PDFs
   if (Use["LikelihoodKDE"])
      factory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodKDE",
                           "!H:!V:!TransformOutput:PDFInterpol=KDE:KDEtype=Gauss:KDEiter=Adaptive:KDEFineFactor=0.3:KDEborder=None:NAvEvtPerBin=50" ); 

   // Use a variable-dependent mix of splines and kernel density estimator
   if (Use["LikelihoodMIX"])
      factory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodMIX",
                           "!H:!V:!TransformOutput:PDFInterpolSig[0]=KDE:PDFInterpolBkg[0]=KDE:PDFInterpolSig[1]=KDE:PDFInterpolBkg[1]=KDE:PDFInterpolSig[2]=Spline2:PDFInterpolBkg[2]=Spline2:PDFInterpolSig[3]=Spline2:PDFInterpolBkg[3]=Spline2:KDEtype=Gauss:KDEiter=Nonadaptive:KDEborder=None:NAvEvtPerBin=50" ); 

   // Test the multi-dimensional probability density estimator
   // here are the options strings for the MinMax and RMS methods, respectively:
   //      "!H:!V:VolumeRangeMode=MinMax:DeltaFrac=0.2:KernelEstimator=Gauss:GaussSigma=0.3" );
   //      "!H:!V:VolumeRangeMode=RMS:DeltaFrac=3:KernelEstimator=Gauss:GaussSigma=0.3" );
   if (Use["PDERS"])
      factory->BookMethod( TMVA::Types::kPDERS, "PDERS",
                           "!H:!V:NormTree=T:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600" );

   if (Use["PDERSD"])
      factory->BookMethod( TMVA::Types::kPDERS, "PDERSD",
                           "!H:!V:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600:VarTransform=Decorrelate" );

   if (Use["PDERSPCA"])
      factory->BookMethod( TMVA::Types::kPDERS, "PDERSPCA",
                           "!H:!V:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600:VarTransform=PCA" );

   // Multi-dimensional likelihood estimator using self-adapting phase-space binning
   if (Use["PDEFoam"])
      factory->BookMethod( TMVA::Types::kPDEFoam, "PDEFoam",
                           "!H:!V:SigBgSeparate=F:TailCut=0.001:VolFrac=0.0666:nActiveCells=500:nSampl=2000:nBin=5:Nmin=100:Kernel=None:Compress=T" );

   if (Use["PDEFoamBoost"])
      factory->BookMethod( TMVA::Types::kPDEFoam, "PDEFoamBoost",
                           "!H:!V:Boost_Num=30:Boost_Transform=linear:SigBgSeparate=F:MaxDepth=4:UseYesNoCell=T:DTLogic=MisClassificationError:FillFoamWithOrigWeights=F:TailCut=0:nActiveCells=500:nBin=20:Nmin=400:Kernel=None:Compress=T" );

   // K-Nearest Neighbour classifier (KNN)
   if (Use["KNN"])
      factory->BookMethod( TMVA::Types::kKNN, "KNN",
                           "H:nkNN=20:ScaleFrac=0.8:SigmaFact=1.0:Kernel=Gaus:UseKernel=F:UseWeight=T:!Trim" );

   // H-Matrix (chi2-squared) method
   if (Use["HMatrix"])
      factory->BookMethod( TMVA::Types::kHMatrix, "HMatrix", "!H:!V:VarTransform=None" );

   // Linear discriminant (same as Fisher discriminant)
   if (Use["LD"])
      factory->BookMethod( TMVA::Types::kLD, "LD", "H:!V:VarTransform=None:CreateMVAPdfs:PDFInterpolMVAPdf=Spline2:NbinsMVAPdf=50:NsmoothMVAPdf=10" );

   // Fisher discriminant (same as LD)
   if (Use["Fisher"])
      factory->BookMethod( TMVA::Types::kFisher, "Fisher", "H:!V:Fisher:VarTransform=None:CreateMVAPdfs:PDFInterpolMVAPdf=Spline2:NbinsMVAPdf=50:NsmoothMVAPdf=10" );

   // Fisher with Gauss-transformed input variables
   if (Use["FisherG"])
      factory->BookMethod( TMVA::Types::kFisher, "FisherG", "H:!V:VarTransform=Gauss" );

   // Composite classifier: ensemble (tree) of boosted Fisher classifiers
   if (Use["BoostedFisher"])
      factory->BookMethod( TMVA::Types::kFisher, "BoostedFisher", 
                           "H:!V:Boost_Num=20:Boost_Transform=log:Boost_Type=AdaBoost:Boost_AdaBoostBeta=0.2:!Boost_DetailedMonitoring" );

   // Function discrimination analysis (FDA) -- test of various fitters - the recommended one is Minuit (or GA or SA)
   if (Use["FDA_MC"])
      factory->BookMethod( TMVA::Types::kFDA, "FDA_MC",
                           "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=MC:SampleSize=100000:Sigma=0.1" );

   if (Use["FDA_GA"]) // can also use Simulated Annealing (SA) algorithm (see Cuts_SA options])
      factory->BookMethod( TMVA::Types::kFDA, "FDA_GA",
                           "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=GA:PopSize=300:Cycles=3:Steps=20:Trim=True:SaveBestGen=1" );

   if (Use["FDA_SA"]) // can also use Simulated Annealing (SA) algorithm (see Cuts_SA options])
      factory->BookMethod( TMVA::Types::kFDA, "FDA_SA",
                           "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=SA:MaxCalls=15000:KernelTemp=IncAdaptive:InitialTemp=1e+6:MinTemp=1e-6:Eps=1e-10:UseDefaultScale" );

   if (Use["FDA_MT"])
      factory->BookMethod( TMVA::Types::kFDA, "FDA_MT",
                           "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=2:UseImprove:UseMinos:SetBatch" );

   if (Use["FDA_GAMT"])
      factory->BookMethod( TMVA::Types::kFDA, "FDA_GAMT",
                           "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=GA:Converger=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=0:!UseImprove:!UseMinos:SetBatch:Cycles=1:PopSize=5:Steps=5:Trim" );

   if (Use["FDA_MCMT"])
      factory->BookMethod( TMVA::Types::kFDA, "FDA_MCMT",
                           "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=MC:Converger=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=0:!UseImprove:!UseMinos:SetBatch:SampleSize=20" );

   // TMVA ANN: MLP (recommended ANN) -- all ANNs in TMVA are Multilayer Perceptrons
   if (Use["MLP"])
      factory->BookMethod( TMVA::Types::kMLP, "MLP", "H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:!UseRegulator" );

   if (Use["MLPBFGS"])
      factory->BookMethod( TMVA::Types::kMLP, "MLPBFGS", "H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:TrainingMethod=BFGS:!UseRegulator" );

   if (Use["MLPBNN"])
      factory->BookMethod( TMVA::Types::kMLP, "MLPBNN", "H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:TrainingMethod=BFGS:UseRegulator" ); // BFGS training with bayesian regulators

   // CF(Clermont-Ferrand)ANN
   if (Use["CFMlpANN"])
      factory->BookMethod( TMVA::Types::kCFMlpANN, "CFMlpANN", "!H:!V:NCycles=2000:HiddenLayers=N+1,N"  ); // n_cycles:#nodes:#nodes:...  

   // Tmlp(Root)ANN
   if (Use["TMlpANN"])
      factory->BookMethod( TMVA::Types::kTMlpANN, "TMlpANN", "!H:!V:NCycles=200:HiddenLayers=N+1,N:LearningMethod=BFGS:ValidationFraction=0.3"  ); // n_cycles:#nodes:#nodes:...

   // Support Vector Machine
   if (Use["SVM"])
      factory->BookMethod( TMVA::Types::kSVM, "SVM", "Gamma=0.25:Tol=0.001:VarTransform=Norm" );

	
	// Boosted Decision Trees
	if (Use["BDT"])  // Adaptive Boost
		factory->BookMethod( TMVA::Types::kBDT, "BDT", ParaOptStr );
			//BDT option : !H:!V:NTrees=850:MinNodeSize=4.0%:MaxDepth=3:BoostType=AdaBoost:DoBoostMonitor:AdaBoostBeta=0.35:SeparationType=GiniIndex:nCuts=20
	//----------------------------------------------------------------------
	
	// DoBoostMonitor
	// SeparationType=GiniIndex
	// NegWeightTreatment=Pray
	// MaxDepth 2-4
	// Shrinkage 0.1-0.3
	// BaggedSampleFraction 0.5-0.8
	// UseBaggedBoost >> always ?
	// MinNodeSize default = 5%
	// NTrees default = 800 try 200-1000
	// nCuts=-1
	// VarTransform=G,D VarTransform=G,D,G,D
	
	// CreateMVAPdfs:NbinsMVAPdf=20 to make BDT plot possible // https://root-forum.cern.ch/t/bdt-c-macro-in-the-tmvagui-error-in-txmlengine-parsefile-xml-syntax-error-at-line/29593/2
	
	if (Use["BDTG"]) // Gradient Boost
		factory->BookMethod( TMVA::Types::kBDT, "BDTG",
							"!H:!V:BoostType=Grad:DoBoostMonitor:SeparationType=GiniIndex:NegWeightTreatment=Pray:NTrees=850:MinNodeSize=2.5%:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=20:MaxDepth=3:CreateMVAPdfs:NbinsMVAPdf=20");
	//------------------- Gradient Boost
	
	if (Use["BDTG01"]) // Gradient Boost
		factory->BookMethod( TMVA::Types::kBDT, "BDTG01",
							"!H:!V:BoostType=Grad:DoBoostMonitor:SeparationType=GiniIndex:NTrees=850:MinNodeSize=2.5%:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=20:MaxDepth=3:CreateMVAPdfs:NbinsMVAPdf=20");
	
	
	
	
	
	if (Use["BDTG02"]) // Gradient Boost // MaxDepth=2
		factory->BookMethod( TMVA::Types::kBDT, "BDTG02",
							"!H:!V:BoostType=Grad:DoBoostMonitor:SeparationType=GiniIndex:NegWeightTreatment=Pray:NTrees=1200:MinNodeSize=3%:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=20:MaxDepth=2:CreateMVAPdfs:NbinsMVAPdf=20");
	if (Use["BDTG03"]) // Gradient Boost // MinNodeSize=5%
		factory->BookMethod( TMVA::Types::kBDT, "BDTG03",
							"!H:!V:BoostType=Grad:DoBoostMonitor:SeparationType=GiniIndex:NegWeightTreatment=Pray:NTrees=1200:MinNodeSize=5%:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=20:MaxDepth=4:CreateMVAPdfs:NbinsMVAPdf=20");
	if (Use["BDTG04"]) // Gradient Boost // MaxDepth=3 MinNodeSize=5%
		factory->BookMethod( TMVA::Types::kBDT, "BDTG04",
							"!H:!V:BoostType=Grad:DoBoostMonitor:SeparationType=GiniIndex:NegWeightTreatment=Pray:NTrees=1200:MinNodeSize=5%:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=20:MaxDepth=3:CreateMVAPdfs:NbinsMVAPdf=20");
	
	
	
	if (Use["BDTG05"]) // Gradient Boost // MinNodeSize=5%
		factory->BookMethod( TMVA::Types::kBDT, "BDTG05",
							"!H:!V:BoostType=Grad:DoBoostMonitor:SeparationType=GiniIndex:NegWeightTreatment=Pray:NTrees=850:MinNodeSize=5.0%:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=20:MaxDepth=3");
	if (Use["BDTG06"]) // Gradient Boost // Shrinkage=0.05
		factory->BookMethod( TMVA::Types::kBDT, "BDTG06",
						"!H:!V:BoostType=Grad:DoBoostMonitor:SeparationType=GiniIndex:NegWeightTreatment=Pray:NTrees=850:MinNodeSize=2.5%:Shrinkage=0.05:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=20:MaxDepth=3");
	if (Use["BDTG07"]) // Gradient Boost
		factory->BookMethod( TMVA::Types::kBDT, "BDTG07",
						"!H:!V:BoostType=Grad:DoBoostMonitor:SeparationType=GiniIndex:NegWeightTreatment=Pray:NTrees=850:MinNodeSize=2.5%:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=-1:MaxDepth=3");
	if (Use["BDTG08"]) // Gradient Boost
		factory->BookMethod( TMVA::Types::kBDT, "BDTG08",
						"!H:!V:BoostType=Grad:DoBoostMonitor:SeparationType=GiniIndex:NegWeightTreatment=Pray:NTrees=850:MinNodeSize=2.5%:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=20:MaxDepth=2");
	if (Use["BDTG09"]) // Gradient Boost // MaxDepth=4
		factory->BookMethod( TMVA::Types::kBDT, "BDTG09",
						"!H:!V:BoostType=Grad:DoBoostMonitor:SeparationType=GiniIndex:NegWeightTreatment=Pray:NTrees=850:MinNodeSize=2.5%:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=20:MaxDepth=4");
	//----------------------------------------------------------------------


   if (Use["BDTB"]) // Bagging
      factory->BookMethod( TMVA::Types::kBDT, "BDTB",
                           "!H:!V:NTrees=400:BoostType=Bagging:SeparationType=GiniIndex:nCuts=20" );

   if (Use["BDTD"]) // Decorrelation + Adaptive Boost
      factory->BookMethod( TMVA::Types::kBDT, "BDTD",
                           "!H:!V:NTrees=400:MinNodeSize=5%:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:VarTransform=Decorrelate" );

   if (Use["BDTF"])  // Allow Using Fisher discriminant in node splitting for (strong) linearly correlated variables
	  factory->BookMethod( TMVA::Types::kBDT, "BDTMitFisher",
						   "!H:!V:NTrees=50:MinNodeSize=2.5%:UseFisherCuts:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20" );

   // RuleFit -- TMVA implementation of Friedman's method
   if (Use["RuleFit"])
      factory->BookMethod( TMVA::Types::kRuleFit, "RuleFit",
                           "H:!V:RuleFitModule=RFTMVA:Model=ModRuleLinear:MinImp=0.001:RuleMinDist=0.001:NTrees=20:fEventsMin=0.01:fEventsMax=0.5:GDTau=-1.0:GDTauPrec=0.01:GDStep=0.01:GDNSteps=10000:GDErrScale=1.02" );

   // For an example of the category classifier usage, see: TMVAClassificationCategory

   // --------------------------------------------------------------------------------------------------

   // ---- Now you can optimize the setting (configuration) of the MVAs using the set of training events

   // ---- STILL EXPERIMENTAL and only implemented for BDT's ! 
   // factory->OptimizeAllMethods("SigEffAt001","Scan");
   // factory->OptimizeAllMethods("ROCIntegral","FitGA");

   // --------------------------------------------------------------------------------------------------

   // ---- Now you can tell the factory to train, test, and evaluate the MVAs

   // Train MVAs using the set of training events
   factory->TrainAllMethods();

   // ---- Evaluate all MVAs using the set of test events
   factory->TestAllMethods();

   // ----- Evaluate and compare performance of all configured MVAs
   factory->EvaluateAllMethods();

   // --------------------------------------------------------------

   // Save the output
   outputFile->Close();

   std::cout << "==> Wrote root file: " << outputFile->GetName() << std::endl;
   std::cout << "==> TMVAClassification is done!" << std::endl;

   delete factory;

   // Launch the GUI for the root macros
   //if (!gROOT->IsBatch()) TMVAGui( outfileName );
}
