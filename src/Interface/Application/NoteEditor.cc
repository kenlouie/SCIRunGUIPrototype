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

#include <QtGui>
#include <iostream>
#include <Interface/Application/NoteEditor.h>

using namespace SCIRun::Gui;

namespace
{
  int moveIncrement = 30;
}

NoteEditor::NoteEditor(const QString& moduleName, QWidget* parent) : QDialog(parent), moduleName_(moduleName)
{
  setupUi(this);
  setModal(false);
  setWindowTitle("Note for " + moduleName);
  setVisible(false);
  moveIncrement += moveIncrement;
  move(moveIncrement, moveIncrement);

  connect(chooseColorButton_, SIGNAL(clicked()), this, SLOT(changeTextColor()));
  connect(resetColorButton_, SIGNAL(clicked()), this, SLOT(resetTextColor()));
  connect(positionComboBox_, SIGNAL(activated(int)), this, SLOT(changeNotePosition(int)));
  connect(fontSizeComboBox_, SIGNAL(activated(const QString&)), this, SLOT(changeFontSize(const QString&)));
  //TODO: sloppy.
  //connect(alignmentComboBox_, SIGNAL(activated(const QString&)), this, SLOT(changeTextAlignment(const QString&)));

  connect(textEdit_, SIGNAL(textChanged()), this, SLOT(updateNote()));

  connect(buttonBox_->button(QDialogButtonBox::Reset), SIGNAL(clicked()), this, SLOT(resetText()));
  connect(buttonBox_->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(ok()));
  connect(buttonBox_->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(cancel()));

  //TODO: settable notes
  previousColor_ = Qt::black;
  position_ = Default;
}

void NoteEditor::changeNotePosition(int index)
{
  position_ = (NotePosition)index; //TODO: unit test;
  updateNote();
}

void NoteEditor::changeFontSize(const QString& text)
{
  textEdit_->setFontPointSize(text.toDouble());
  textEdit_->setPlainText(textEdit_->toPlainText());
}

void NoteEditor::changeTextAlignment(const QString& text)
{
  //TODO: only changes one line at a time...may just chuck this option
  Qt::Alignment alignment;
  if (text == "Left")
    alignment = Qt::AlignLeft;
  else if (text == "Center")
    alignment = Qt::AlignHCenter;
  else if (text == "Right")
    alignment = Qt::AlignRight;
  else // text == "Justify")
    alignment = Qt::AlignJustify;
  textEdit_->setAlignment(alignment);
  textEdit_->setPlainText(textEdit_->toPlainText());
}

void NoteEditor::changeTextColor()
{
  auto newColor = QColorDialog::getColor(previousColor_, this, "Choose text color");
  if (newColor.isValid())
  {
    previousColor_ = textEdit_->textColor();
    textEdit_->setTextColor(newColor);
    textEdit_->setPlainText(textEdit_->toPlainText());
  }
}

void NoteEditor::resetText()
{
  //noteBackup_.html_ = textEdit_->toHtml();

  textEdit_->clear();
}

void NoteEditor::resetTextColor()
{
  auto oldColor = textEdit_->textColor();
  textEdit_->setTextColor(previousColor_);
  textEdit_->setPlainText(textEdit_->toPlainText());
  previousColor_ = oldColor;
}

void NoteEditor::ok()
{
  hide();
}

void NoteEditor::cancel()
{
  textEdit_->setHtml(noteHtmlBackup_);
  fontSizeComboBox_->setCurrentIndex(fontSizeBackup_);
  //positionComboBox_->setCurrentIndex(positionBackup_);
  //updateNote();
  hide();
}

void NoteEditor::updateNote()
{
  currentNote_.html_ = textEdit_->toHtml();
  currentNote_.position_ = position_;
  Q_EMIT noteChanged(currentNote_);
}

void NoteEditor::showEvent(QShowEvent* event)
{
  noteHtmlBackup_ = textEdit_->toHtml();
  fontSizeBackup_ = fontSizeComboBox_->currentIndex();
  //positionBackup_ = positionComboBox_->currentIndex();
  QDialog::showEvent(event);
}