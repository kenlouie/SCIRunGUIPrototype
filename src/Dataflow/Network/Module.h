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


#ifndef DATAFLOW_NETWORK_MODULE_H
#define DATAFLOW_NETWORK_MODULE_H 

#include <boost/noncopyable.hpp>
#include <boost/static_assert.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>
#include <Core/Logging/LoggerInterface.h>
#include <Core/Datatypes/DatatypeFwd.h>
#include <Core/Datatypes/Mesh/FieldFwd.h>
#include <Core/Algorithms/Base/AlgorithmFwd.h>
#include <Dataflow/Network/NetworkFwd.h>
#include <Dataflow/Network/ModuleInterface.h>
#include <Dataflow/Network/ModuleStateInterface.h>
#include <Dataflow/Network/ModuleDescription.h>
#include <Dataflow/Network/PortManager.h>
#include <Dataflow/Network/share.h>

namespace SCIRun {
namespace Dataflow {
namespace Networks {
  
  class SCISHARE Module : public ModuleInterface, public Core::Logging::LoggerInterface, boost::noncopyable
  {
  public:
    Module(const ModuleLookupInfo& info, 
      bool hasUi = true, 
      Core::Algorithms::AlgorithmFactoryHandle algoFactory = defaultAlgoFactory_,
      ModuleStateFactoryHandle stateFactory = defaultStateFactory_,
      const std::string& version = "1.0");
    virtual ~Module();

    std::string get_module_name() const { return info_.module_name_; }
    std::string get_categoryname() const { return info_.category_name_; }
    std::string get_packagename() const { return info_.package_name_; }
    ModuleId get_id() const { return id_; }

    //for serialization
    virtual const ModuleLookupInfo& get_info() const { return info_; }
    virtual void set_id(const std::string& id) { id_ = ModuleId(id); }

    bool has_ui() const { return has_ui_; }
    void setUiVisible(bool visible); 
    size_t num_input_ports() const;
    size_t num_output_ports() const;

    InputPortHandle get_input_port(const std::string &name) const;
    OutputPortHandle get_output_port(const std::string &name) const;
    OutputPortHandle get_output_port(size_t idx) const;
    InputPortHandle get_input_port(size_t idx) const;

    //TODO: execute signal here.
    virtual void do_execute() throw(); //--C++11--will throw nothing
    virtual ModuleStateHandle get_state();
    virtual void set_state(ModuleStateHandle state);

  private:
    virtual SCIRun::Core::Datatypes::DatatypeHandleOption get_input_handle(size_t idx);
    virtual void send_output_handle(size_t idx, SCIRun::Core::Datatypes::DatatypeHandle data);

  public:
    virtual void setLogger(SCIRun::Core::Logging::LoggerHandle log);
    virtual SCIRun::Core::Logging::LoggerHandle getLogger() const;
    virtual void error(const std::string& msg) const { errorSignal_(id_); getLogger()->error(msg); }
    virtual void warning(const std::string& msg) const { getLogger()->warning(msg); }
    virtual void remark(const std::string& msg) const { getLogger()->remark(msg); }
    virtual void status(const std::string& msg) const { getLogger()->status(msg); }

    virtual SCIRun::Core::Algorithms::AlgorithmStatusReporter::UpdaterFunc getUpdaterFunc() const { return updaterFunc_; }
    virtual void setUpdaterFunc(SCIRun::Core::Algorithms::AlgorithmStatusReporter::UpdaterFunc func);
    virtual void setUiToggleFunc(UiToggleFunc func) { uiToggleFunc_ = func; }

    virtual boost::signals2::connection connectExecuteBegins(const ExecuteBeginsSignalType::slot_type& subscriber);
    virtual boost::signals2::connection connectExecuteEnds(const ExecuteEndsSignalType::slot_type& subscriber);
    virtual boost::signals2::connection connectErrorListener(const ErrorSignalType::slot_type& subscriber);

    virtual bool needToExecute() const  
    {
      return true; //TODO
    }

    virtual void setDefaults() {} //TODO should be pure virtual

    bool oport_connected(size_t portIndex) const;

    template <class Type, size_t N>
    struct PortName
    {
      explicit PortName(const std::string& name = "") : name_(name) {}
      operator size_t() const { return N; }
      operator std::string() const 
      { 
        if (name_.empty())
          BOOST_THROW_EXCEPTION(DataPortException() << SCIRun::Core::ErrorMessage("Port name not initialized!"));
        return name_; 
      }

      std::string name_;
    };

    // Throws if input is not present or null.
    template <class T, size_t N>
    boost::shared_ptr<T> getRequiredInput(const PortName<T,N>& port);

    template <class T, size_t N>
    boost::optional<boost::shared_ptr<T>> getOptionalInput(const PortName<T,N>& port);

    template <class T, class D, size_t N>
    void sendOutput(const PortName<T,N>& port, boost::shared_ptr<D> data);

    template <class T, size_t N>
    void sendOutputFromAlgorithm(const PortName<T,N>& port, const Core::Algorithms::AlgorithmOutput& output);

    class SCISHARE Builder : boost::noncopyable
    {
    public:
      Builder();
      Builder& with_name(const std::string& name);
      Builder& using_func(ModuleMaker create);
      Builder& add_input_port(const Port::ConstructionParams& params);
      Builder& add_output_port(const Port::ConstructionParams& params);
      Builder& disable_ui();
      ModuleHandle build();

      typedef boost::function<SCIRun::Dataflow::Networks::DatatypeSinkInterface*()> SinkMaker;
      typedef boost::function<SCIRun::Dataflow::Networks::DatatypeSourceInterface*()> SourceMaker;
      static void use_sink_type(SinkMaker func);
      static void use_source_type(SourceMaker func);
    private:
      boost::shared_ptr<Module> module_;
      static SinkMaker sink_maker_;
      static SourceMaker source_maker_;
    };

    //TODO: yuck
    static ModuleStateFactoryHandle defaultStateFactory_;
    static Core::Algorithms::AlgorithmFactoryHandle defaultAlgoFactory_;

  protected:
    ModuleLookupInfo info_;
    ModuleId id_;

    Core::Algorithms::AlgorithmHandle algo_;
    Core::Algorithms::AlgorithmFactoryHandle algoFactory_;

    enum State {
      NeedData,
      JustStarted,
      Executing,
      Completed
    };
    void update_state(State) { /*TODO*/ }

  private:
    template <class T>
    boost::shared_ptr<T> getRequiredInputAtIndex(size_t idx);
    template <class T>
    boost::optional<boost::shared_ptr<T>> getOptionalInputAtIndex(size_t idx);

    friend class Builder;
    size_t add_input_port(InputPortHandle);
    size_t add_output_port(OutputPortHandle);
    bool has_ui_;
   
    ModuleStateHandle state_;
    PortManager<OutputPortHandle> oports_;
    PortManager<InputPortHandle> iports_;

    ExecuteBeginsSignalType executeBegins_;
    ExecuteEndsSignalType executeEnds_;
    ErrorSignalType errorSignal_;

    SCIRun::Core::Logging::LoggerHandle log_;
    SCIRun::Core::Algorithms::AlgorithmStatusReporter::UpdaterFunc updaterFunc_;
    UiToggleFunc uiToggleFunc_;
    static int instanceCount_;
    static SCIRun::Core::Logging::LoggerHandle defaultLogger_;
  };

  template <class T>
  boost::shared_ptr<T> Module::getRequiredInputAtIndex(size_t idx)
  {
    auto inputOpt = get_input_handle(idx);
    if (!inputOpt)
      MODULE_ERROR_WITH_TYPE(NoHandleOnPortException, "Input data required on port #" + boost::lexical_cast<std::string>(idx));

    if (!*inputOpt)
      MODULE_ERROR_WITH_TYPE(NullHandleOnPortException, "Null handle on port #" + boost::lexical_cast<std::string>(idx));

    boost::shared_ptr<T> data = boost::dynamic_pointer_cast<T>(*inputOpt);
    if (!data)
    {
      std::ostringstream ostr;
      ostr << "Wrong datatype on port #" << idx << "; expected " << typeid(T).name() << " but received " << typeid(*inputOpt).name();
      MODULE_ERROR_WITH_TYPE(WrongDatatypeOnPortException, ostr.str());
    }
    return data;
  }
  
  template <class T, size_t N>
  boost::shared_ptr<T> Module::getRequiredInput(const PortName<T,N>& port)
  {
    return getRequiredInputAtIndex<T>(static_cast<size_t>(port));
  }

  template <class T>
  boost::optional<boost::shared_ptr<T>> Module::getOptionalInputAtIndex(size_t idx)
  {
    auto inputOpt = get_input_handle(idx);
    if (!inputOpt)
      return boost::optional<boost::shared_ptr<T>>();

    if (!*inputOpt)
      MODULE_ERROR_WITH_TYPE(NullHandleOnPortException, "Null handle on port #" + boost::lexical_cast<std::string>(idx));

    boost::shared_ptr<T> data = boost::dynamic_pointer_cast<T>(*inputOpt);
    if (!data)
    {
      std::ostringstream ostr;
      ostr << "Wrong datatype on port #" << idx << "; expected " << typeid(T).name() << " but received " << typeid(*inputOpt).name();
      MODULE_ERROR_WITH_TYPE(WrongDatatypeOnPortException, ostr.str());
    }
    return data;
  }

  template <class T, size_t N>
  boost::optional<boost::shared_ptr<T>> Module::getOptionalInput(const PortName<T,N>& port)
  {
    return getOptionalInputAtIndex<T>(static_cast<size_t>(port));
  }

  template <class T, class D, size_t N>
  void Module::sendOutput(const PortName<T,N>& port, boost::shared_ptr<D> data)
  {
    const bool datatypeForThisPortMustBeCompatible = boost::is_base_of<T,D>::value;
    BOOST_STATIC_ASSERT(datatypeForThisPortMustBeCompatible);
    send_output_handle(static_cast<size_t>(port), data);
  }
  
  template <class T, size_t N>
  void Module::sendOutputFromAlgorithm(const PortName<T,N>& port, const Core::Algorithms::AlgorithmOutput& output)
  {
    sendOutput<T, T, N>(port, output.get<T>(Core::Algorithms::AlgorithmParameterName(port)));
  }

}}


namespace Modules
{

  struct SCISHARE MatrixPortTag {};
  struct SCISHARE ScalarPortTag {};
  struct SCISHARE StringPortTag {};
  struct SCISHARE FieldPortTag {};
  struct SCISHARE MeshPortTag {}; //TODO temporary
  struct SCISHARE GeometryPortTag {};
  struct SCISHARE DatatypePortTag {};
  
  template <size_t N>
  struct NumInputPorts
  {
    enum { NumIPorts = N };
  };
  
  template <size_t N>
  struct NumOutputPorts
  {
    enum { NumOPorts = N };
  };
  
  struct HasNoInputPorts : NumInputPorts<0> {};
  struct HasNoOutputPorts : NumOutputPorts<0> {};
  
  template <class PortTypeTag>
  class Has1InputPort : public NumInputPorts<1>
  {
  public:
    static std::vector<SCIRun::Dataflow::Networks::InputPortDescription> inputPortDescription(const std::string& port0Name);
  };

  template <class PortTypeTag0, class PortTypeTag1>
  class Has2InputPorts : public NumInputPorts<2>
  {
  public:
    static std::vector<SCIRun::Dataflow::Networks::InputPortDescription> inputPortDescription(const std::string& port0Name, const std::string& port1Name)
    {
      //TODO: use move semantics
      auto ports = Has1InputPort<PortTypeTag0>::inputPortDescription(port0Name);
      ports.push_back(Has1InputPort<PortTypeTag1>::inputPortDescription(port1Name)[0]);
      return ports;
    }
  };

  
  template <class PortTypeTag>
  class Has1OutputPort : public NumOutputPorts<1>
  {
  public:
    static std::vector<SCIRun::Dataflow::Networks::OutputPortDescription> outputPortDescription(const std::string& port0Name);
  };

  template <class PortTypeTag0, class PortTypeTag1>
  class Has2OutputPorts : public NumOutputPorts<2>
  {
  public:
    static std::vector<SCIRun::Dataflow::Networks::OutputPortDescription> outputPortDescription(const std::string& port0Name, const std::string& port1Name)
    {
      //TODO: use move semantics
      auto ports = Has1OutputPort<PortTypeTag0>::outputPortDescription(port0Name);
      ports.push_back(Has1OutputPort<PortTypeTag1>::outputPortDescription(port1Name)[0]);
      return ports;
    }
  };

#define PORT_SPEC(type)   template <>\
  class Has1InputPort<type ##PortTag> : public NumInputPorts<1>\
  {\
  public:\
    static std::vector<SCIRun::Dataflow::Networks::InputPortDescription> inputPortDescription(const std::string& port0Name)\
    {\
      std::vector<SCIRun::Dataflow::Networks::InputPortDescription> ports;\
      ports.push_back(SCIRun::Dataflow::Networks::PortDescription(port0Name, #type)); \
      return ports;\
    }\
  };\
  template <>\
  class Has1OutputPort<type ##PortTag> : public NumOutputPorts<1>\
  {\
  public:\
    static std::vector<SCIRun::Dataflow::Networks::OutputPortDescription> outputPortDescription(const std::string& port0Name)\
    {\
      std::vector<SCIRun::Dataflow::Networks::OutputPortDescription> ports;\
      ports.push_back(SCIRun::Dataflow::Networks::PortDescription(port0Name, #type)); \
      return ports;\
    }\
  }\

  PORT_SPEC(Matrix);
  PORT_SPEC(Scalar);
  PORT_SPEC(String);
  PORT_SPEC(Field);
  PORT_SPEC(Mesh);  //TODO temporary
  PORT_SPEC(Geometry);
  PORT_SPEC(Datatype);

#define ATTACH_NAMESPACE(type) Core::Datatypes::type
#define ATTACH_NAMESPACE2(type) SCIRun::Core::Datatypes::type

#define INPUT_PORT(index, name, type) static std::string inputPort ## index ## Name() { return #name; } \
  PortName< ATTACH_NAMESPACE(type), index > name;

#define OUTPUT_PORT(index, name, type) static std::string outputPort ## index ## Name() { return #name; } \
  PortName< ATTACH_NAMESPACE(type), index> name;

#define INITIALIZE_PORT(name) do{ name.name_ = #name;}while(0);

  //TODO: make metafunc for Input/Output

  template <size_t numPorts, class ModuleType>
  struct IPortDescriber
  {
    static std::vector<SCIRun::Dataflow::Networks::InputPortDescription> inputs()
    {
      return std::vector<SCIRun::Dataflow::Networks::InputPortDescription>();
    }
  };

  template <class ModuleType>
  struct IPortDescriber<1, ModuleType>
  {
    static std::vector<SCIRun::Dataflow::Networks::InputPortDescription> inputs()
    {
      return ModuleType::inputPortDescription(ModuleType::inputPort0Name());
    }
  };

  template <class ModuleType>
  struct IPortDescriber<2, ModuleType>
  {
    static std::vector<SCIRun::Dataflow::Networks::InputPortDescription> inputs()
    {
      return ModuleType::inputPortDescription(ModuleType::inputPort0Name(), ModuleType::inputPort1Name());
    }
  };

  template <size_t numPorts, class ModuleType>
  struct OPortDescriber
  {
    static std::vector<SCIRun::Dataflow::Networks::OutputPortDescription> outputs()
    {
      return std::vector<SCIRun::Dataflow::Networks::OutputPortDescription>();
    }
  };

  template <class ModuleType>
  struct OPortDescriber<1, ModuleType>
  {
    static std::vector<SCIRun::Dataflow::Networks::OutputPortDescription> outputs()
    {
      return ModuleType::outputPortDescription(ModuleType::outputPort0Name());
    }
  };

  template <class ModuleType>
  struct OPortDescriber<2, ModuleType>
  {
    static std::vector<SCIRun::Dataflow::Networks::OutputPortDescription> outputs()
    {
      return ModuleType::outputPortDescription(ModuleType::outputPort0Name(), ModuleType::outputPort1Name());
    }
  };

}
}

#endif
