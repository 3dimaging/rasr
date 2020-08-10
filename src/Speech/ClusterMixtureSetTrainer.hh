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
#ifndef _SPEECH_CLUSTER_MIXTURE_SET_TRAINER_HH
#define _SPEECH_CLUSTER_MIXTURE_SET_TRAINER_HH

#include "DiscriminativeMixtureSetTrainer.hh"

namespace Speech {

/**
 * Mixture set estimator to cluster a mixture set.
 */
class ClusterMixtureSetTrainer : public ConvertMixtureSetTrainer {
    typedef ConvertMixtureSetTrainer Precursor;

public:
    ClusterMixtureSetTrainer(const Core::Configuration&);
    virtual ~ClusterMixtureSetTrainer();
    virtual void cluster();
};

}  // namespace Speech

#endif  // _SPEECH_CLUSTER_MIXTURE_SET_TRAINER_HH
