add_library(mcu STATIC startup.cpp)
target_include_directories(mcu PUBLIC ${PROJECT_SOURCE_DIR})
target_compile_definitions(mcu PRIVATE ${DRVN_CHIP_DEFINE})

# Only the LL drivers are ever used from the HAL driver repo (Inc/ holds both)
target_include_directories(mcu SYSTEM PUBLIC
  ${PROJECT_SOURCE_DIR}/external/cmsis_core/Include
  ${PROJECT_SOURCE_DIR}/external/cmsis_device_g4/Include
  ${PROJECT_SOURCE_DIR}/external/stm32g4xx_hal_driver/Inc
)
