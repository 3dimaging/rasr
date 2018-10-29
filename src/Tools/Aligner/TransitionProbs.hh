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
#ifndef TRANSITION_PROBS
#define TRANSITION_PROBS

#include <Fsa/Automaton.hh>
#include <vector>

typedef std::vector<double> TransitionProbs;

//struct TransitionProbs {
//   double d;
//   double h;
//   double v;
//   double exponent; //(log-linear) exponent of transition probabilities
//   operator bool() const {
//      return (d>0 && h>0 && v>0 && exponent >0);
//   }
//};

#endif