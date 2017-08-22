cmake_minimum_required(VERSION 2.8)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

set(SHORT_NAME vitabrot)
project(${SHORT_NAME})
include("${VITASDK}/share/vita.cmake" REQUIRED)

set(VITA_APP_NAME "VitaBrot")
set(VITA_TITLEID  "VITABROT1")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -O4 -mtune=cortex-a9 -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O4 -mtune=cortex-a9 -Wall")

include_directories(
  include
)

add_executable(${SHORT_NAME}
  src/main.cc
  lib/display.cc
  lib/mandelbrot.cc
  lib/debuglog.c
)

target_link_libraries(${SHORT_NAME}
  SDL2
  vita2d
  SceDisplay_stub
  SceCtrl_stub
  SceAudio_stub
  SceSysmodule_stub
  SceGxm_stub
  SceCommonDialog_stub
  ScePower_stub
  gmp
  gmpxx
  m
)

vita_create_self(${SHORT_NAME}.self ${SHORT_NAME})
vita_create_vpk(${SHORT_NAME}.vpk ${VITA_TITLEID} ${SHORT_NAME}.self
  VERSION ${VITA_VERSION}
  NAME ${VITA_APP_NAME}
  FILE sce_sys/icon0.png sce_sys/icon0.png
  FILE sce_sys/livearea/contents/bg.png sce_sys/livearea/contents/bg.png
  FILE sce_sys/livearea/contents/startup.png sce_sys/livearea/contents/startup.png
  FILE sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
)
