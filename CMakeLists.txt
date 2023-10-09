cmake_minimum_required(VERSION 3.19)
project(bm_sim CXX)

find_package(spdlog REQUIRED)

# ==============================================
# Targets

add_executable( ${PROJECT_NAME}
  src/main.cpp
  src/node.cpp
  src/subnet.cpp
  src/neighbor.cpp
  src/resource.cpp
  src/message.cpp
  src/timer.cpp
)

target_link_libraries( ${PROJECT_NAME}
PRIVATE
  spdlog::spdlog
)

target_include_directories(${PROJECT_NAME}
PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:include>
)

target_compile_options(${PROJECT_NAME}
PUBLIC
  -Wall 
  -Wextra 
  -Wpedantic
)

target_compile_features(${PROJECT_NAME}
PUBLIC
  cxx_std_17
)

# ==============================================
# Install
install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin
)