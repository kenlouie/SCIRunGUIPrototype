/*
   For more information, please see: http://software.sci.utah.edu

   The MIT License

   Copyright (c) 2009 Scientific Computing and Imaging Institute,
   University of Utah.

   
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


// itkDatatype.h - Base Insight Datatype
//
//  Written by:
//   Darby J Brown
//   School of Computing
//   University of Utah
//   January 2003
//

#ifndef SCI_Insight_itkDatatype_h
#define SCI_Insight_itkDatatype_h

#include <Core/Datatypes/Datatype.h>
#include <Core/Containers/LockingHandle.h>
#include "itkObject.h"

// TEMP
#include "itkImage.h"
#include "itkRGBPixel.h"
#include "itkVector.h"

#include <Core/Datatypes/share.h>
namespace SCIRun {

class SCISHARE ITKDatatype : public Datatype {
public:  
  std::string fname;
  itk::Object::Pointer data_;

  ITKDatatype();
  ITKDatatype(const ITKDatatype&);
  ~ITKDatatype();

  virtual void io(Piostream&);
  static PersistentTypeID type_id;
  virtual std::string dynamic_type_name() const { return type_id.type; }
};

typedef LockingHandle<ITKDatatype> ITKDatatypeHandle;

} // end namespace SCIRun

#endif
