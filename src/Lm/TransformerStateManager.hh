#ifndef _LM_TRANSFORMER_STATE_MANAGER_HH
#define _LM_TRANSFORMER_STATE_MANAGER_HH

#include "StateManager.hh"

namespace Lm {

template<typename T>
class TransformerStateManager : public StateManager {
public:
    using Precursor = StateManager;

    static const Core::ParameterInt  paramMaxHistoryLength;
    static const Core::ParameterBool paramAlwaysIncludeFirstTokenState;

    TransformerStateManager(Core::Configuration const& config);
    virtual ~TransformerStateManager() = default;

    virtual bool requiresAllParentStates() const;

    virtual HistoryState              initialState(StateVariables const& vars, CompressedVectorFactory<float> const& vector_factory);
    virtual void                      mergeStates (StateVariables const& vars,
                                                   std::vector<size_t>& prefix_lengths,
                                                   std::vector<HistoryState const*> const& prefix_states,
                                                   FeedDict& feed_dict,
                                                   TargetList& targets);
    virtual std::vector<HistoryState> splitStates (StateVariables const& vars,
                                                   std::vector<size_t>& suffix_lengths,
                                                   std::vector<Tensorflow::Tensor> const& state_tensors,
                                                   CompressedVectorFactory<float> const& vector_factory);
protected:
    size_t maxHistory_;
    bool   alwaysIncludeFirstTokenState_;
};

template<typename T>
class TransformerStateManagerWithCommonPrefix : public TransformerStateManager<T> {
public:
    using Precursor = TransformerStateManager<T>;

    static const Core::ParameterString paramVarName;
    static const Core::ParameterString paramCommonPrefixInitialValue;
    static const Core::ParameterString paramCommonPrefixInitializer;
    static const Core::ParameterInt    paramMinBatchSize;
    static const Core::ParameterInt    paramMinCommonPrefixLength;

    TransformerStateManagerWithCommonPrefix(Core::Configuration const& config);
    virtual ~TransformerStateManagerWithCommonPrefix() = default;

    virtual void mergeStates(typename Precursor::StateVariables const& vars,
                             std::vector<size_t>& prefix_lengths,
                             std::vector<typename Precursor::HistoryState const*> const& prefix_states,
                             typename Precursor::FeedDict& feed_dict,
                             typename Precursor::TargetList& targets);
protected:
    std::unordered_map<std::string, std::pair<std::string, std::string>> varMap_;

    size_t minBatchSize_;
    size_t minCommonPrefixLength_;
};


// inline implementations

template<typename T>
inline TransformerStateManager<T>::TransformerStateManager(Core::Configuration const& config) : Precursor(config),
                                                                                                maxHistory_(paramMaxHistoryLength(config)),
                                                                                                alwaysIncludeFirstTokenState_(paramAlwaysIncludeFirstTokenState(config)) {
}

template<typename T>
inline bool TransformerStateManager<T>::requiresAllParentStates() const {
    return true;
}


template<typename T>
inline TransformerStateManagerWithCommonPrefix<T>::TransformerStateManagerWithCommonPrefix(Core::Configuration const& config) : Precursor(config),
                                                                                                                                minBatchSize_(paramMinBatchSize(config)),
                                                                                                                                minCommonPrefixLength_(paramMinCommonPrefixLength(config)) {
    Core::Configuration varmap_config = this->select("var-map");
    for (size_t i = 0ul; true; i++) {
        Core::Configuration idx_config(varmap_config, std::string("item-") + std::to_string(i));
        std::string var_name      = paramVarName(idx_config);
        std::string initial_value = paramCommonPrefixInitialValue(idx_config);
        std::string initializer   = paramCommonPrefixInitializer(idx_config);
        if (not var_name.empty()) {
            varMap_[var_name] = std::make_pair<>(initial_value, initializer);
        }
        else {
            break;
        }
    }
}

}  // namespace Lm

#endif  // _LM_TRANSFORMER_STATE_MANAGER_HH
