install(CODE "execute_process( COMMAND bash \"${CMAKE_CURRENT_SOURCE_DIR}\"/buildscript.sh \"${CMAKE_CURRENT_SOURCE_DIR}\" ${PYTHON} install )" )
