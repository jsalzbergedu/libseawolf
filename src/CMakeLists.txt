cmake_minimum_required(VERSION 3.1.3)
set(libseawolf_VERSION_STRING "${libseawolf_VERSION_MAJOR}.${libseawolf_VERSION_MINOR}")
add_library(seawolf SHARED ardcomm.c logging.c main.c notify.c pid.c var.c config.c serial.c stack.c
  synch.c task.c timer.c util.c dictionary.c list.c queue.c comm.c mem_pool.c)
set(LIBSEAWOLF_VERSION "0.1")
set(LIBSEAWOLF_SOVERSION 0)
set_target_properties(seawolf PROPERTIES VERSION ${LIBSEAWOLF_VERSION})
set_target_properties(seawolf PROPERTIES SOVERSION ${LIBSEAWOLF_SOVERSION})

# From https://stackoverflow.com/questions/18706942/create-a-custom-symbolic-link-to-a-library-at-install-time-with-cmake -- generate a libseawolf.so.0
SET(legacy_link   ${CMAKE_INSTALL_PREFIX}/libseawolf.so)
SET(legacy_target ${CMAKE_INSTALL_PREFIX}/libseawolf.so.0)
ADD_CUSTOM_COMMAND( OUTPUT ${legacy_link}
  COMMAND ln -s ${legacy_target} ${legacy_link}
  DEPENDS install ${legacy_link} 
  COMMENT "Generating legacy symbolic link")
ADD_CUSTOM_TARGET(install_legacy DEPENDS ${legacy_link} )



add_library(Seawolf::Lib ALIAS seawolf)
target_link_libraries(seawolf "${DEFAULT_LIBRARIES}")
install(TARGETS seawolf DESTINATION "lib")
install(DIRECTORY ${LIBSEAWOLF_TOPLEVEL}/include DESTINATION include/seawolf)
add_subdirectory(hub)
add_subdirectory(python)
