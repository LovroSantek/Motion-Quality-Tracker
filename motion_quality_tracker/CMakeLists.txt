cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(motion_quality_tracker)

idf_build_set_property(COMPILE_OPTIONS "-fdiagnostics-color=always" APPEND)
idf_build_set_property(COMPILE_OPTIONS "-Wno-unused-variable" APPEND)
idf_build_set_property(COMPILE_OPTIONS "-Wno-maybe-uninitialized" APPEND)
idf_build_set_property(COMPILE_OPTIONS "-Wno-error=format=" APPEND)
idf_build_set_property(COMPILE_OPTIONS "-Wno-unused-but-set-parameter" APPEND)
idf_build_set_property(COMPILE_OPTIONS "-Wno-error=nonnull-compare" APPEND)
idf_build_set_property(COMPILE_OPTIONS "-Wno-error=stringop-truncation" APPEND)
idf_build_set_property(COMPILE_OPTIONS "-Wno-error=uninitialized" APPEND)
idf_build_set_property(COMPILE_OPTIONS "-Wno-stringop-truncation" APPEND)
idf_build_set_property(COMPILE_OPTIONS "-Wno-deprecated-declarations" APPEND)
idf_build_set_property(COMPILE_OPTIONS "-fno-if-conversion" APPEND)
