/** Copyright 2018 RWTH Aachen University. All rights reserved.
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
#ifndef _NN_NEURAL_NETWORK_TRAINER_HH
#define _NN_NEURAL_NETWORK_TRAINER_HH

#include <Core/Component.hh>
#include <Speech/AlignedFeatureProcessor.hh>  // for supervised training
#include <Speech/CorpusVisitor.hh>            // for unsupervised training

#include "Criterion.hh"
#include "Estimator.hh"
#include "NeuralNetwork.hh"  // neural network
#include "Regularizer.hh"
#include "Types.hh"

#include <Math/CudaVector.hh>
#include <Math/Vector.hh>
#include <memory>
#include <vector>

namespace Bliss {
class Segment;
class SpeechSegment;
}  // namespace Bliss

namespace Core {
class Archive;
}

namespace Nn {

//=============================================================================
/**
 *  Base class for any neural network trainer.
 *
 * Unsupervised trainers are instantiated in BufferedFeatureExtractor,
 * supervised trainers are instantiated in BufferedAlignedFeatureProcessor
 * or BufferedSegmentFeatureProcessor.
 *
 * "trainer" is maybe a bad word, because it is more like the base class
 * for any possible action we can do on the features - however, training
 * a NN is the most common one, so this base class already comes with
 * these instances:
 *
 *  - NeuralNetwork (optional in some cases)
 *  - Criterion     (not used in all cases)
 *  - Estimator     (not used in all cases)
 *  - Regularizer   (not used in all cases)
 *
 * For Criterion, Estimator and Regularizer, there exists dummy types
 * which will be taken by default if you don't need it.
 *
 */
template<class T>
class NeuralNetworkTrainer : virtual public Core::Component {
    typedef Core::Component             Precursor;
    typedef typename Types<T>::NnVector NnVector;
    typedef typename Types<T>::NnMatrix NnMatrix;

public:
    enum TrainerType {
        dummy,
        // supervised trainer types
        feedForwardTrainer,
        frameClassificationErrorAccumulator,
        // unsupervised trainer types
        //  .. currently nothing
        //
        // can be used in both modes
        meanAndVarianceAccumulator,
        networkEvaluator,
        autoencoderTrainer,
        pythonTrainer,
        pythonEvaluator,
    };

public:
    static const Core::Choice          choiceNetworkTrainer;
    static const Core::ParameterChoice paramNetworkTrainer;
    static const Core::ParameterInt    paramEpoch;
    static const Core::ParameterBool   paramWeightedAccumulation;  // relevant for supervised training
    static const Core::ParameterBool   paramMeasureTime;

protected:
    Criterion<T>* criterion_;
    // perform weighted accumulation (according to class-weights, relevant for supervised training only)
    // can be set from outside, therefore not const
    bool weightedAccumulation_;
    // needed for prior estimation from class counts, passed by BufferedAlignedFeatureProcessor
    const Math::Vector<T>* classWeights_;
    // measure runtime
    const bool measureTime_;
    // depends on trainer, but normally true
    bool needsNetwork_;

protected:
    mutable Core::XmlChannel statisticsChannel_;  // statistics-channel
    bool                     needInit_;
    // neural network to be trained, use pointer in order to pass a network from outside
    NeuralNetwork<T>* network_;
    // estimator for weights update
    Estimator<T>* estimator_;
    // Regularizer
    Regularizer<T>* regularizer_;

public:
    NeuralNetworkTrainer(const Core::Configuration& config);
    virtual ~NeuralNetworkTrainer();

    // initialization & finalization methods
    virtual void initializeTrainer(u32 batchSize);
    virtual void initializeTrainer(u32 batchSize, std::vector<u32>& streamSizes);
    virtual void finalize();

    // getter and setter methods

    /** get activations of output layer */
    NnMatrix& getOutputActivation() {
        require(network_);
        return network_->getTopLayerOutput();
    }
    /** returns whether trainer has a network */
    bool hasNetwork() const {
        return (network_ != 0);
    }
    /** returns whether trainer has an estimator */
    bool hasEstimator() const {
        return (estimator_ != 0);
    }
    /** returns whether frame weights are used */
    bool weightedAccumulation() const {
        return weightedAccumulation_;
    }
    /** returns current batch size (equal to size of activations)*/
    u32 batchSize() const {
        return hasNetwork() ? network_->activationsSize() : 0u;
    }
    /** returns whether trainer is initialized */
    bool isInitialized() const {
        return !needInit_;
    }
    /** returns whether trainer measures computation time */
    bool measuresTime() const {
        return measureTime_;
    }
    Criterion<T>& criterion() {
        require(criterion_);
        return *criterion_;
    }
    /** return reference to network */
    NeuralNetwork<T>& network() {
        require(network_);
        return *network_;
    }
    const NeuralNetwork<T>& network() const {
        require(network_);
        return *network_;
    }
    /** return reference to network */
    u32 nLayers() const {
        return hasNetwork() ? network_->nLayers() : 0;
    }
    /** return reference to estimator */
    Estimator<T>& estimator() {
        require(estimator_);
        return *estimator_;
    }
    const Estimator<T>& estimator() const {
        require(estimator_);
        return *estimator_;
    }
    /** return reference to regularizer */
    Regularizer<T>& regularizer() {
        require(regularizer_);
        return *regularizer_;
    }
    const Regularizer<T>& regularizer() const {
        require(regularizer_);
        return *regularizer_;
    }
    /** sets class weights to values in vector*/
    void setClassWeights(const Math::Vector<T>* vector);

    /** returns whether trainer needs to process all features, i.e. for batch training */
    virtual bool needsToProcessAllFeatures() const {
        return false;
    }
    /** resize activations (necessary when batch size has changed) */
    virtual void setBatchSize(u32 batchSize);

    /** if we have a network, is its output layer representing the class label posterior probabilities */
    virtual bool isNetworkOutputRepresentingClassLabels() {
        return true;
    }
    /** whether we can call getClassLabelPosteriors() */
    virtual bool hasClassLabelPosteriors() {
        if (hasNetwork() && isNetworkOutputRepresentingClassLabels())
            return true;
        return false;
    }
    /** returns the posteriors after a call to processBatch_feedInput() */
    virtual NnMatrix& getClassLabelPosteriors() {
        require(hasNetwork());
        require(isNetworkOutputRepresentingClassLabels());
        return getOutputActivation();
    }
    /** returns posterior dimension */
    virtual int getClassLabelPosteriorDimension() {
        require(hasNetwork());
        require(isNetworkOutputRepresentingClassLabels());
        return network().getTopLayer().getOutputDimension();
    }

    virtual bool allowsDownsampling() const {
        return false;
    }

    // interface methods

    /** Override this method for the NN forward pass in NN training.
     * The weights are for the individual frames. Note that the pointer has
     * to stay valid until the processBatch_finish* call. Can be NULL.
     * The segment is only given if the calling feature extractor is
     * in segmentwise mode. Otherwise it's NULL.
     */
    virtual void processBatch_feedInput(std::vector<NnMatrix>& features, NnVector* weights, Bliss::Segment* segment) {}

    /** Override this method for supervised training.
     * It's usually used for backprop + collects the gradient,
     * where error is the accumulated value of the objective function,
     * and errorSignal is the gradient of the objective function.
     * If you use this directly, you ignore the training criterion.
     */
    virtual void processBatch_finishWithError(T error, NnMatrix& errorSignal) {
        // This default implementation just uses the backprop implementation of the layer.
        verify(network_);
        NnMatrix outErrorSignal(errorSignal.nRows(), errorSignal.nColumns());
        network().getTopLayer().backpropagateActivations(errorSignal, outErrorSignal, network().getTopLayerOutput());
        processBatch_finishWithError_naturalPairing(error, outErrorSignal);
    }

    /** This is a special variant of processBatch_finishWithError(), where the error signal
     * is already related to the natural pairing of the objective function with the last layer
     * activation function.
     */
    virtual void processBatch_finishWithError_naturalPairing(T error, NnMatrix& errorSignal) {
        processBatch_finish();
    }

    /** Override this method for supervised training with a criterion based on an alignment */
    virtual void processBatch_finishWithAlignment(Math::CudaVector<u32>& alignment) {
        processBatch_finish();
    }

    /** Override this method for supervised training with a criterion based on a segment */
    virtual void processBatch_finishWithSpeechSegment(Bliss::SpeechSegment& segment) {
        processBatch_finish();
    }

    /** Override this method for unsupervised training to finish a minibatch */
    virtual void processBatch_finish() {}

    // reset history (for recurrent networks)
    virtual void resetHistory();

    virtual void logBatchTimes() const {}

protected:
    // log configuration
    virtual void logProperties() const;

public:
    // factory methods
    static NeuralNetworkTrainer<T>* createSupervisedTrainer(const Core::Configuration& config);
    static NeuralNetworkTrainer<T>* createUnsupervisedTrainer(const Core::Configuration& config);
};

//=============================================================================
/** only computes frame classification error and objective function
 * (supervised trainer)
 */
template<class T>
class FrameErrorEvaluator : public NeuralNetworkTrainer<T> {
    typedef NeuralNetworkTrainer<T> Precursor;

protected:
    typedef typename Types<T>::NnVector NnVector;
    typedef typename Types<T>::NnMatrix NnMatrix;

protected:
    using Precursor::statisticsChannel_;
    NnVector* weights_;  // weights of last feedInput call
    u32       nObservations_;
    u32       nFrameClassificationErrors_;
    T         objectiveFunction_;
    bool      logFrameEntropy_;
    T         frameEntropy_;

public:
    static const Core::ParameterBool paramLogFrameEntropy;

    FrameErrorEvaluator(const Core::Configuration& config);
    virtual ~FrameErrorEvaluator() {}
    NeuralNetwork<T>& network() {
        return Precursor::network();
    }
    virtual void finalize();
    virtual void processBatch_feedInput(std::vector<NnMatrix>& features, NnVector* weights, Bliss::Segment* segment);
    virtual void processBatch_finishWithAlignment(Math::CudaVector<u32>& alignment);
    virtual void processBatch_finishWithSpeechSegment(Bliss::SpeechSegment& segment);
    virtual void processBatch_finish();
    virtual bool needsToProcessAllFeatures() const {
        return true;
    }
};

//=============================================================================
/** computes mean and variance of input features
 *
 *  implemented as a NN trainer in order to get GPU support and
 *  to reuse the configuration of the network
 */
template<class T>
class MeanAndVarianceTrainer : public NeuralNetworkTrainer<T> {
    typedef NeuralNetworkTrainer<T> Precursor;

protected:
    typedef typename Types<T>::NnVector NnVector;
    typedef typename Types<T>::NnMatrix NnMatrix;

protected:
    using Precursor::statisticsChannel_;

public:
    static const Core::ParameterString paramMeanFile;
    static const Core::ParameterString paramStandardDeviationFile;
    static const Core::ParameterString paramStatisticsFile;

protected:
    NnVector*       weights_;  // weights of last feedInput call
    Statistics<T>*  statistics_;
    Math::Vector<T> mean_;
    Math::Vector<T> standardDeviation_;
    NnMatrix        tmp_;

    std::string meanFile_;
    std::string standardDeviationFile_;
    std::string statisticsFile_;

public:
    MeanAndVarianceTrainer(const Core::Configuration& config);
    virtual ~MeanAndVarianceTrainer();

    /** initialization method */
    virtual void initializeTrainer(u32 batchSize, std::vector<u32>& streamSizes);

    /** write statistics */
    virtual void finalize();
    /** compute mean and standard deviation and write it to file */
    virtual void writeMeanAndStandardDeviation(Statistics<T>& statistics);
    /** accumulates mean and variance on mini-batch */
    virtual void processBatch_feedInput(std::vector<NnMatrix>& features, NnVector* weights, Bliss::Segment* segment);
    virtual bool needsToProcessAllFeatures() const {
        return true;
    }

private:
    void saveVector(std::string& filename, Math::Vector<T>& vector);
};

//=============================================================================
/** only forwards through the network and dumps the NN output (= emission label posteriors)
 */
template<class T>
class NetworkEvaluator : public NeuralNetworkTrainer<T> {
    typedef NeuralNetworkTrainer<T> Precursor;

protected:
    typedef typename Types<T>::NnVector NnVector;
    typedef typename Types<T>::NnMatrix NnMatrix;
    static const Core::ParameterString  paramDumpPosteriors;
    static const Core::ParameterString  paramDumpBestPosteriorIndices;
    u32                                 nObservations_;
    std::shared_ptr<Core::Archive>      dumpPosteriorsArchive_;
    std::shared_ptr<Core::Archive>      dumpBestPosterioIndicesArchive_;

public:
    NetworkEvaluator(const Core::Configuration& config);
    virtual ~NetworkEvaluator() {}
    NeuralNetwork<T>& network() {
        return Precursor::network();
    }
    virtual void finalize();
    virtual void processBatch_feedInput(std::vector<NnMatrix>& features, NnVector* weights, Bliss::Segment* segment);
    virtual void processBatch_finishWithSpeechSegment(Bliss::SpeechSegment& segment);
    virtual void processBatch_finish();
    virtual bool needsToProcessAllFeatures() const {
        return true;
    }
};

}  // namespace Nn

#endif  // _NN_NEURAL_NETWORK_TRAINER_HH
