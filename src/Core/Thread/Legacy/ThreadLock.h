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


 
/*
 *  ThreadLock.h:  
 *     Mutex that is lockable multiple times within the same thread
 *  Written by:
 *   McKay Davis / Steven G. Parker
 *   Department of Computer Science
 *   University of Utah
 *   August 1994
 *
 */

#ifndef SCIRun_Core_Thread_ThreadLock_h
#define SCIRun_Core_Thread_ThreadLock_h

#include <Core/Thread/Legacy/Mutex.h>
#include <Core/Thread/Legacy/Thread.h>
#include <Core/Thread/Legacy/share.h>

namespace SCIRun {

class SCISHARE ThreadLock {
public:
  ThreadLock(const char *);
  void          lock();
  int           try_lock();
  void          unlock();
private:
  Mutex         mutex_;
  Thread *      owner_;
  int           count_;
};

} // End namespace SCIRun


#endif
