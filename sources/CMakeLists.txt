
configure_file(directories.h.in ${CMAKE_CURRENT_LIST_DIR}/directories.h @ONLY)

set(SOURCES appmain.cc
            vector3d.cc
            ray.cc
            plane.cc
            bbox.cc
            triangle.cc
            trimesh.cc
            qbvh_accel.cc
            hash_grid.cc
            brdf.cc
            scene.cc
            sampler.cc
            progressive_photon_mapping.cc
            photon_diffusion.cc)

set(HEADERS common.h
            random.h
            vector3d.h
            ray.h
            geometry_interface.h
            plane.h
            bbox.h
            triangle.h
            trimesh.h
            qbvh_accel.h
            hash_grid.h
            random_queue.h
            brdf.h
            scene.h
            sampler.h
            render_parameters.h
            progressive_photon_mapping.h
            photon_diffusion.h)

add_executable(photon_diffusion ${SOURCES} ${HEADERS})
