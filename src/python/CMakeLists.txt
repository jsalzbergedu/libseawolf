cmake_minimum_required(VERSION 3.1.3)
add_custom_target(pylib_copy_so
  COMMAND cp ${CMAKE_BINARY_DIR}/src/libseawolf.so ${CMAKE_CURRENT_SOURCE_DIR}/
  DEPENDS seawolf)
add_custom_target(pylib_setup
  COMMAND ln -sf ${CMAKE_CURRENT_SOURCE_DIR}/libseawolf.so ${CMAKE_CURRENT_SOURCE_DIR}/libseawolf.so.${LIBSEAWOLF_SOVERSION}
  DEPENDS pylib_copy_so)
add_custom_target(pylib
  COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/buildscript.sh ${CMAKE_CURRENT_SOURCE_DIR} ${PYTHON_EXECUTABLE} build_ext build
  DEPENDS pylib_setup)
install(CODE "execute_process( COMMAND bash \"${CMAKE_CURRENT_SOURCE_DIR}\"/buildscript.sh \"${CMAKE_CURRENT_SOURCE_DIR}\" ${PYTHON} install )" )
add_custom_target(pylib-install
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  COMMAND "${CMAKE_COMMAND}" ${CMAKE_CURRENT_SOURCE_DIR}/pyinstall.cmake . --target install)
