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

#include <Dataflow/Serialization/Network/NetworkXMLSerializer.h>
#include <Dataflow/Serialization/Network/NetworkDescriptionSerialization.h>
#include <Dataflow/Network/Network.h> //TODO: need network factory??
#include <Dataflow/Network/ModuleInterface.h>
#include <Dataflow/Serialization/Network/XMLSerializer.h>
#include <fstream>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/foreach.hpp>

using namespace SCIRun::Dataflow::Networks;
using namespace SCIRun::Dataflow::State;
using namespace SCIRun::Core::Algorithms;

NetworkXMLConverter::NetworkXMLConverter(ModuleFactoryHandle moduleFactory, ModuleStateFactoryHandle stateFactory, AlgorithmFactoryHandle algoFactory, ModulePositionEditor* mpg)
  : moduleFactory_(moduleFactory), stateFactory_(stateFactory), algoFactory_(algoFactory), mpg_(mpg)
{
}

NetworkHandle NetworkXMLConverter::from_xml_data(const NetworkXML& data)
{
  NetworkHandle network(boost::make_shared<Network>(moduleFactory_, stateFactory_, algoFactory_));

  BOOST_FOREACH(const ModuleMapXML::value_type& modPair, data.modules)
  {
    ModuleHandle module = network->add_module(modPair.second.module);
    module->set_id(modPair.first);
    ModuleStateHandle state(modPair.second.state.clone());
    module->set_state(state);
  }

  BOOST_FOREACH(const ConnectionDescriptionXML& conn, data.connections)
  {
    ModuleHandle from = network->lookupModule(conn.out_.moduleId_);
    ModuleHandle to = network->lookupModule(conn.in_.moduleId_);
    network->connect(ConnectionOutputPort(from, conn.out_.port_), ConnectionInputPort(to, conn.in_.port_));
  }

  return network;
}

NetworkToXML::NetworkToXML(ModulePositionEditor* mpg) 
  : mpg_(mpg)
{}

NetworkFileHandle NetworkXMLConverter::to_xml_data(const NetworkHandle& network)
{
  return NetworkToXML(mpg_).to_xml_data(network);
}

NetworkFileHandle NetworkToXML::to_xml_data(const NetworkHandle& network)
{
  NetworkXML networkXML;
  Network::ConnectionDescriptionList conns = network->connections();
  BOOST_FOREACH(ConnectionDescription& desc, conns)
    networkXML.connections.push_back(ConnectionDescriptionXML(desc));
  for (size_t i = 0; i < network->nmodules(); ++i)
  {
    ModuleHandle module = network->module(i);
    ModuleStateHandle state = module->get_state();
    boost::shared_ptr<SimpleMapModuleStateXML> stateXML = make_state_xml(state);
    networkXML.modules[module->get_id()] = ModuleWithState(module->get_info(), stateXML ? *stateXML : SimpleMapModuleStateXML());
  }

  NetworkFileHandle file(boost::make_shared<NetworkFile>());
  file->network = networkXML;
  if (mpg_)
    file->modulePositions = *mpg_->dumpModulePositions();
  return file;
}


void NetworkXMLSerializer::save_xml(const NetworkXML& data, const std::string& filename)
{
  XMLSerializer::save_xml(data, filename, "network");
}

void NetworkXMLSerializer::save_xml(const NetworkXML& data, std::ostream& ostr)
{
  XMLSerializer::save_xml(data, ostr, "network");
}

NetworkXMLHandle NetworkXMLSerializer::load_xml(const std::string& filename)
{
  return XMLSerializer::load_xml<NetworkXML>(filename);
}

NetworkXMLHandle NetworkXMLSerializer::load_xml(std::istream& istr)
{
  return XMLSerializer::load_xml<NetworkXML>(istr);
}
