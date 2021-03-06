#
#  For more information, please see: http://software.sci.utah.edu
# 
#  The MIT License
# 
#  Copyright (c) 2012 Scientific Computing and Imaging Institute,
#  University of Utah.
# 
#  
#  Permission is hereby granted, free of charge, to any person obtaining a
#  copy of this software and associated documentation files (the "Software"),
#  to deal in the Software without restriction, including without limitation
#  the rights to use, copy, modify, merge, publish, distribute, sublicense,
#  and/or sell copies of the Software, and to permit persons to whom the
#  Software is furnished to do so, subject to the following conditions:
# 
#  The above copyright notice and this permission notice shall be included
#  in all copies or substantial portions of the Software. 
# 
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
#  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#  DEALINGS IN THE SOFTWARE.
#

SET(Modules_Math_SRCS
  AppendMatrix.cc
  CreateMatrix.cc
  EvaluateLinearAlgebraUnary.cc
  EvaluateLinearAlgebraUnaryState.cc
  EvaluateLinearAlgebraBinary.cc
  EvaluateLinearAlgebraBinaryState.cc
  ReportMatrixInfo.cc
  ReportMatrixInfoState.cc
  SolveLinearSystem.cc
)

SET(Modules_Math_HEADERS
  AppendMatrix.h
  CreateMatrix.h
  EvaluateLinearAlgebraUnary.h
  EvaluateLinearAlgebraUnaryState.h
  EvaluateLinearAlgebraBinary.h
  EvaluateLinearAlgebraBinaryState.h
  ReportMatrixInfo.h
  ReportMatrixInfoState.h
  share.h
  SolveLinearSystem.h
)

ADD_LIBRARY(Modules_Math 
  ${Modules_Math_HEADERS}
  ${Modules_Math_SRCS}
)

TARGET_LINK_LIBRARIES(Modules_Math
  Dataflow_Network
  Core_Datatypes
  Algorithms_Math
)

IF(BUILD_SHARED_LIBS)
  ADD_DEFINITIONS(-DBUILD_Modules_Math)
ENDIF(BUILD_SHARED_LIBS)

ADD_SUBDIRECTORY(Tests)

