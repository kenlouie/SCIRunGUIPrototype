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

#ifndef INTERFACE_APPLICATION_NETWORKEDITORCONTROLLER_H
#define INTERFACE_APPLICATION_NETWORKEDITORCONTROLLER_H

#include <QObject>
#include <Core/Dataflow/Network/NetworkFwd.h>

namespace SCIRun {
namespace Gui {
  
  //TODO: rework with boost::signal/slots
  class NetworkEditorController : public QObject
  {
    Q_OBJECT
  public:
    NetworkEditorController();
  public slots:
    void addModule(const QString& moduleName);
    void removeModule(const std::string& id);
    void addConnection(const std::string& id1, size_t port1, const std::string& id2, size_t port2);
    void removeConnection(const std::string& id);
  signals:
    void moduleAdded(const QString& name, const SCIRun::Domain::Networks::ModuleInfoProvider& portInfoProvider);
    void moduleRemoved(const std::string& id);
  private:
    void printNetwork() const;
    SCIRun::Domain::Networks::NetworkHandle theNetwork_;
  };

}
}

#endif