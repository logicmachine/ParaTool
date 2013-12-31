cmake_minimum_required(VERSION 2.8)
project(paratool-test)

include_directories(../src)

include(ExternalProject)
set_directory_properties(
	PROPERTIES EP_PREFIX
	${CMAKE_BINARY_DIR}/ThirdParty)
ExternalProject_Add(
	googletest
	SOURCE_DIR ${CMAKE_SOURCE_DIR}/../gtest-1.7.0
	INSTALL_COMMAND "")
ExternalProject_Get_Property(googletest source_dir)
include_directories(${source_dir}/include)

set(
	CMAKE_CXX_FLAGS
	"${CMAKE_CXX_FLAGS} -fopenmp -g -O0 -lnuma -lpthread")

file(GLOB test_files "*.cpp")
add_executable(paratool_test ${test_files})
add_dependencies(paratool_test googletest)
ExternalProject_Get_Property(googletest binary_dir)
target_link_libraries(
	paratool_test
	debug ${binary_dir}/libgtest.a
	optimized ${binary_dir}/libgtest.a)

