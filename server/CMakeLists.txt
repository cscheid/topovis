project(persistence)

list( APPEND CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")	# enables C++ 11 features
set(CMAKE_BUILD_TYPE Release)

include_directories(${persistence_SOURCE_DIR})

subdirs(geometry topology)

add_executable(toposerver toposerver.cc mongoose.c topocubes.cc)
target_link_libraries( toposerver topology geometry ANN )

add_executable(example_gen example_gen.cc)
target_link_libraries( example_gen topology geometry ANN )

add_executable(pset_to_rips pset_to_rips.cc)
target_link_libraries( pset_to_rips topology geometry ANN )
