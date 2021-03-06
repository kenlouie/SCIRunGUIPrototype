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

SET(Engine_Network_SRCS
  NetworkEditorController.cc
  NetworkCommands.cc
  ProvenanceItem.cc
  ProvenanceItemFactory.cc
  ProvenanceItemImpl.cc
  ProvenanceManager.cc
  PythonImpl.cc
)

SET(Engine_Network_HEADERS
  ControllerInterfaces.h
  NetworkEditorController.h
  NetworkCommands.h
  ProvenanceItem.h
  ProvenanceItemFactory.h
  ProvenanceItemImpl.h
  ProvenanceManager.h
  PythonImpl.h
  share.h
)

ADD_LIBRARY(Engine_Network 
  ${Engine_Network_HEADERS}
  ${Engine_Network_SRCS}
)

SET(Engine_Network_NonPythonDependentLibs
  Dataflow_Network
  Core_Serialization_Network
  Core_Command
  Engine_Scheduler
  ${SCI_LOG4CPP_LIBRARY}
)

IF(BUILD_WITH_PYTHON)

  TARGET_LINK_LIBRARIES(Engine_Network
    ${Engine_Network_NonPythonDependentLibs}
    SCIRunPythonAPI
  )

  INCLUDE_DIRECTORIES(
    ${PYTHON_INCLUDE_DIR}
  )

  ADD_DEPENDENCIES(Engine_Network Python_external)

ELSE()

  TARGET_LINK_LIBRARIES(Engine_Network
    ${Engine_Network_NonPythonDependentLibs}
  )

ENDIF()

IF(BUILD_SHARED_LIBS)
  ADD_DEFINITIONS(-DBUILD_Engine_Network)
ENDIF(BUILD_SHARED_LIBS)

ADD_DEPENDENCIES(Engine_Network Boost_external)

ADD_SUBDIRECTORY(Tests)

