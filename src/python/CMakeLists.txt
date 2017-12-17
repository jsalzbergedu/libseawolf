cmake_minimum_required(VERSION 3.1.3)
add_custom_target(python ALL
  COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/buildscript.sh ${CMAKE_CURRENT_SOURCE_DIR} ${PYTHON} build_ext build)
install(CODE "execute_process( COMMAND bash \"${CMAKE_CURRENT_SOURCE_DIR}\"/buildscript.sh \"${CMAKE_CURRENT_SOURCE_DIR}\" ${PYTHON} install )" )
