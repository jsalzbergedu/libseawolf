cmake_minimum_required(VERSION 3.1.3)
add_custom_target(pylib_setup
  COMMAND ln -s ${LIBSEAWOLF_SO_DIR}/libseawolf.so ${LIBSEAWOLF_SO_DIR}/libseawolf.so.${LIBSEAWOLF_SOVERSION})
add_custom_target(pylib
  COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/buildscript.sh ${CMAKE_CURRENT_SOURCE_DIR} ${PYTHON} build_ext build
  DEPENDS pylib_setup)
install(CODE "execute_process( COMMAND bash \"${CMAKE_CURRENT_SOURCE_DIR}\"/buildscript.sh \"${CMAKE_CURRENT_SOURCE_DIR}\" ${PYTHON} install )" )
add_custom_target(pylib-install
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  COMMAND "${CMAKE_COMMAND}" ${CMAKE_CURRENT_SOURCE_DIR}/pyinstall.cmake . --target install)
