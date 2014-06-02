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

#ifndef ESSENTIA_STREAMING_RingBufferRt_H
#define ESSENTIA_STREAMING_RingBufferRt_H

#include "../streamingalgorithm.h"
#include "ringbufferimpl.h"

using namespace std;
namespace essentia {
namespace streaming {

class RingBufferRt : public Algorithm {
 protected:
  Source<Real> _output;
  class RingBufferImpl* _impl;

 public:
  RingBufferRt();
  ~RingBufferRt();

void add(Real* inputData, int size)
{
	//std::cerr << "adding " << size << " to ringbuffer with space " << _impl->_space << std::endl;
	int added = _impl->add(inputData,size);
	if (added < size) throw EssentiaException("Not enough space in ringbuffer at input");
}
AlgorithmStatus process() {
  //std::cerr << "RingBufferRt waiting" << std::endl;
  _impl->waitAvailable();
  //std::cerr << "RingBufferRt waiting done" << std::endl;

  AlgorithmStatus status = acquireData();

  if (status != OK) {
    //std::cerr << "leaving the RingBufferRt while loop" << std::endl;
    if (status == NO_OUTPUT) throw EssentiaException("internal error: output buffer full");
    return status;
  }

  vector<AudioSample>& outputSignal = _output.tokens();
  AudioSample* outputData = &(outputSignal[0]);
  int outputSize = outputSignal.size();

  //std::cerr << "RingBufferRt getting" << outputSize << endl;
  int size = _impl->get(outputData, outputSize);
  //std::cerr << "got " << size << " from ringbuffer with space " << _impl->_space << std::endl;

  _output.setReleaseSize(size);
  releaseData();

  assert(size);

  return OK;
}


  void shouldStop(bool stop) {
    E_DEBUG(EExecution, "RBI should stop...");
  }

  void declareParameters() {
    declareParameter("bufferSize", "the size of the ringbuffer", "", 8192);
  }

  void configure();
  void reset();

  static const char* name;
  static const char* description;

};

} // namespace streaming
} // namespace essentia

#endif // ESSENTIA_STREAMING_RingBufferRt_H
