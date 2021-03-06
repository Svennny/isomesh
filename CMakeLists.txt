cmake_minimum_required (VERSION 3.10 FATAL_ERROR)

project (
	isomesh
	VERSION 0.1.0
	DESCRIPTION "Library of isosurface extraction algorithms"
	LANGUAGES CXX
)

list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include (CMakePackageConfigHelpers)
include (GNUInstallDirs)
include (cotire)

set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_STANDARD 17)

find_package (glm CONFIG REQUIRED)

set (ISOMESH_HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/isomesh)
set (ISOMESH_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set (ISOMESH_HEADERS
	include/isomesh/common.hpp
	include/isomesh/isomesh.hpp
	include/isomesh/algo/marching_cubes.hpp
	include/isomesh/algo/uniform_dual_contouring.hpp
	include/isomesh/data/dc_octree.hpp
	include/isomesh/data/dc_octree_node.hpp
	include/isomesh/data/dmc_octree.hpp
	include/isomesh/data/dmc_octree_node.hpp
	include/isomesh/data/grid.hpp
	include/isomesh/data/grid_edge_storage.hpp
	include/isomesh/data/mdc_octree.hpp
	include/isomesh/data/mdc_octree_node.hpp
	include/isomesh/data/mesh.hpp
	include/isomesh/export/mesh2ply.hpp
	include/isomesh/field/heightmap.hpp
	include/isomesh/field/mesh_field.hpp
	include/isomesh/field/scalar_field.hpp
	include/isomesh/qef/qef_solver_3d.hpp
	include/isomesh/qef/qef_solver_4d.hpp
	include/isomesh/util/material_filter.hpp
	include/isomesh/util/ply_mesh.hpp
	include/isomesh/util/tables.hpp
	include/isomesh/util/zero_finder.hpp
)
source_group (TREE ${ISOMESH_HEADER_DIR} PREFIX Headers FILES ${ISOMESH_HEADERS})

set (ISOMESH_SOURCES
	src/3dparty/stb_image.h
	src/3dparty/tinyply.h
	src/3dparty/tinyply_impl.cpp
	src/algo/marching_cubes.cpp
	src/algo/uniform_dual_contouring.cpp
	src/data/dc_octree.cpp
	src/data/dc_octree_node.cpp
	src/data/dmc_octree.cpp
	src/data/dmc_octree_node.cpp
	src/data/grid.cpp
	src/data/grid_edge_storage.cpp
	src/data/mdc_octree.cpp
	src/data/mdc_octree_node.cpp
	src/data/mesh.cpp
	src/export/mesh2ply.cpp
	src/field/heightmap.cpp
	src/field/mesh_field.cpp
	src/private/disjoint_set_union.hpp
	src/private/octree.cpp
	src/private/octree.hpp
	src/private/ply_data.cpp
	src/private/ply_data.hpp
	src/private/stbi_data.cpp
	src/private/stbi_data.hpp
	src/private/triangle.hpp
	src/qef/householder.hpp
	src/qef/jacobi.hpp
	src/qef/qef_solver_3d.cpp
	src/qef/qef_solver_4d.cpp
	src/util/bisection_zero_finder.cpp
	src/util/material_filter.cpp
	src/util/ply_mesh.cpp
	src/util/tables.cpp
	src/util/regula_falsi_zero_finder.cpp
)
source_group (TREE ${ISOMESH_SOURCE_DIR} PREFIX Sources FILES ${ISOMESH_SOURCES})

add_library (isomesh STATIC ${ISOMESH_HEADERS} ${ISOMESH_SOURCES})
target_link_libraries (isomesh PUBLIC glm)
target_link_libraries (isomesh PRIVATE $<$<CXX_COMPILER_ID:GNU>:stdc++fs>)
target_include_directories (isomesh PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

# For more strict warnings
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  target_compile_options(isomesh PRIVATE "-Wall")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  target_compile_options(isomesh PRIVATE "-Wall")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  target_compile_options(isomesh PRIVATE "/W4")
endif()

option(BUILD_DOC "Build developer documentation" ON)
if (BUILD_DOC)
	find_package(Doxygen)
	if (DOXYGEN_FOUND)
		include(configureDoxygenFile)

		set(DOXY_INPUT ${ISOMESH_HEADER_DIR})
		configure_doxygen_file(Doxyfile)

		add_custom_command(
			OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/doxygen.stamp
			DEPENDS ${ISOMESH_HEADERS}
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
			COMMAND cmake -E touch ${CMAKE_CURRENT_BINARY_DIR}/doxygen.stamp
			COMMENT "Generating API documentation with Doxygen"
			VERBATIM
		)
		add_custom_target(docs ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/doxygen.stamp)

		install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION ${CMAKE_INSTALL_DOCDIR})
	else (DOXYGEN_FOUND)
		message("Doxygen need to be installed to generate the doxygen documentation")
	endif (DOXYGEN_FOUND)
endif(BUILD_DOC)

set_target_properties (isomesh PROPERTIES
	DEBUG_POSTFIX _d
	COTIRE_ADD_UNITY_BUILD FALSE
	COTIRE_PREFIX_HEADER_INCLUDE_PATH ${GLM_INCLUDE_DIRS}
)
cotire (isomesh)

option (ISOMESH_TEST "Enable testing" OFF)
if (ISOMESH_TEST)
	enable_testing ()
	add_subdirectory (test)
endif ()

set (ISOMESH_INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/isomesh)

install (TARGETS isomesh EXPORT isomesh_targets
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# build tree package
write_basic_package_version_file (${CMAKE_CURRENT_BINARY_DIR}/isomesh-config-version.cmake
	VERSION ${isomesh_VERSION} COMPATIBILITY SameMajorVersion)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/cmake/isomesh-build-config.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/isomesh-config.cmake @ONLY)
export (EXPORT isomesh_targets FILE ${CMAKE_CURRENT_BINARY_DIR}/isomesh-targets.cmake)
export (PACKAGE isomesh)

# install tree package
configure_package_config_file (${CMAKE_CURRENT_SOURCE_DIR}/cmake/isomesh-config.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/${ISOMESH_INSTALL_CONFIGDIR}/isomesh-config.cmake
	INSTALL_DESTINATION ${ISOMESH_INSTALL_CONFIGDIR}
	PATH_VARS CMAKE_INSTALL_LIBDIR CMAKE_INSTALL_INCLUDEDIR
	NO_CHECK_REQUIRED_COMPONENTS_MACRO)
install (FILES
	${CMAKE_CURRENT_BINARY_DIR}/${ISOMESH_INSTALL_CONFIGDIR}/isomesh-config.cmake
	${CMAKE_CURRENT_BINARY_DIR}/isomesh-config-version.cmake
	DESTINATION ${ISOMESH_INSTALL_CONFIGDIR})
install (DIRECTORY ${ISOMESH_HEADER_DIR} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install (EXPORT isomesh_targets FILE isomesh-targets.cmake DESTINATION ${ISOMESH_INSTALL_CONFIGDIR})
