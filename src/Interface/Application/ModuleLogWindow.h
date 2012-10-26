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

#ifndef INTERFACE_APPLICATION_MODULELOGWINDOW_H
#define INTERFACE_APPLICATION_MODULELOGWINDOW_H

#include "ui_ModuleLogWindow.h"

#include <Core/Logging/Logger.h>
#include <Dataflow/Network/NetworkFwd.h>

namespace SCIRun {
namespace Gui {

class ModuleLogWindow : public QDialog, public Ui::ModuleLogWindow
{
	Q_OBJECT
	
public:
  explicit ModuleLogWindow(const QString& moduleName, QWidget* parent = 0);
public Q_SLOTS:
  void appendMessage(const QString& message, const QColor& color = Qt::black);
Q_SIGNALS:
  void messageReceived(const QColor& color);
};

class ModuleLogger : public QObject, public Core::Logging::LoggerInterface
{
  Q_OBJECT
public:
  explicit ModuleLogger(ModuleLogWindow* window);
  virtual void error(const std::string& msg);
  virtual void warning(const std::string& msg);
  virtual void remark(const std::string& msg);
  virtual void status(const std::string& msg);
Q_SIGNALS:
  void logSignal(const QString& message, const QColor& color);
  void alert(const QColor& color);
};

}
}

#endif