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

#include <iostream>
#include <Core/Dataflow/Network/Module.h>
#include <Core/Dataflow/Network/PortManager.h>

using namespace SCIRun::Domain::Networks;

Module::Module(const std::string& name, const std::string& cat/* ="unknown" */, const std::string& pack/* ="unknown" */, const std::string& version/* ="1.0" */)
  : module_name_(name)
{

}

Module::~Module()
{

}

OutputPortHandle Module::get_output_port(size_t idx) const
{
  return oports_[idx];
}

InputPortHandle Module::get_input_port(size_t idx) const
{
  return iports_[idx];
}

size_t Module::num_input_ports() const
{
  return iports_.size();
}

size_t Module::num_output_ports() const
{
  return oports_.size();
}

void Module::execute()
{
  std::cout << "Module " << get_modulename() << " executing..." << std::endl;
}

void Module::add_input_port(InputPortHandle h)
{
  iports_.add(h);
}

void Module::add_output_port(OutputPortHandle h)
{
  oports_.add(h);
}

Module::Builder::Builder() {}

Module::Builder& Module::Builder::with_name(const std::string& name)
{
  if (!module_)
    module_.reset(new Module(name));
  return *this;
}

Module::Builder& Module::Builder::add_input_port(const Port::ConstructionParams& params)
{
  if (module_)
  {
    InputPortHandle port(new InputPort(module_.get(), params));
    module_->add_input_port(port);
  }
  return *this;
}

Module::Builder& Module::Builder::add_output_port(const Port::ConstructionParams& params)
{
  if (module_)
  {
    OutputPortHandle port(new OutputPort(module_.get(), params));
    module_->add_output_port(port);
  }
  return *this;
}

ModuleHandle Module::Builder::build()
{
  return module_;
}