#include "CompressedVector.hh"

namespace Lm {

ContiguousBlockInfo::ContiguousBlockInfo(std::gslice const& slice) : start_(slice.start()),
                                                                     sizes_(slice.size()),
                                                                     strides_(slice.size()),
                                                                     totalSize_(0ul),
                                                                     numBlocks_(1ul),
                                                                     blockSize_(1ul),
                                                                     firstIdxDim_(-1) {
    size_t i                 = sizes_.size();
    size_t contiguous_stride = 1ul;
    while (i > 0ul) {
        i -= 1ul;
        bool contiguous = (contiguous_stride == strides_[i]);
        if (contiguous) {
            firstIdxDim_ = std::max<int>(firstIdxDim_, i);
        }
        numBlocks_ *= contiguous ? 1ul : sizes_[i];
        blockSize_ *= contiguous ? sizes_[i] : 1ul;
        contiguous_stride *= sizes_[i];
    }

    if (sizes_.size() > 0ul) {
        totalSize_ = 1ul;
        for (size_t i = 0ul; i < sizes_.size(); i++) {
            totalSize_ *= sizes_[i];
        }
    }
}

size_t ContiguousBlockInfo::blockOffset(size_t idx) const {
    size_t res = start_;
    for (int i = firstIdxDim_; i >= 0; i--) {
        res += strides_[i] * (idx % sizes_[i]);
        idx /= sizes_[i];
    }

    return res;
}

}  // namespace Lm
