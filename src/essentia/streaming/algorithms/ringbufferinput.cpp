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

#include "ringbufferinput.h"
#include "ringbufferimpl.h"
#include "sourcebase.h"
#include "atomic.h"



namespace essentia {
namespace streaming {

const char* RingBufferInput::name = "RingBufferInput";
const char* RingBufferInput::description = DOC(
"This algorithm gets data from an input ringbuffer of type Real that is fed into the essentia streaming mode."
);

RingBufferInput::RingBufferInput():_impl(0)
{
  declareOutput(_output, 4096, "signal", "data source of what's coming from the ringbuffer");
  _output.setBufferType(BufferUsage::forAudioStream);
}

RingBufferInput::~RingBufferInput()
{
	delete _impl;
}

void RingBufferInput::configure()
{
	delete _impl;
	_impl = new RingBufferImpl(RingBufferImpl::kAvailable,parameter("bufferSize").toInt());
	
	_output.setAcquireSize(parameter("blockSize").toInt());
}

void RingBufferInput::add(Real* inputData, int size)
{
	EXEC_DEBUG("adding " << size << " to ringbuffer with space " << _impl->_space);
	int added = _impl->add(inputData,size);
	if (added < size) throw EssentiaException("Not enough space in ringbuffer at input");
}

AlgorithmStatus RingBufferInput::process() {
  //std::cerr << "ringbufferinput waiting" << std::endl;
  _impl->waitAvailable();
  //std::cerr << "ringbufferinput waiting done" << std::endl;

  AlgorithmStatus status = acquireData();

  if (status != OK) {
    EXEC_DEBUG("leaving the ringbufferinput while loop");
    if (status == NO_OUTPUT) throw EssentiaException("internal error: output buffer full");
    return status;
  }

  vector<AudioSample>& outputSignal = _output.tokens();
  AudioSample* outputData = &(outputSignal[0]);
  int outputSize = outputSignal.size();

  EXEC_DEBUG("ringbufferinput getting" << outputSize);
  int size = _impl->get(outputData, outputSize);
  EXEC_DEBUG( "got " << size << " from ringbuffer with space " << _impl->_space );

  _output.setReleaseSize(size);
  releaseData();

  assert(size);

  return OK;
}

void RingBufferInput::reset() {
  Algorithm::reset();
  _impl->reset();
}

} // namespace streaming
} // namespace essentia
