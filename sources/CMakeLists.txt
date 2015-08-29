# Directory setting
if (${ONSITE_BUILD} STREQUAL "ON")
  set(ASSET_DIRECTORY "assets/")
  set(RESULT_DIRECTORY "./")
else()
  set(ASSET_DIRECTORY "${CMAKE_SOURCE_DIR}/assets/")
  set(RESULT_DIRECTORY "${CMAKE_SOURCE_DIR}/results/")
  file(MAKE_DIRECTORY ${RESULT_DIRECTORY})
endif()

configure_file(directories.h.in ${CMAKE_CURRENT_LIST_DIR}/directories.h @ONLY)

set(SOURCE_CORE ${SOURCE_CORE}
    vector3d.cc
    vector3d_avx.cc
    random.cc
    halton.cc
    random_sampler.cc
    ray.cc
    plane.cc
    bbox.cc
    triangle.cc
    trimesh.cc
    orthogonal_camera.cc
    perspective_camera.cc
    qbvh_accel.cc
    hash_grid.cc
    bsdf.cc
    brdf.cc
    bssrdf.cc
    image.cc
    envmap.cc
    scene.cc
    sampler.cc
    photon_map.cc)

set(SOURCE_RENDER ${SOURCE_RENDER}
    path_tracing.cc
    progressive_photon_mapping.cc
    ppm_probabilistic.cc
    subsurface_integrator.cc)

set(HEADER_CORE ${HEADER_CORE}
    tatsy_pppm.h
    common.h
    path.h
    renderer.h
    timer.h
    size.h
    readonly_interface.h
    kdtree.h
    kdtree_detail.h
    random_interface.h
    random.h
    halton.h
    random_sampler.h
    random_sequence.h
    vector3d.h
    photon.h
    ray.h
    geometry_interface.h
    plane.h
    bbox.h
    triangle.h
    trimesh.h
    camera_interface.h
    orthogonal_camera.h
    perspective_camera.h
    qbvh_accel.h
    hash_grid.h
    random_queue.h
    bsdf.h
    brdf.h
    bssrdf.h
    image.h
    envmap.h
    scene.h
    sampler.h
    reflectance.h
    photon_map.h)

set(HEADER_RENDER ${HEADER_RENDER}
    render_parameters.h
    path_tracing.h
    progressive_photon_mapping.h
    ppm_probabilistic.h
    subsurface_integrator.h)

add_library(tatsy_pppm SHARED ${SOURCE_CORE} ${HEADER_CORE} ${SOURCE_RENDER} ${HEADER_RENDER})

source_group(Core FILES ${SOURCE_CORE} ${HEADER_CORE})
source_group(Render FILES ${SOURCE_RENDER} ${HEADER_RENDER})

add_executable(appmain appmain.cc)

link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
target_link_libraries(appmain ${LIB_PREFIX}tatsy_pppm${LIB_POSTFIX})

if (${ONSITE_BUILD} STREQUAL "ON")
  set(ASSET_DIRECTORY "assets")
  set(RESULT_DIRECTORY ".")
else()
  set(ASSET_DIRECTORY "${CMAKE_SOURCE_DIR}/assets")
  set(RESULT_DIRECTORY "${CMAKE_SOURCE_DIR}/results")
  file(MAKE_DIRECTORY ${RESULT_DIRECTORY})
endif()

if (MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi")
  set_property(TARGET appmain APPEND PROPERTY LINK_FLAGS "/DEBUG /PROFILE")
  set_property(TARGET tatsy_pppm APPEND PROPERTY LINK_FLAGS "/DEBUG /PROFILE")
  configure_file(project.vcxproj.user.in "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/appmain.vcxproj.user" @ONLY)
  configure_file(project.vcxproj.user.in "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tatsy_pppm.vcxproj.user" @ONLY)
endif()
