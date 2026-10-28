add_executable(app main.cpp)
target_include_directories(app PRIVATE ${PROJECT_SOURCE_DIR})
target_link_libraries(app PRIVATE boards)

set_target_properties(app PROPERTIES SUFFIX ".elf")
target_link_options(app PRIVATE
  -nostartfiles
  -T${DRVN_LINKER_SCRIPT}
  -Wl,--gc-sections
  -Wl,-Map=${CMAKE_BINARY_DIR}/app.map
)

add_custom_command(TARGET app POST_BUILD
  COMMAND arm-none-eabi-size $<TARGET_FILE:app>
  COMMENT "app size"
  VERBATIM
)
