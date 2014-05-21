/*
 * Copyright (C) 2006-2013  Music Technology Group - Universitat Pompeu Fabra
 *
 * This file is part of Essentia
 *
 * Essentia is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License as published by the Free
 * Software Foundation (FSF), either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the Affero GNU General Public License
 * version 3 along with this program.  If not, see http://www.gnu.org/licenses/
 */

#ifndef ESSENTIA_VECTORINPUTSTRAIGHT_H
#define ESSENTIA_VECTORINPUTSTRAIGHT_H

#include "../streamingalgorithm.h"

namespace essentia {
namespace streaming {


template <typename TokenType>
class VectorInputStraight : public Algorithm {
 protected:
  Source<std::vector<TokenType> > _output;

  const std::vector<TokenType>* _inputVector;
  bool _ownVector;
  int _idx;

 public:

  VectorInputStraight(const std::vector<TokenType>* input=0, bool own = false)
    : _inputVector(input), _ownVector(own) {
    setName("VectorInputStraight");
    declareOutput(_output,1,1,  "data", "the values read from the vector");
    reset();
  }

  VectorInputStraight(std::vector<TokenType>* input, bool own = false)
    : _inputVector(input), _ownVector(own) {
    setName("VectorInputStraight");
    declareOutput(_output, 1,1 "data", "the values read from the vector");
    reset();
  }



  ~VectorInputStraight() {
    clear();
  }

  void clear() {
    if (_ownVector) delete _inputVector;
    _inputVector = 0;
  }

  /**
   * TODO: Should we make a copy of the vector here or only keep the ref?
   */
  void setVector(const std::vector<TokenType>* input, bool own=false) {
    clear();
    _inputVector = input;
    _ownVector = own;
  }

  void reset() {
    Algorithm::reset();
    _idx = 0;

  }

//   bool shouldStop() const {
//     return _idx >= (int)_inputVector->size();
//   }

  AlgorithmStatus process() {
    // no more data available in vector. shouldn't be necessary to check,
    // but it doesn't cost us anything to be sure
    EXEC_DEBUG("process()");
    if (shouldStop()) {
      return PASS;
    }



    EXEC_DEBUG("acquiring " << _output.acquireSize() << " tokens");
    AlgorithmStatus status = acquireData();

    if (status != OK) {
      if (status == NO_OUTPUT) {
        throw EssentiaException("VectorInputStraight: internal error: output buffer full");
      }
      // should never get there, right?
      return NO_INPUT;
    }

    TokenType* dest = (TokenType*)_output.getFirstToken();
    const TokenType* src = &((*_inputVector)[0]);
    int howmuch = _inputVector->size();
    fastcopy(dest, src, howmuch);


    releaseData();
    EXEC_DEBUG("released " << _output.releaseSize() << " tokens");

    return OK;
  }

  void declareParameters() {}

};

template <typename T>
void connect(VectorInputStraight<T>& v, SinkBase& sink) {
  // optimization: if the sink we're connected to requires a lot of samples at once,
  // we might as well wait to have them all instead of feeding it them one by one
  int size = sink.acquireSize();
  SourceBase& visource = v.output("data");
  if (visource.acquireSize() < size) {
    visource.setAcquireSize(size);
    visource.setReleaseSize(size);
  }
  connect(v.output("data"), sink);
}

template <typename T>
void operator>>(VectorInputStraight<T>& v, SinkBase& sink) {
  connect(v, sink);
}



} // namespace streaming
} // namespace essentia

#endif // ESSENTIA_VECTORINPUT_H
