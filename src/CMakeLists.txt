cmake_minimum_required(VERSION 3.1.3)
add_library(seawolf SHARED ardcomm.c logging.c main.c notify.c pid.c var.c config.c serial.c stack.c
  synch.c task.c timer.c util.c dictionary.c list.c queue.c comm.c mem_pool.c)
add_library(Seawolf::Lib ALIAS seawolf)
target_link_libraries(seawolf "${DEFAULT_LIBRARIES}")
install(TARGETS seawolf DESTINATION "lib")
install(DIRECTORY ${LIBSEAWOLF_TOPLEVEL}/include DESTINATION include/seawolf)
add_subdirectory(hub)
add_subdirectory(python)
