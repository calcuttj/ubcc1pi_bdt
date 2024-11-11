/**
 *  @file  ubcc1pi_standalone/Helpers/BDTHelper.h
 *
 *  @brief The header file for the bdt helper class
 */

#ifndef UBCC1PI_STANDALONE_HELPERS_BDT_HELPER
#define UBCC1PI_STANDALONE_HELPERS_BDT_HELPER

#include <string>
#include <vector>
#include <unordered_map>

#include "TMVA/Factory.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Reader.h"
#include <TFile.h>
#include "Interface/Member.h"
#include "Interface/Event.h"

namespace ubcc1pi
{

/**
 *  @brief  The BDT helper class
 */
namespace BDTHelper
{
        typedef std::vector< std::pair<float, float> > BDTResponseWeightVector; ///< A vector of BDT responses and their corresponding weights
        /**
         *  @brief  Base wrapper class for at root TMVA BDT
         */
        class BDTBase
        {
            public:
                /**
                 *  @brief  Constructor
                 *
                 *  @param  bdtName the name of the BDT
                 *  @param  featureNames the vector of feature variable names
                 */
                BDTBase(const std::string &bdtName, const std::vector<std::string> &featureNames);

                /**
                 *  @brief  Get the name
                 *
                 *  @return BDT name
                 */
                std::string GetName() const;

            protected:

                std::string               m_bdtName;      ///< The BDT name
                std::vector<std::string>  m_featureNames; ///< The feature variable names
        };

        /**
         *  @brief  The class for using a trained BDT
         */
        class BDT : public BDTBase
        {
            public:
                /**
                 *  @brief  Constructor
                 *
                 *  @param  bdtName the name of the BDT
                 *  @param  featureNames the vector of feature variable names
                 */
                BDT(const std::string &bdtName, const std::string & path, const std::vector<std::string> &featureNames);

                /**
                 *  @brief  Get the BDT response for a given set of features
                 *
                 *  @param  features the input vector of features
                 *
                 *  @return the BDT response
                 */
                float GetResponse(const std::vector<float> &features);

            private:
                TMVA::Reader      *m_pReader;     ///< The TMVA reader
                std::string        m_weightsFile; ///< The weights file
                std::vector<float> m_features;    ///< The BDT features
        };



// -----------------------------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------------------------

const std::vector<std::string> ParticleBDTFeatureNames = {
    "logBragg_pToMIP",
    "logBragg_piToMIP",
    "truncMeandEdx",
    "protonForward",
    "muonForward",
    "nDescendents",
    "nSpacePointsNearEnd",
    "wiggliness",
    "trackScore",
    "llrPID"
};

// -----------------------------------------------------------------------------------------------------------------------------------------

const std::vector<std::string> MuonBDTFeatureNames = {
    "logBragg_pToMIP",
    "logBragg_piToMIP",
    "truncMeandEdx",
    //"protonForward",
    //"muonForward",
    "nDescendents",
    //"nSpacePointsNearEnd",
    "wiggliness",
    "trackScore"//,
    //"llrPID"
};

// -----------------------------------------------------------------------------------------------------------------------------------------

const std::vector<std::string> ProtonBDTFeatureNames = {
    "logBragg_pToMIP",
    "logBragg_piToMIP",
    "truncMeandEdx",
    //"protonForward",
    //"muonForward",
    //"nDescendents",
    //"nSpacePointsNearEnd",
    "wiggliness",
    "trackScore"//,
    //"llrPID"
};

// -----------------------------------------------------------------------------------------------------------------------------------------

const std::vector<std::string> GoldenPionBDTFeatureNames = {
    "logBragg_pToMIP",
    "logBragg_piToMIP",
    "truncMeandEdx",
    "nDescendents",
    "wiggliness",
    "trackScore"
};

 bool GetBDTFeatures(const Event::Reco::Particle &recoParticle, const std::vector<std::string> &featureNames, std::vector<float> &features, const bool shouldDebug = false);
 bool GetLogLikelihoodRatio(const Member<float> &numerator, const Member<float> &denominator, float &ratio);

/**
 *  @brief  Get the softmax of two variables: exp(s) / (exp(s) + exp(b))
 *
 *  @param  signal the signal variable member
 *  @param  background the background variable member
 *  @param  softmax the output softmax
 *
 *  @return boolean, true if the value is calculable
 */
 bool GetSoftmax(const Member<float> &signal, const Member<float> &background, float &softmax);

}
} // namespace ubcc1pi

#endif
