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

void GetGeneralFeatures(const auto & reco_particle,
                        const std::vector<std::string> & feature_names,
                        BDTHelper::BDT & the_bdt,
                        std::vector<float> & responses) {
  std::vector<float> features;
  if (BDTHelper::GetBDTFeatures(reco_particle, feature_names, features)) {
    responses.push_back(the_bdt.GetResponse(features));
  }
  else {
    responses.push_back(9999.);
  }
}

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
        ("muon,m", po::value<std::string>(), "Muon BDT file")
        ("pion,g", po::value<std::string>(), "Golden Pion BDT file")
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
    //else {
    //  std::cout << "Need muon bdt file" << std::endl;
    //  //return 1;
    //}
    std::string golden_pion_bdt_file = "/exp/uboone/app/users/jdetje/cc1pi_handover_57_2/FebPeLEE4/goldenPion_dataset/weights/goldenPion_BDT.weights.xml";
    if (vm.count("pion")) {
      golden_pion_bdt_file = vm["pion"].as<std::string>();
    }
    //else {
    //  std::cout << "Need pion bdt file" << std::endl;
    //  //return 1;
    //}
    std::cout << "Using bdt files:\n";
    std::cout << "\tProton: " << proton_bdt_file << "\n" <<
                 "\tMuon: " << muon_bdt_file << "\n" <<
                 "\tPion: " << golden_pion_bdt_file << std::endl;

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

        const auto goldenPionFeatureNames = BDTHelper::GoldenPionBDTFeatureNames;
        BDTHelper::BDT goldenPionBDT("goldenPion", golden_pion_bdt_file,
                                     goldenPionFeatureNames);

        for (const auto & filePath : inputData) {

                std::cout << filePath << std::endl;

                std::vector<float> protonBDTResponses;
                std::vector<float> muonBDTResponses;
                std::vector<float> goldenPionBDTResponses;

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

                TBranch* protonBDTBranch = outTree.Branch("protonBDTResponses",
                                                          &protonBDTResponses);
                TBranch* muonBDTBranch = outTree.Branch("muonBDTResponses",
                                                        &muonBDTResponses);
                TBranch* goldenPionBDTBranch = outTree.Branch("goldenPionBDTResponses",
                                                              &goldenPionBDTResponses);

                const bool isMC = true; //(sampleType == AnalysisHelper::Overlay || sampleType == AnalysisHelper::Dirt ||  sampleType == AnalysisHelper::NuWro);
                FileReader<EventPeLEE, SubrunPeLEE> readerPeLEE(filePath, isMC);
                auto pEventPeLEE = readerPeLEE.GetBoundEventAddress();

                //for (unsigned int i = 0, nEvents = readerPeLEE.GetNumberOfEvents(); i < 20; ++i)
                std::cout << "Start Loop" << std::endl;
                size_t nEvents = nEvents = readerPeLEE.GetNumberOfEvents();
                if (nevents > 0) {
                  nEvents = (size_t)nevents;
                }

                for (unsigned int i = 0; i < nEvents; ++i) {
                        if (i%1000 == 0) std::cout << "Event " << i << std::endl;
                        readerPeLEE.LoadEvent(i);
                        Event event(*pEventPeLEE, false);
                        const auto pEvent = std::make_shared<Event>(event);

                        //const auto truthParticles = pEvent->truth.particles;
                        const auto recoParticles = pEvent->reco.particles;
                        for (const auto &recoParticle : recoParticles) {
                                //std::cout << "nreco: " << recoParticles.size() << std::endl;
                                std::vector<float> protonFeatures;
                                //const auto areAllFeaturesAvailableProton = ;

                                GetGeneralFeatures(
                                    recoParticle, protonFeatureNames,
                                    protonBDT, protonBDTResponses);
                                GetGeneralFeatures(
                                    recoParticle, muonFeatureNames,
                                    muonBDT, muonBDTResponses);
                                GetGeneralFeatures(
                                    recoParticle, goldenPionFeatureNames,
                                    goldenPionBDT, goldenPionBDTResponses);

                                /*if (BDTHelper::GetBDTFeatures(recoParticle, protonFeatureNames, protonFeatures)) {
                                        const auto protonBDTResponse = protonBDT.GetResponse(protonFeatures);
                                        protonBDTResponses.push_back(protonBDTResponse);
                                }
                                else {
                                        protonBDTResponses.push_back(9999.);
                                }*/
                                //std::cout << protonBDTResponses.back() << std::endl;

                                //std::vector<float> muonFeatures;
                                //if (BDTHelper::GetBDTFeatures(recoParticle, muonFeatureNames, muonFeatures)) {
                                //        const auto muonBDTResponse = muonBDT.GetResponse(muonFeatures);
                                //        muonBDTResponses.push_back(muonBDTResponse);
                                //}
                                //else {
                                //        muonBDTResponses.push_back(9999.);
                                //}
                                //std::cout << muonBDTResponses.back() << std::endl;
                        }
                        //protonBDTBranch->Fill();
                        //muonBDTBranch->Fill();
                        outTree.Fill();
                        protonBDTResponses.clear();
                        muonBDTResponses.clear();
                        goldenPionBDTResponses.clear();
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
