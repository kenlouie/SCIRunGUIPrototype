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

#include <Python.h>
#include <iostream>
#include <Dataflow/Engine/Python/NetworkEditorPythonInterface.h>
#include <Dataflow/Engine/Python/NetworkEditorPythonAPI.h>
#include <Dataflow/Engine/Python/SCIRunPythonModule.h>

using namespace SCIRun::Dataflow::Engine;
using namespace SCIRun::Dataflow::Networks;

boost::shared_ptr<NetworkEditorPythonInterface> NetworkEditorPythonAPI::impl_;
ExecutableLookup* NetworkEditorPythonAPI::lookup_ = 0;

void NetworkEditorPythonAPI::setImpl(boost::shared_ptr<NetworkEditorPythonInterface> impl) 
{
  impl_ = impl;
}

void NetworkEditorPythonAPI::setExecutionContext(ExecutableLookup* lookup)
{
  lookup_ = lookup;
}

std::string NetworkEditorPythonAPI::addModule(const std::string& name)
{
  if (impl_)
    return impl_->addModule(name);
  else
  {
    return "Null implementation: NetworkEditorPythonAPI::addModule()";
  }
}

std::string NetworkEditorPythonAPI::removeModule(const std::string& id)
{
  if (impl_)
    return impl_->removeModule(id);
  else
  {
    return "Null implementation: NetworkEditorPythonAPI::removeModule()";
  }
}

std::string NetworkEditorPythonAPI::executeAll()
{
  if (impl_ && lookup_)
    return impl_->executeAll(*lookup_);
  else
  {
    return "Null implementation or execution context: NetworkEditorPythonAPI::executeAll()"; 
  }
}

std::string NetworkEditorPythonAPI::connect(const std::string& moduleId1, int port1, const std::string& moduleId2, int port2)
{
  if (impl_)
    return impl_->connect(moduleId1, port1, moduleId2, port2);
  else
  {
    return "Null implementation: NetworkEditorPythonAPI::connect()";
  }
}

std::string NetworkEditorPythonAPI::disconnect(const std::string& moduleId1, int port1, const std::string& moduleId2, int port2)
{
  if (impl_)
    return impl_->disconnect(moduleId1, port1, moduleId2, port2);
  else
  {
    return "Null implementation: NetworkEditorPythonAPI::disconnect()";
  }
}

std::string NetworkEditorPythonAPI::saveNetwork(const std::string& filename)
{
  if (impl_)
    return impl_->saveNetwork(filename);
  else
  {
    return "Null implementation: NetworkEditorPythonAPI::saveNetwork()";
  }
}

std::string NetworkEditorPythonAPI::loadNetwork(const std::string& filename)
{
  if (impl_)
    return impl_->loadNetwork(filename);
  else
  {
    return "Null implementation: NetworkEditorPythonAPI::loadNetwork()";
  }
}

std::string NetworkEditorPythonAPI::quit(bool force)
{
  if (impl_)
    return impl_->quit(force);
  else
  {
    return "Null implementation: NetworkEditorPythonAPI::quit()";
  }
}