cmake_minimum_required(VERSION 3.1.3)
add_custom_target(pylib
  COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/buildscript.sh ${CMAKE_CURRENT_SOURCE_DIR} ${PYTHON} build_ext build)
install(CODE "execute_process( COMMAND bash \"${CMAKE_CURRENT_SOURCE_DIR}\"/buildscript.sh \"${CMAKE_CURRENT_SOURCE_DIR}\" ${PYTHON} install )" )
add_custom_target(pylib-install
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  COMMAND "${CMAKE_COMMAND}" ${CMAKE_CURRENT_SOURCE_DIR}/pyinstall.cmake . --target install)
