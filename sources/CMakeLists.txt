
configure_file(directories.h.in ${CMAKE_CURRENT_LIST_DIR}/directories.h @ONLY)

set(SOURCES vector3d.cc
            ray.cc
            plane.cc
            bbox.cc
            triangle.cc
            trimesh.cc
            orthogonal_camera.cc
            qbvh_accel.cc
            hash_grid.cc
            brdf.cc
            image.cc
            envmap.cc
            scene.cc
            sampler.cc
            progressive_photon_mapping.cc
            photon_diffusion.cc)

set(HEADERS common.h
            renderer.h
            size.h
            random.h
            random_sequence.h
            vector3d.h
            photon.h
            ray.h
            geometry_interface.h
            plane.h
            bbox.h
            triangle.h
            trimesh.h
            orthogonal_camera.h
            qbvh_accel.h
            hash_grid.h
            random_queue.h
            brdf.h
            image.h
            envmap.h
            scene.h
            sampler.h
            render_parameters.h
            progressive_photon_mapping.h
            photon_diffusion.h)

add_library(photon_diffusion SHARED ${SOURCES} ${HEADERS})

add_executable(appmain appmain.cc)
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
target_link_libraries(appmain ${LIB_PREFIX}photon_diffusion${LIB_SUFFIX})
