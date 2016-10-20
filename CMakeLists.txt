cmake_minimum_required(VERSION 3.2)

project(Moneyphant CXX) # Project name and programming languages (CXX = C++).

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/Modules)

# We need the path to the directory with the CMakeLists.txt of libmr.
# You don't need to build libmr yourself!
set(LIBMR_DIR ${CMAKE_SOURCE_DIR}/../libmr)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${LIBMR_DIR}/cmake)


# If you want to build libmr including classes that use Qt classes you have to make sure cmake can find a recent Qt version.
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} /usr/local/Cellar/qt5/5.7.0)

# Throw error if we are not building out of source.
include(RequireOutOfSourceBuild)

###
### Build
###
if (NOT CMAKE_BUILD_TYPE)
	# Default to Release.
	set(CMAKE_BUILD_TYPE "Release")
	message(STATUS "No build type selected, default to Release. You can choose debug build by appending -DCMAKE_BUILD_TYPE=Debug to the cmake command.")
endif()

# Put binaries in bin/ and lib/ subfolders instead of CMAKE_BINARY_DIR root.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

###
### Sources
###

###
### Targets
###
# Build libmr instead of depending on a existing build
add_subdirectory(${LIBMR_DIR}/ ${CMAKE_BINARY_DIR}/lib)

# Moneyphant GUI application
file(GLOB src_model src/model/*.cpp)
set(src_all ${src_model} src/main.cpp)

add_executable(moneyphant WIN32 MACOSX_BUNDLE ${src_all})
set_target_properties(moneyphant PROPERTIES CXX_STANDARD 14)

###
### Dependencies
###
# Link libmr.
include_directories(${LIBMR_DIR}/include)
target_link_libraries(moneyphant libmr)

# Configure version.cpp to contain version information based on the repository history
add_custom_target(version_cpp ALL COMMAND ${CMAKE_COMMAND} -DSOURCE_DIR=${PROJECT_SOURCE_DIR} -P ${LIBMR_DIR}/cmake/ConfigureVersionCppFromGit.cmake)
add_dependencies(moneyphant version_cpp)

# sqlpp11
find_package(Sqlpp11 REQUIRED)
message(STATUS "SQLPP11_INCLUDE_DIRS=${SQLPP11_INCLUDE_DIRS}")
message(STATUS "SQLPP11_INCLUDE_DIRS=${SQLPP11_LIBRARIES}")
target_include_directories(moneyphant PUBLIC ${SQLPP11_INCLUDE_DIRS})
target_link_libraries(moneyphant ${SQLPP11_LIBRARIES})

# Qt
# Qt5 documentation on using CMake: http://doc.qt.io/qt-5/cmake-manual.html

# Add Homebrew default install directory of qt5 formula.
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} /usr/local/Cellar/qt5/5.7.0)
# Find includes in corresponding build directories.
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets REQUIRED) # depends on Qt5Core and Qt5Gui
target_link_libraries(moneyphant Qt5::Widgets)
