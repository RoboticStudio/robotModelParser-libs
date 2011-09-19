cmake_minimum_required(VERSION 2.6)

# Configure the project
project(test)
set(EXECUTABLE_OUTPUT_PATH bin)

# Add path to find a library to include
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules")

# Include RobotModelParser
FIND_PACKAGE(robotModelParser REQUIRED)
INCLUDE_directories(${robotModelParser_INCLUDE_DIR})
SET(INCLUDE_DIRS ${INCLUDE_DIRS} ${robotModelParser_INCLUDE_DIR})
SET(LIBS ${LIBS} ${robotModelParser_LIBRARIES})

# Create executable
add_executable(
    test
    test.cpp
)

# Link with librairies
target_link_libraries(
    test
    ${LIBS}
)
