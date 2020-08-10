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
#ifndef CUDADATASTRUCTURE_HH_
#define CUDADATASTRUCTURE_HH_

#include <Math/CublasWrapper.hh>
#include <Math/CudaWrapper.hh>

namespace Math {

/*
 * CudaDataStructure
 *
 * base class for all data structures that require initialization of Cuda resources
 * availability of GPUs is checked here once
 * cublas handle is created here once
 *
 */

class CudaDataStructure {
private:
    static bool initialized;
    static bool hasGpu_;
    static int  activeGpu_;

protected:
    const bool            gpuMode_;
    static cublasHandle_t cublasHandle;
    // create a single random number generator
    static curandGenerator_t randomNumberGenerator;
    static u32               multiPrecisionBunchSize;
    static void              initialize();
    static void              log(const std::string& msg);
    static void              warning(const std::string& msg);
    static void              error(const std::string& msg);
    static void              criticalError(const std::string& msg);

public:
    // constructor with memory allocation
    CudaDataStructure();
    CudaDataStructure(const CudaDataStructure& x);
    // multiprecision bunch size
    static u32 getMultiprecisionBunchSize() {
        return multiPrecisionBunchSize;
    }
    static void setMultiprecisionBunchSize(u32 val);
    static bool hasGpu();
    static int  getActiveGpu();  // only allowed after hasGpu() == true
};

}  // namespace Math

#endif /* CUDADATASTRUCTURE_HH_ */
