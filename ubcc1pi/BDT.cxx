/**
 *  @file  ubcc1pi_standalone/Helpers/BDTHelper.cxx
 *
 *  @brief Implementation of the BDT helper class
 */

#include "BDT.h"
//#include "ubcc1pi_standalone/Helpers/AnalysisHelper.h"

#include <TString.h>
#include <random>
#include <algorithm>

namespace ubcc1pi
{


BDTHelper::BDTBase::BDTBase(const std::string &bdtName, const std::vector<std::string> &featureNames) :
    m_bdtName(bdtName),
    m_featureNames(featureNames)
{
}

// -----------------------------------------------------------------------------------------------------------------------------------------

std::string BDTHelper::BDTBase::GetName() const
{
    return m_bdtName;
}

BDTHelper::BDT::BDT(const std::string &bdtName, const std::string & path, const std::vector<std::string> &featureNames) :
    BDTBase(bdtName, featureNames),
    m_pReader(new TMVA::Reader("Silent")),
    /*m_weightsFile(bdtName + "_dataset/weights/" + bdtName + "_BDT.weights.xml"),*/
    m_weightsFile(path),
    m_features(featureNames.size(), -std::numeric_limits<float>::max())
{
    // Bind the feature vector to the reader
    for (unsigned int iFeature = 0; iFeature < featureNames.size(); ++iFeature)
        m_pReader->AddVariable(featureNames.at(iFeature).c_str(), &m_features.at(iFeature));

    // Book the MVA using the weights file
    m_pReader->BookMVA("BDT", m_weightsFile.c_str());
}

// -----------------------------------------------------------------------------------------------------------------------------------------

float BDTHelper::BDT::GetResponse(const std::vector<float> &features)
{
    if (features.size() != m_features.size())
        throw std::invalid_argument("BDTHelper::BDT::GetResponse - Wrong number of features, expected " + std::to_string(m_features.size()) + " but passed " + std::to_string(features.size()));

    // Shallow copy the features
    for (unsigned int iFeature = 0; iFeature < m_features.size(); ++iFeature)
        m_features.at(iFeature) = features.at(iFeature);

    // Run the BDT
    return m_pReader->EvaluateMVA("BDT");
}

bool BDTHelper::GetBDTFeatures(const Event::Reco::Particle &recoParticle, const std::vector<std::string> &featureNames, std::vector<float> &features, const bool shouldDebug)
{
    if (!features.empty())
        throw std::logic_error("BDTHelper::GetBDTFeatures - Input feature vector isn't empty");

    for (const auto &name : featureNames)
    {
        if (shouldDebug)
            std::cout << "DEBUG - Calculating: " << name << std::endl;

        if (name == "logBragg_pToMIP")
        {
            float feature = -std::numeric_limits<float>::max();
            if (!GetLogLikelihoodRatio(recoParticle.likelihoodForwardProton, recoParticle.likelihoodMIP, feature))
            {
                if (shouldDebug)
                    std::cout << "DEBUG - Can't calculate: " << name << std::endl;

                features.clear();
                return false;
            }

            features.push_back(feature);
            continue;
        }

        if (name == "logBragg_piToMIP")
        {
            float feature = -std::numeric_limits<float>::max();
            if (!GetLogLikelihoodRatio(recoParticle.likelihoodForwardPion, recoParticle.likelihoodMIP, feature))
            {
                if (shouldDebug)
                    std::cout << "DEBUG - Can't calculate: " << name << std::endl;

                features.clear();
                return false;
            }

            features.push_back(feature);
            continue;
        }

        if (name == "truncMeandEdx")
        {
            if (!recoParticle.truncatedMeandEdx.IsSet())
            {
                if (shouldDebug)
                    std::cout << "DEBUG - Can't calculate: " << name << std::endl;

                features.clear();
                return false;
            }

            features.push_back(recoParticle.truncatedMeandEdx());
            continue;
        }

        if (name == "protonForward")
        {
            float feature = -std::numeric_limits<float>::max();
            if (!GetSoftmax(recoParticle.likelihoodForwardProton, recoParticle.likelihoodBackwardProton, feature))
            {
                if (shouldDebug)
                    std::cout << "DEBUG - Can't calculate: " << name << std::endl;

                features.clear();
                return false;
            }

            features.push_back(feature);
            continue;
        }
        if (name == "muonForward")
        {
            float feature = -std::numeric_limits<float>::max();
            if (!GetSoftmax(recoParticle.likelihoodForwardMuon, recoParticle.likelihoodBackwardMuon, feature))
            {
                if (shouldDebug)
                    std::cout << "DEBUG - Can't calculate: " << name << std::endl;

                features.clear();
                return false;
            }

            features.push_back(feature);
            continue;
        }
        if (name == "nDescendents")
        {
            if (!recoParticle.nDescendents.IsSet())
            {
                if (shouldDebug)
                    std::cout << "DEBUG - Can't calculate: " << name << std::endl;

                features.clear();
                return false;
            }

            features.push_back(recoParticle.nDescendents());
            continue;
        }

        if (name == "nSpacePointsNearEnd")
        {
            if (!recoParticle.nSpacePointsNearEnd.IsSet())
            {
                if (shouldDebug)
                    std::cout << "DEBUG - Can't calculate: " << name << std::endl;

                features.clear();
                return false;
            }

            features.push_back(recoParticle.nSpacePointsNearEnd());
            continue;
        }

        if (name == "wiggliness")
        {
            if (!recoParticle.wiggliness.IsSet())
            {
                if (shouldDebug)
                    std::cout << "DEBUG - Can't calculate: " << name << std::endl;

                features.clear();
                return false;
            }

            features.push_back(recoParticle.wiggliness());
            continue;
        }

        if (name == "trackScore")
        {
            if (!recoParticle.trackScore.IsSet())
            {
                if (shouldDebug)
                    std::cout << "DEBUG - Can't calculate: " << name << std::endl;

                features.clear();
                return false;
            }

            features.push_back(recoParticle.trackScore());
            continue;
        }

	if (name == "llrPID")
        {
            if (!recoParticle.llrScore.IsSet())
            {
                if (shouldDebug)
                    std::cout << "DEBUG - Can't calculate: " << name << std::endl;

                features.clear();
                return false;
            }

            features.push_back(recoParticle.llrScore());
            continue;
        }

        throw std::invalid_argument("BDTHelper::GetBDTFeatures - Unknown feature: \"" + name + "\"");
    }

    // This should never happen - just a sanity check
    if (features.size() != featureNames.size())
        throw std::logic_error("BDTHelper::GetBDTFeatures - Number of features calculated is incorrect!");

    return true;
}

bool BDTHelper::GetLogLikelihoodRatio(const Member<float> &numerator, const Member<float> &denominator, float &ratio)
{
    ratio = -std::numeric_limits<float>::max();

    if (!numerator.IsSet() || !denominator.IsSet())
        return false;

    if (numerator() < 0.f)
        return false;

    if (denominator() <= std::numeric_limits<float>::epsilon())
        return false;

    ratio = std::log(numerator() / denominator());
    return true;
}

// -----------------------------------------------------------------------------------------------------------------------------------------

bool BDTHelper::GetSoftmax(const Member<float> &signal, const Member<float> &background, float &softmax)
{
    softmax = -std::numeric_limits<float>::max();

    if (!signal.IsSet() || !background.IsSet())
        return false;

    const auto denominator = std::exp(signal()) + std::exp(background());
    if (std::abs(denominator) <= std::numeric_limits<float>::epsilon())
        return false;

    softmax = std::exp(signal()) / denominator;
    return true;
}

}

