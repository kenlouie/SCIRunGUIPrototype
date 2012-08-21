/*
   For more information, please see: http://software.sci.utah.edu

   The MIT License

   Copyright (c) 2012 Scientific Computing and Imaging Institute,
   University of Utah.

   License for the specific language governing rights and limitations under
   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.
*/

#include <Dataflow/Network/NullModuleState.h>

using namespace SCIRun::Engine::State;
  
void NullModuleState::setValue(const Name&, const SCIRun::Algorithms::AlgorithmParameter::Value&)
{
}

const NullModuleState::Value NullModuleState::getValue(const Name&) const
{
  return Value();
}

NullModuleState::Keys NullModuleState::getKeys() const
{
  return Keys();
}

boost::signals::connection NullModuleState::connect_state_changed(state_changed_sig_t::slot_function_type subscriber)
{
  return boost::signals::connection();
}

const NullModuleState::TransientValue NullModuleState::getTransientValue(const std::string& name) const
{
  return TransientValue();
}

void NullModuleState::setTransientValue(const std::string& name, const TransientValue& value)
{
}