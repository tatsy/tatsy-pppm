
configure_file(directories.h.in ${CMAKE_CURRENT_LIST_DIR}/directories.h @ONLY)

set(SOURCES vector3d.cc
            vector3d_sse.cc
            halton.cc
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
            random.h
            halton.h
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
            subsurface_integrator.h
            photon_diffusion.h)

add_library(photon_diffusion SHARED ${SOURCES} ${HEADERS})
# set_property(TARGET photon_diffusion APPEND PROPERTY LINK_FLAGS /DEBUG)

add_executable(appmain appmain.cc)
# set_property(TARGET appmain APPEND PROPERTY LINK_FLAGS /DEBUG)

link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
target_link_libraries(appmain ${LIB_PREFIX}photon_diffusion${LIB_SUFFIX})
