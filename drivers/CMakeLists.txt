# No drivers yet; interface target for the include convention.
# Becomes a static library with the first driver.
add_library(drivers INTERFACE)
target_include_directories(drivers INTERFACE ${PROJECT_SOURCE_DIR})
