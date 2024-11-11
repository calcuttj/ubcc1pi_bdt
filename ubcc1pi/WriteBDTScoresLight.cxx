/**
*  @file  ubcc1pi_standalone/Macros/WriteBDTScores.cxx
*
*  @brief Write BDT score for each reco particle into the TTree 
*
*/

//#include "ubcc1pi_standalone/Macros/Macros.h"
#include "Objects/FileReader.h"
#include "BDT.h"

#include "boost/program_options/options_description.hpp"
#include "boost/program_options/variables_map.hpp"
#include "boost/program_options/parsers.hpp"

//#include "ubcc1pi_standalone/Helpers/AnalysisHelper.h"

using namespace ubcc1pi;

int main(int argc, char ** argv) // WriteBDTScoresLight(/*const Config &config*/)
{

    namespace po = boost::program_options;

    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("input,i", po::value<std::string>(), "Input file")
        ("output,o", po::value<std::string>(), "Output file")
        ("proton,p", po::value<std::string>(), "Proton BDT file")
        ("muon,p", po::value<std::string>(), "Muon BDT file")
        ("nevents,n", po::value<int>(), "Number of events (-1 for all)");
    
    po::variables_map vm;
    po::store(
        po::command_line_parser(argc, argv).options(desc).style(
            po::command_line_style::unix_style |
            po::command_line_style::allow_dash_for_short).run(),
        vm);
    po::notify(vm);    
    
    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }

    std::string input_file = "";
    if (vm.count("input")) {
      input_file = vm["input"].as<std::string>();
    }
    else {
      std::cout << "Need file" << std::endl;
      return 1;
    }

    std::string output_file = "";
    if (vm.count("output")) {
      output_file = vm["output"].as<std::string>();
    }
    else {
      std::cout << "Need output file" << std::endl;
      return 1;
    }

    std::string proton_bdt_file = "/exp/uboone/app/users/jdetje/cc1pi_handover_57_2/FebPeLEE4/proton_dataset/weights/proton_BDT.weights.xml";
    if (vm.count("proton")) {
      proton_bdt_file = vm["proton"].as<std::string>();
    }
    std::string muon_bdt_file = "/exp/uboone/app/users/jdetje/cc1pi_handover_57_2/FebPeLEE4/muon_dataset/weights/muon_BDT.weights.xml";
    if (vm.count("muon")) {
      muon_bdt_file = vm["muon"].as<std::string>();
    }
    else {
      std::cout << "Need muon bdt file" << std::endl;
      //return 1;
    }

    int nevents = (!vm.count("nevents")) ? -1 : vm["nevents"].as<int>();




	std::cout<<"Setup input data" <<std::endl;
	// ~~~~~~~~~~~~~~~~~~~
	// Set-up input data
	// ~~~~~~~~~~~~~~~~~~	
        //std::string thefile = "/exp/uboone/data/users/cthorpe/PELEE_2023_Samples/run1/numupresel/overlay_peleeTuple_uboone_v08_00_00_70_run1_nu.root";
	std::vector<std::string> inputData = {input_file};
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Set up BDTs for evaluation using trained BDT weight files
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	const auto muonFeatureNames = BDTHelper::MuonBDTFeatureNames;
        //std::string bdt_muon_file = "/exp/uboone/app/users/jdetje/cc1pi_handover_57_2/FebPeLEE4/muon_dataset/weights/muon_BDT.weights.xml";
        BDTHelper::BDT muonBDT("muon", muon_bdt_file, muonFeatureNames);	
        //std::string bdt_proton_file = "/exp/uboone/app/users/jdetje/cc1pi_handover_57_2/FebPeLEE4/proton_dataset/weights/proton_BDT.weights.xml";
	const auto protonFeatureNames = BDTHelper::ProtonBDTFeatureNames;
	BDTHelper::BDT protonBDT("proton", proton_bdt_file, protonFeatureNames);

	for (const auto & filePath : inputData) {
		
		std::cout << filePath << std::endl;	
		
		std::vector<float> protonBDTResponses;
		std::vector<float> muonBDTResponses;

		TFile* inputFile = TFile::Open(filePath.c_str());
		TFile* outputFile = new TFile(output_file.c_str(), "RECREATE");
		std::cout << "finding tree" <<std::endl;
		TTree* tree = dynamic_cast<TTree*>(inputFile->Get("nuselection/NeutrinoSelectionFilter"));
		TTree* subrun = dynamic_cast<TTree*>(inputFile->Get("nuselection/SubRun"));

		if (!tree) {
        		std::cerr << "Error: Unable to find the specified tree in the ROOT file." << std::endl;
        		inputFile->Close();
    		}	
		
                std::cout << "Got tree" << std::endl;

                //std::cout << "Copying out tree" << std::endl;
		//TTree* outTree = tree->CopyTree("1");
                //std::cout << "Copying subrun tree" << std::endl;
		//TTree* outSubrunTree = subrun->CopyTree("1");
                //std::cout << "Done" << std::endl;
                TTree outTree("BDTScores", "");

		TBranch* protonBDTBranch = outTree.Branch("protonBDTResponses", &protonBDTResponses);
		TBranch* muonBDTBranch = outTree.Branch("muonBDTResponses", &muonBDTResponses);	
		
		const bool isMC = true; //(sampleType == AnalysisHelper::Overlay || sampleType == AnalysisHelper::Dirt ||  sampleType == AnalysisHelper::NuWro);
		FileReader<EventPeLEE, SubrunPeLEE> readerPeLEE(filePath, isMC);
		auto pEventPeLEE = readerPeLEE.GetBoundEventAddress();

		//for (unsigned int i = 0, nEvents = readerPeLEE.GetNumberOfEvents(); i < 20; ++i)
                std::cout << "Start Loop" << std::endl;
                size_t nEvents = nEvents = readerPeLEE.GetNumberOfEvents();
                if (nevents > 0) {
                  nEvents = (size_t)nevents;
                }

		for (unsigned int i = 0; i < nEvents; ++i)
        	{

			if (i%1000 == 0) std::cout << "Event " << i << std::endl;
		//	std::cout << "Entry: " << i << std::endl;
			readerPeLEE.LoadEvent(i);
            		Event event(*pEventPeLEE, false);	
			const auto pEvent = std::make_shared<Event>(event);
			
			//const auto truthParticles = pEvent->truth.particles;
            		const auto recoParticles = pEvent->reco.particles;
			for (const auto &recoParticle : recoParticles)
			{
				//std::cout << "nreco: " << recoParticles.size() << std::endl;
				std::vector<float> muonFeatures;
                                const auto areAllFeaturesAvailableMuon = BDTHelper::GetBDTFeatures(recoParticle, muonFeatureNames, muonFeatures);
				std::vector<float> protonFeatures; 
                		const auto areAllFeaturesAvailableProton = BDTHelper::GetBDTFeatures(recoParticle, protonFeatureNames, protonFeatures);

				if (areAllFeaturesAvailableProton)
        			{	
					const auto protonBDTResponse = protonBDT.GetResponse(protonFeatures);	
					protonBDTResponses.push_back(protonBDTResponse);
					//std::cout << protonBDTResponse << std::endl;
				}

				else 
				{
					protonBDTResponses.push_back(9999.);
					//std::cout << "9999." << std::endl;
				}
				
				if (areAllFeaturesAvailableMuon)
                                {
                                        const auto muonBDTResponse = muonBDT.GetResponse(muonFeatures);
                                        muonBDTResponses.push_back(muonBDTResponse);
                                	//std::cout << muonBDTResponse << std::endl;
				}

                                else
                                {
                                        muonBDTResponses.push_back(9999.);
                        		//std::cout << "9999." << std::endl;
			        }
			}
			//protonBDTBranch->Fill();
			//muonBDTBranch->Fill();
                        outTree.Fill();
			protonBDTResponses.clear();
			muonBDTResponses.clear();
		}
                outputFile->cd();
		outTree.Write();
		//outSubrunTree->Write();
		outputFile->Write();
		outputFile->Close();
		inputFile->Close();
	}
///uboone/data/users/kwresilo/CCNp1pi/data/newPeLEE/nu_overlay_run1.root

  return 0;
}
