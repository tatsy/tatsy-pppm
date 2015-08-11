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

set(SOURCES vector3d.cc
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
            photon_map.cc
            path_tracing.cc
            progressive_photon_mapping.cc
            ppm_probabilistic.cc
            subsurface_integrator.cc
            photon_diffusion.cc)

set(HEADERS common.h
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
            render_parameters.h
            photon_map.h
            path_tracing.h
            progressive_photon_mapping.h
            ppm_probabilistic.h
            subsurface_integrator.h
            photon_diffusion.h)

add_library(spica_for_rt3_renderer SHARED ${SOURCES} ${HEADERS})

add_executable(appmain appmain.cc)

link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
target_link_libraries(appmain ${LIB_PREFIX}spica_for_rt3_renderer${LIB_SUFFIX})

if (${ONSITE_BUILD} STREQUAL "ON")
  set(ASSET_DIRECTORY "assets")
  set(RESULT_DIRECTORY ".")
else()
  set(ASSET_DIRECTORY "${CMAKTE_SOURCE_DIR}/assets")
  set(RESULT_DIRECTORY "${CMAKTE_SOURCE_DIR}/results")
  file(MAKE_DIRECTORY ${RESULT_DIRECTORY})
endif()

if (MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi")
  set_property(TARGET appmain APPEND PROPERTY LINK_FLAGS "/DEBUG /PROFILE")
  set_property(TARGET spica_for_rt3_renderer APPEND PROPERTY LINK_FLAGS "/DEBUG /PROFILE")
  configure_file(project.vcxproj.user.in "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/appmain.vcxproj.user" @ONLY)
  configure_file(project.vcxproj.user.in "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/photon_diffusion.vcxproj.user" @ONLY)
endif()
