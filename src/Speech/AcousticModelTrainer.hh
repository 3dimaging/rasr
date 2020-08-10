/** Copyright 2020 RWTH Aachen University. All rights reserved.
 *
 *  Licensed under the RWTH ASR License (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.hltpr.rwth-aachen.de/rwth-asr/rwth-asr-license.html
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#ifndef _SPEECH_ACOUSTIC_MODEL_TRAINER_HH
#define _SPEECH_ACOUSTIC_MODEL_TRAINER_HH

#include <Am/AcousticModel.hh>
#include "AlignedFeatureProcessor.hh"
#include "MixtureSetTrainer.hh"

namespace Speech {

/**
 *  Base class for acoustic training applications.
 *
 *  Extends the AlignedFeatureProcessor by adding an acoustic model
 *  and a lexicon
 */
class AcousticModelTrainer : public AlignedFeatureProcessor {
    typedef AlignedFeatureProcessor Precursor;

private:
    Bliss::LexiconRef            lexicon_;
    Core::Ref<Am::AcousticModel> acousticModel_;

protected:
    Bliss::LexiconRef lexicon() const {
        return lexicon_;
    }
    Core::Ref<Am::AcousticModel> acousticModel() const {
        return acousticModel_;
    }
    virtual void processAlignedFeature(Core::Ref<const Feature> f, Am::AllophoneStateIndex e) {
        processAlignedFeature(f, e, 1.0);
    }
    virtual void processAlignedFeature(Core::Ref<const Feature>, Am::AllophoneStateIndex, Mm::Weight) {
        criticalError("Processing of weighted alignments is not supported.");
    }

public:
    void signOn(CorpusVisitor& corpusVisitor);
    AcousticModelTrainer(const Core::Configuration& c, Am::AcousticModel::Mode mode);
    virtual ~AcousticModelTrainer();
};

/** TextDependentMixtureSetTrainer
 */
class TextDependentMixtureSetTrainer : public AcousticModelTrainer,
                                       public MlMixtureSetTrainer {
private:
    Mm::FeatureDescription featureDescription_;
    bool                   initialized_;

protected:
    virtual void setFeatureDescription(const Mm::FeatureDescription&);

    virtual void processAlignedFeature(Core::Ref<const Feature> f, Am::AllophoneStateIndex e) {
        accumulate(f->mainStream(), acousticModel()->emissionIndex(e));
    }
    virtual void processAlignedFeature(Core::Ref<const Feature> f, Am::AllophoneStateIndex e, Mm::Weight w) {
        accumulate(f->mainStream(), acousticModel()->emissionIndex(e), w);
    }

public:
    TextDependentMixtureSetTrainer(const Core::Configuration&);
    virtual ~TextDependentMixtureSetTrainer() {}
};

class TiedTextDependentMixtureSetTrainer : public AcousticModelTrainer,
                                           public MlMixtureSetTrainer {
private:
    static const Core::ParameterFloat               paramTyingFactor;
    static const Core::ParameterFloat               paramTyingMinFactor;
    static const Core::ParameterInt                 paramTyingMinDepth;
    static const Core::ParameterInt                 paramTyingMaxEmissions;
    Mm::FeatureDescription                          featureDescription_;
    bool                                            initialized_;
    float                                           tyingFactor_, minTyingFactor_;
    int                                             minDepth_, maxEmissions_;
    std::vector<std::vector<std::pair<u32, float>>> tiedEmissions_;

protected:
    virtual void setFeatureDescription(const Mm::FeatureDescription&);

    virtual void processAlignedFeature(Core::Ref<const Feature> f, Am::AllophoneStateIndex e);
    virtual void processAlignedFeature(Core::Ref<const Feature> f, Am::AllophoneStateIndex e, Mm::Weight w);

public:
    TiedTextDependentMixtureSetTrainer(const Core::Configuration&);
    virtual ~TiedTextDependentMixtureSetTrainer() {}
};

}  // namespace Speech

#endif  // _SPEECH_ACOUSTIC_MODEL_TRAINER_HH
