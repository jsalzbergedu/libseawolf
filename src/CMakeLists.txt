cmake_minimum_required(VERSION 3.1.3)
add_library(seawolf SHARED ardcomm.c logging.c main.c notify.c pid.c var.c config.c serial.c stack.c
  synch.c task.c timer.c util.c dictionary.c list.c queue.c comm.c mem_pool.c)
set(LIBSEAWOLF_VERSION "0.1")
set(LIBSEAWOLF_SOVERSION 0)
set_target_properties(seawolf PROPERTIES VERSION ${LIBSEAWOLF_VERSION})
set_target_properties(seawolf PROPERTIES SOVERSION ${LIBSEAWOLF_SOVERSION})

add_library(Seawolf::Lib ALIAS seawolf)
target_link_libraries(seawolf "${DEFAULT_LIBRARIES}")

install(TARGETS seawolf DESTINATION "lib")

add_subdirectory(hub)

set(LIBSEAWOLF_SO_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
add_subdirectory(python)
