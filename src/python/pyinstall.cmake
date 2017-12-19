cmake_minimum_required(VERSION 3.1.3)
set(Python_ADDITIONAL_VERSIONS 2.7 2.6 2.5)
find_package(PythonInterp REQUIRED)
install(CODE "execute_process( COMMAND bash \"${CMAKE_CURRENT_SOURCE_DIR}\"/buildscript.sh \"${CMAKE_CURRENT_SOURCE_DIR}\" ${PYTHON_EXECUTABLE} install )" )
