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

#ifndef INTERFACE_APPLICATION_NOTESEDITOR_H
#define INTERFACE_APPLICATION_NOTESEDITOR_H

#include "ui_NoteEditor.h"

#include <Interface/Application/Note.h>
#include <Dataflow/Network/NetworkFwd.h>

namespace SCIRun {
namespace Gui {

class NoteEditor : public QDialog, public Ui::NoteEditor
{
	Q_OBJECT
	
public:
  explicit NoteEditor(const QString& moduleName, QWidget* parent = 0);
private Q_SLOTS:
  void changeTextColor();
  void changeFontSize(const QString& text);
  void changeTextAlignment(const QString& text);
  void resetText();
  void resetTextColor();
  void changeNotePosition(int index);
  void ok();
  void cancel();
  void updateNote();
Q_SIGNALS:
  void noteChanged(const Note& note);
protected:
  virtual void showEvent(QShowEvent* event);
private:
  QString moduleName_;
  Note currentNote_;
  QString noteHtmlBackup_;
  int fontSizeBackup_, positionBackup_;
  QColor previousColor_;
  NotePosition position_;
};

}
}

#endif
