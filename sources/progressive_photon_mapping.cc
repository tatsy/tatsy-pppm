#include "progressive_photon_mapping.h"

#include <iostream>

const double ProgressivePhotonMapping::ALPHA = 0.7;

ProgressivePhotonMapping::ProgressivePhotonMapping()
{
}

void ProgressivePhotonMapping::render(const Scene& scene, const RenderParameters& renderParam) {
    const int width  = renderParam.width();
    const int height = renderParam.height();
    const int numPixels = width * height;

}

void ProgressivePhotonMapping::traceRays(const Scene& scene, const RenderParameters& renderParams) {
    const int width  = renderParams.width();
    const int height = renderParams.height();
    const int numPixels = width * height;

    // Generate a ray to cast
    std::cout << "Tracing rays from camera ..." << std::endl;

    // Distribute pixels to each thread
    std::vector<std::vector<int> > pids(OMP_NUM_CORE);
    for (int i = 0; i < numPixels; i++) {
        pids[i % OMP_NUM_CORE].push_back(i);
    }

    int proc = 0;
    ompfor (int threadID = 0; threadID < OMP_NUM_CORE; threadID++) {
        const int taskPerThread = (int)pids[threadID].size();
        for (int i = 0; i < taskPerThread; i++) {
            omplock {
                
            }

            const int pid = pids[threadID][i];
            executePathTracing(scene, );

            omplock {
                proc += 1;
                if (proc % width == 0) {
                    printf("%6.2f %% processed ... \r", 100.0 * proc / numPixels);
                }
            }
        }
    }
    printf("\nFinish !!\n");

    // Construct k-d tree
    constructHashGrid();
    std::cout << "Hash grid constructed !!" << std::endl << std::endl;
}

void ProgressivePhotonMapping::executePathTracing(const Scene& scene, const RenderParameters& renderParams, Random& rng, RenderPoint* rp, const int bounceLimit) {
    assert(rp->pixelX >= 0 && rp->pixelY >= 0 && rp->pixelX < renderParams.width() && rp->pixelY < renderParams.height() && "Pixel index out of bounds!!");   

    Ray ray = Ray(/*-- TODO: CHECK HERE --*/);

    Intersection isect;
    Vector3D weight(1.0, 1.0, 1.0);
    for (int bounce = 0; ; bounce++) {
        if (!scene.intersect(ray, isect) || bounce > bounceLimit) {
            rp->weight = weight;
            rp->coeff  = coeff;
        }

        const int objectID = isect.objectID();
        const Hitpoint& hitpoint = isect.hitpoint();
        const BRDF& brdf = scene.getBrdf(objectID);
        const Vector3D orientNormal = Vector3D::dot(hitpoint.normal(), ray.direction()) < 0.0 ? hitpoint.normal() : -hitpoint.normal();

        
    }
}
