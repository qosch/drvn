cmake_minimum_required(VERSION 3.28)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "In-source builds are not allowed. Use a separate build directory, e.g. cmake --preset devboard-debug | devboard-release | host-tests")
endif()

project(drvn CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(DRVN_HOST "Build host artifacts (tests) instead of firmware" OFF)

add_compile_options(-Wall -Wextra -Wconversion -Wdouble-promotion -ffunction-sections -fdata-sections)

if(NOT DRVN_HOST)
  if(NOT CMAKE_SYSTEM_NAME STREQUAL "Generic")
    message(FATAL_ERROR "Firmware builds need the ARM toolchain file; use the devboard-debug or devboard-release preset")
  endif()

  if(NOT DEFINED BOARD OR BOARD STREQUAL "")
    set(BOARD devboard)
  endif()

  add_compile_options(-fno-exceptions -fno-rtti)

  # Order matters:
  # boards/${BOARD} defines the `boards` target and sets DRVN_CHIP_DEFINE and
  # DRVN_LINKER_SCRIPT (PARENT_SCOPE); mcu/ and app/ read those variables.
  # `boards` is linked against `mcu` below, where both targets already exist.
  add_subdirectory(drivers)
  add_subdirectory(boards/${BOARD})
  add_subdirectory(mcu)
  add_subdirectory(app)

  target_link_libraries(boards PUBLIC mcu)
endif()
