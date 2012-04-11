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

#ifndef PORTWIDGET_H
#define PORTWIDGET_H

#include <boost/shared_ptr.hpp>
#include <QGraphicsWidget>
#include <QWidget>
#include <QColor>
#include <set>
#include <Interface/Application/PositionProvider.h>

class QGraphicsScene;

namespace SCIRun {
namespace Gui {

class Connection;
class ConnectionInProgress;
class PositionProvider;

class Port : public QWidget, public NeedsScenePositionProvider
{
  Q_OBJECT
public:
  Port(const QString& name, const QColor& color, bool isInput, QWidget* parent = 0);

  QString name() const { return name_; }
  QColor color() const { return color_; }
  bool isInput() const { return isInput_; }
  bool isConnected() const { return isConnected_; }
  void setConnected(bool connected);

  void toggleLight();
  void turn_on_light();
  void turn_off_light();
  bool isLightOn() const { return lightOn_; }

  QSize sizeHint() const;

  void addConnection(Connection* c);
  void removeConnection(Connection* c);

  void trackConnections();
  void deleteConnections();

  QPointF position() const;

  //TODO: yuck
  static QGraphicsScene* TheScene;
  void doMousePress(Qt::MouseButton button, const QPointF& pos);
  void doMouseMove(Qt::MouseButtons buttons, const QPointF& pos);
  void doMouseRelease(Qt::MouseButton button, const QPointF& pos);
protected:
  void mousePressEvent(QMouseEvent* event);
  void mouseReleaseEvent(QMouseEvent* event);
  void mouseMoveEvent(QMouseEvent* event);
  void paintEvent(QPaintEvent* event);
private:
  void performDrag(const QPointF& endPos);
  bool canBeConnected(Port* other) const;

  const QString name_;
  const QColor color_;
  const bool isInput_;
  bool isConnected_;
  bool lightOn_;
  QPointF startPos_;
  QWidget* moduleParent_;
  ConnectionInProgress* currentConnection_;
  std::set<Connection*> connections_;
};

class InputPort : public Port 
{
public:
  InputPort(const QString& name, const QColor& color, QWidget* parent = 0);
};

class OutputPort : public Port 
{
public:
  OutputPort(const QString& name, const QColor& color, QWidget* parent = 0);
};

}
}

#endif