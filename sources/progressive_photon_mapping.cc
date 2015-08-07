#define PROGRESSIVe_PHOTON_MAPPING_EXPORT
#include "progressive_photon_mapping.h"

#include <ctime>
#include <iostream>

#include "common.h"
#include "sampler.h"

#include "random.h"
#include "random_sequence.h"

const double ProgressivePhotonMapping::ALPHA = 0.7;

ProgressivePhotonMapping::ProgressivePhotonMapping()
    : _result()
{
}

ProgressivePhotonMapping::~ProgressivePhotonMapping()
{
}

void ProgressivePhotonMapping::render(const Scene& scene, const Camera& camera, const RenderParameters& params) {
    const int width  = camera.imagesize().width();
    const int height = camera.imagesize().height();
    const int numPixels = width * height;

    std::vector<RenderPoint> rpoints(numPixels);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int id = y * width + x;
            rpoints[id].pixelX = x;
            rpoints[id].pixelY = y;
            rpoints[id].n = 0;
        }
    }

    // Allocate image
    _result.resize(width, height);

    Random rand = Random((unsigned int)time(NULL));
    for (int t = 0; t < params.spp(); t++) {
        std::cout << "--- Iteration No." << (t + 1) << " ---" << std::endl;

        // 1st pass: trace rays from camera
        traceRays(scene, camera, rand, &rpoints);

        // 2nd pass: trace photons from lights
        tracePhotons(scene, rand, params.photons());

        // Save temporal image
        _result.fill(Vector3D(0.0, 0.0, 0.0));
        for (int i = 0; i < numPixels; i++) {
            const RenderPoint& rp = rpoints[i];
            if (rp.pixelX >= 0 && rp.pixelY >= 0 && rp.pixelX < width && rp.pixelY < height) {
                _result.pixel(rp.pixelX, height - rp.pixelY - 1) += (rp.emission + rp.flux / (PI * rp.r2)) * (rp.coeff / (t + 1));
            }
        }

        char filename[512];
        sprintf(filename, (RESULT_DIRECTORY + "progressive_photonmap_%03d.bmp").c_str(), t + 1);
        _result.gamma(2.2, true);
        _result.saveBMP(filename);
    }

}

void ProgressivePhotonMapping::constructHashGrid(std::vector<RenderPoint>& rpoints, int imageW, int imageH) {
    hashgrid.clear();

    const int numPixels = rpoints.size();

    BBox bbox;
    for (int i = 0; i < numPixels; i++) {
        bbox.merge(static_cast<Vector3D>(rpoints[i]));
    }

    // Heuristic for initial radius
    Vector3D boxsize = bbox.posMax() - bbox.posMin();
    const double irad = ((boxsize.x() + boxsize.y() + boxsize.z()) / 3.0) / ((imageW + imageH) / 2.0) * 2.0;

    // Initialize radi
    Vector3D iradv(irad, irad, irad);
    for (int i = 0; i < numPixels; i++) {
        if (rpoints[i].n == 0) {
            rpoints[i].r2 = irad * irad;
            rpoints[i].flux = Vector3D(0.0, 0.0, 0.0);
        }

        bbox.merge(rpoints[i] + iradv);
        bbox.merge(rpoints[i] - iradv);
    }

    // Make each grid cell which is two times larger than the initial radius
    const double hashscale = 1.0 / (irad * 2.0);
    const int hashsize = numPixels;

    hashgrid.init(hashsize, hashscale, bbox);

    // Set render points
    for (int i = 0; i < numPixels; i++) {
        Vector3D boxMin = static_cast<Vector3D>(rpoints[i]) - iradv;
        Vector3D boxMax = static_cast<Vector3D>(rpoints[i]) + iradv;
        hashgrid.add(&rpoints[i], boxMin, boxMax);
    }
}

void ProgressivePhotonMapping::traceRays(const Scene& scene, const Camera& camera, Random& rand, std::vector<RenderPoint>* rpoints) {
    const int width  = camera.imagesize().width();
    const int height = camera.imagesize().height();
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
            RandomSequence rseq;
            omplock {
                rseq.add(rand, 200);                                
            }

            const int pid = pids[threadID][i];
            executePathTracing(scene, camera, rseq, &rpoints->at(pid));

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
    constructHashGrid(*rpoints, width, height);
    std::cout << "Hash grid constructed !!" << std::endl << std::endl;
}

void ProgressivePhotonMapping::tracePhotons(const Scene& scene, Random& rand, int photons, const int bounceLimit) {
    std::cout << "Shooting photons ..." << std::endl;
    int proc = 0;

    ompfor (int pid = 0; pid < photons; pid++) {
        RandomSequence rseq;
        omplock {
            rseq.add(rand, 200);
        }

        const Photon photon = scene.envmap().samplePhoton(rseq, photons);
        const Vector3D posLight    = static_cast<Vector3D>(photon);
        const Vector3D normalLight = photon.normal();

        Vector3D currentFlux = photon.flux();

        Vector3D nextDir; 
        sampler::onHemisphere(normalLight, &nextDir, rseq.pop(), rseq.pop());

        Ray currentRay(posLight, nextDir);
        Vector3D prevNormal = normalLight;

        // Shooting photons
        for (int bounce = 0; ; bounce++) {
            // Remove photons with zero flux
            if (bounce >= bounceLimit || std::max(currentFlux.x(), std::max(currentFlux.y(), currentFlux.z())) <= 0.0) {
                break;
            }

            // Intersection test
            Intersection isect;
            bool isHit = scene.intersect(currentRay, isect);
            if (!isHit) {
                break;
            }

            const int objectID = isect.objectID();
            const BRDF& brdf = scene.getBrdf(objectID);
            const Hitpoint& hitpoint = isect.hitpoint();
            const Vector3D orientNormal = Vector3D::dot(hitpoint.normal(), currentRay.direction()) < 0.0 ? hitpoint.normal() : -hitpoint.normal();

            if (brdf.type() == BRDF_LAMBERTIAN) {
                // Gather render points
                std::vector<RenderPoint*> results;
                omplock {
                    results = hashgrid[hitpoint.position()];
                }

                // Update render points
                for (int i = 0; i < results.size(); i++) {
                    RenderPoint* rpp = results[i];
                    const Vector3D v = (*rpp) - hitpoint.position();
                    if (Vector3D::dot(rpp->normal, hitpoint.normal()) > EPS && (v.squaredNorm() <= rpp->r2)) {
                        double g = (rpp->n * ALPHA + ALPHA) / (rpp->n * ALPHA + 1.0);
                        omplock {
                            rpp->r2 *= g;
                            rpp->n  += 1;
                            rpp->flux = (rpp->flux + rpp->weight * currentFlux * invPI) * g;
                        }
                    }
                }

                // Russian roulette determines if trace is continued or terminated
                const double probability = (brdf.reflectance().x() + brdf.reflectance().y() + brdf.reflectance().z()) / 3.0;
                if (rseq.pop() < probability) {
                    brdf.sample(currentRay.direction(), orientNormal, rseq.pop(), rseq.pop(), &nextDir);
                    currentRay = Ray(hitpoint.position(), nextDir);
                    currentFlux = currentFlux * brdf.reflectance() / probability;
                } else {
                    break;
                }
            } else {
                brdf.sample(currentRay.direction(), orientNormal, rseq.pop(), rseq.pop(), &nextDir);
                currentRay = Ray(hitpoint.position(), nextDir);
                currentFlux = currentFlux * brdf.reflectance();
            }
        }

        omplock {
            proc += 1;
            if (proc % 100 == 0) {
                printf("%6.2f %% processed ...\r", 100.0 * proc / photons);
            }
        }
    }
    printf("\nFinish !!\n\n");
}

void ProgressivePhotonMapping::executePathTracing(const Scene& scene, const Camera& camera, RandomSequence& rseq, RenderPoint* rp, const int bounceLimit) {
    assert(rp->pixelX >= 0 && rp->pixelY >= 0 && rp->pixelX < camera.imagesize().width() && rp->pixelY < camera.imagesize().height() && "Pixel index out of bounds!!");   

    double px = rp->pixelX + rseq.pop() - 0.5;
    double py = rp->pixelY + rseq.pop() - 0.5;
    Ray ray = camera.getRay(px, py);
    const double coeff = camera.sensitivity();

    Intersection isect;
    Vector3D weight(1.0, 1.0, 1.0);
    for (int bounce = 0; ; bounce++) {
        if (bounce >= bounceLimit || !scene.intersect(ray, isect)) {
            rp->weight = weight;
            rp->coeff  = coeff;
            rp->emission += weight * scene.envmap().sampleFromDir(ray.direction());
            break;
        }

        const int objectID = isect.objectID();
        const Hitpoint& hitpoint = isect.hitpoint();
        const BRDF& brdf = scene.getBrdf(objectID);
        const Vector3D orientNormal = Vector3D::dot(hitpoint.normal(), ray.direction()) < 0.0 ? hitpoint.normal() : -hitpoint.normal();

        if (brdf.type() == BRDF_LAMBERTIAN) {
            weight = weight * brdf.reflectance();
            rp->setPosition(hitpoint.position());
            rp->normal = hitpoint.normal();
            rp->weight = weight;
            rp->coeff  = coeff;
            break;
        } else {
            Vector3D nextDir;
            brdf.sample(ray.direction(), orientNormal, rseq.pop(), rseq.pop(), &nextDir);
            ray = Ray(hitpoint.position(), nextDir);
            weight = weight * brdf.reflectance();
        }
    }
}
