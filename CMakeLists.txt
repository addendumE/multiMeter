cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)


if (NOT DEFINED PROJECT_NAME)
	project(multiMeter)
endif (NOT DEFINED PROJECT_NAME)

get_property(target_names DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY BUILDSYSTEM_TARGETS)
message(target_names="${target_names}")

add_custom_command(
  TARGET multiMeter.elf  PRE_BUILD
  COMMAND ../makefsdata
  VERBATIM)
