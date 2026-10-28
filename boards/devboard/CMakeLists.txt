# This board's chip define and linker script; read by mcu/ and app/.
set(DRVN_CHIP_DEFINE "STM32G474xx" PARENT_SCOPE)
set(DRVN_LINKER_SCRIPT "${PROJECT_SOURCE_DIR}/mcu/linker/stm32g474ve.ld" PARENT_SCOPE)

add_library(boards STATIC board.cpp)
target_include_directories(boards PUBLIC ${PROJECT_SOURCE_DIR})
target_compile_definitions(boards PRIVATE ${DRVN_CHIP_DEFINE})
target_link_libraries(boards PUBLIC drivers)
