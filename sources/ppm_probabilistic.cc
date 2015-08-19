#define PPM_PROBABILISTIC_EXPORT
#include "ppm_probabilistic.h"

#include "timer.h"
#include "halton.h"
#include "sampler.h"
#include "reflectance.h"

const double ProgressivePhotonMappingProb::ALPHA = 0.7;

ProgressivePhotonMappingProb::ProgressivePhotonMappingProb()
    : _result()
    , _integrator(NULL)
    , _radius(0.0)
    , _photonMap()
{
}

ProgressivePhotonMappingProb::~ProgressivePhotonMappingProb()
{
    delete _integrator;
}

void ProgressivePhotonMappingProb::render(const Scene& scene, const Camera& camera, const RenderParameters& params, RandomSamplerType randomSamplerType) {
    const int width  = camera.imagesize().width();
    const int height = camera.imagesize().height();
    const int numPixels = width * height;

    // Start timer
    Timer timer;
    timer.start();

    // Preprocess to account for subsurface scattering
    bool enableBssrdf = false;
    double areaRadius = 0.0;
    double avgArea = 0.0;
    for (int i = 0; i < scene.numTriangles(); i++) {
        if (scene.getBsdf(i).type() & BSDF_TYPE_BSSRDF) {
            enableBssrdf = true;
            avgArea += scene.getTriangle(i).area();
        }
    }
    avgArea /= scene.numTriangles();

    if (enableBssrdf) {
        areaRadius = sqrt(avgArea) * 1.0;
        _integrator = new SubsurfaceIntegrator();
    }

    // Compute global radius for PPM-APA
    BBox bbox;
    for (int i = 0; i < scene.numTriangles(); i++) {
        bbox.merge(scene.getTriangle(i));
    }
    _radius = (bbox.posMax() - bbox.posMin()).norm() * 0.1;

    // Prepare random samplers
    RandomSampler* rsamplers = new RandomSampler[OMP_NUM_CORE];
    for (int i = 0; i < OMP_NUM_CORE; i++) {
        switch (randomSamplerType) {
        case RANDOM_SAMPLER_PSEUDO_RANDOM:
            rsamplers[i] = Random::generateSampler(i);
            break;

        case RANDOM_SAMPLER_QUASI_MONTE_CARLO:
            rsamplers[i] = Halton::generateSampler(200, true, i);
            break;

        default:
            std::cerr << "[ERROR] unknown random sampler type !!" << std::endl;
            std::abort();
        }
    }

    // Rendering
    bool isFinish = false;
    _result.resize(width, height);
    Image buffer(width, height);
    buffer.fill(Vector3D(0.0, 0.0, 0.0));
    for (int t = 1; t <= params.spp(); t++) {
        std::cout << "--- Iteration No." << t << " ---" << std::endl;
        
        if (enableBssrdf) {
            _integrator->initialize(scene, params, areaRadius, 0.05);
        }

        // 1st pass: conventional photon mapping (and account for subsurface scattering)
        tracePhotons(scene, camera, params, rsamplers);

        // 2nd pass: estimate radiance
        traceRays(&buffer, scene, camera, params, rsamplers);

        // Update radius
        _radius = (t + 1.0) / (t + ALPHA) * _radius;

        // Save intermediate result
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                _result.pixel(x, height - y - 1) = buffer(x, y) / t;
            }
        }
        _result.gamma(2.2, true);

        char filename[1024];
        sprintf(filename, (RESULT_DIRECTORY + "%03d.png").c_str(), t);
        _result.save(filename);
        printf("%.2f sec: %d / %d\n", timer.stop(), t, params.spp());

        if (timer.stop() > 875.0 && !isFinish) {
            printf("About 15 min elapsed!!\n");
            _result.save(RESULT_DIRECTORY + "final_result.png");
            isFinish = true;
#ifdef __ONSITE__
            break;
#endif
        }
    }

    // Deallocate memories
    delete[] rsamplers;
}

void ProgressivePhotonMappingProb::tracePhotons(const Scene& scene, const Camera& camera, const RenderParameters& params, RandomSampler* rsamplers, int bounceLimit) {
    const int numPhotons = params.photons();

    std::cout << "Shooting photons ..." << std::endl;
    int proc = 0;

    const int taskPerThread = (numPhotons + OMP_NUM_CORE - 1) / OMP_NUM_CORE;

    std::vector<std::vector<Photon> > photons(OMP_NUM_CORE);
    for (int i = 0; i < taskPerThread; i++) {
        ompfor (int threadID = 0; threadID < OMP_NUM_CORE; threadID++) {
            RandomSequence rseq;
            rsamplers[threadID].request(200, &rseq);

            const Photon photon = scene.envmap().samplePhoton(rseq, numPhotons);
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

                // Request random numbers
                const double rands[3] = { rseq.pop(), rseq.pop(), rseq.pop() };

                // Next bounce
                const int objectID = isect.objectID();
                const BSDF& bsdf = scene.getBsdf(objectID);
                const Hitpoint& hitpoint = isect.hitpoint();
                const Vector3D orientNormal = Vector3D::dot(hitpoint.normal(), currentRay.direction()) < 0.0 ? hitpoint.normal() : -hitpoint.normal();

                if (bsdf.type() & BSDF_TYPE_LAMBERTIAN_BRDF) {
                    // Gather render points
                    photons[threadID].push_back(Photon(hitpoint.position(), currentFlux, currentRay.direction(), hitpoint.normal()));

                    // Russian roulette determines if trace is continued or terminated
                    const double probability = (bsdf.reflectance().x() + bsdf.reflectance().y() + bsdf.reflectance().z()) / 3.0;
                    if (rands[0] < probability) {
                        double pdf = 1.0;
                        bsdf.sample(currentRay.direction(), orientNormal, rands[1], rands[2], &nextDir, &pdf);
                        currentRay = Ray(hitpoint.position(), nextDir);
                        currentFlux = currentFlux * bsdf.reflectance() / probability;
                    } else {
                        break;
                    }
                } else {
                    double pdf = 1.0;
                    bsdf.sample(currentRay.direction(), orientNormal, rands[0], rands[1], &nextDir, &pdf);
                    currentRay = Ray(hitpoint.position(), nextDir);
                    currentFlux = currentFlux * bsdf.reflectance() / pdf;
                }
            }
        }

        proc += OMP_NUM_CORE;
        if (proc % 100 == 0) {
            printf("%6.2f %% processed ...\r", 100.0 * proc / numPhotons);
        }
    }
    printf("\nFinish!!\n");

    // Construct photon map
    std::vector<Photon> photonsAll(taskPerThread * OMP_NUM_CORE);
    for (int i = 0; i < OMP_NUM_CORE; i++) {
        std::copy(photons[i].begin(), photons[i].end(), photonsAll.begin() + i * taskPerThread);
    }
    _photonMap.construct(photonsAll);
}

void ProgressivePhotonMappingProb::traceRays(Image* buffer, const Scene& scene, const Camera& camera, const RenderParameters& params, RandomSampler* rsamplers) const {
    const int width = camera.imagesize().width();
    const int height = camera.imagesize().height();

    const int taskPerThread = (height + OMP_NUM_CORE - 1) / OMP_NUM_CORE;
    std::vector<std::vector<int> > tasks(OMP_NUM_CORE);
    for (int y = 0; y < height; y++) {
        tasks[y % OMP_NUM_CORE].push_back(y);
    }

    for (int i = 0; i < taskPerThread; i++) {
        ompfor (int threadID = 0; threadID < OMP_NUM_CORE; threadID++) {
            if (i < tasks[threadID].size()) {
                const int y = tasks[threadID][i];
                for (int x = 0; x < width; x++) {
                    RandomSequence rseq;
                    rsamplers[threadID].request(200, &rseq);
                    buffer->pixel(x, y) += executePathTracing(scene, camera, params, x, y, rseq);
                }
            }
        }
        printf("%6.2f %% processed ...\r", 100.0 * (i + 1) / taskPerThread);
    }
    printf("\nFinish!!\n");
}

Vector3D ProgressivePhotonMappingProb::executePathTracing(const Scene& scene, const Camera& camera, const RenderParameters& params, int pixelX, int pixelY, RandomSequence& rseq, int bounceLimit) const {
    Assertion(pixelX >= 0 && pixelY >= 0 && pixelX < camera.imagesize().width() && pixelY < camera.imagesize().height(), "Pixel index out of bounds!!");   

    const double px = pixelX + rseq.pop() - 0.5;
    const double py = pixelY + rseq.pop() - 0.5;
    Ray ray = camera.getRay(px, py);

    return radiance(scene, ray, params, rseq, 0, bounceLimit);
}

Vector3D ProgressivePhotonMappingProb::radiance(const Scene& scene, const Ray& ray, const RenderParameters& params, RandomSequence& rseq, int bounces, int bounceLimit) const {
    // Terminate trace if the bounces reach limit or not intersect the scene
    Intersection isect;
    if (bounces >= bounceLimit || !scene.intersect(ray, isect)) {
        return scene.envmap().sampleFromDir(ray.direction());
    }

    // Request random numbers
    const double rands[3] = { rseq.pop(), rseq.pop(), rseq.pop() };

    // Next bounce
    const int objectID = isect.objectID();
    const Hitpoint& hitpoint = isect.hitpoint();
    const BSDF& bsdf = scene.getBsdf(objectID);

    double roulette  = std::max(bsdf.reflectance().x(), std::max(bsdf.reflectance().y(), bsdf.reflectance().z()));
    if (bounces >= 3) {
        if (rands[0] > roulette) {
            return Vector3D(0.0, 0.0, 0.0);
        }
    } else {
        roulette = 1.0;
    }

    Vector3D throughput(0.0, 0.0, 0.0);

    // Account for subsurface scattering
    if (bsdf.type() & BSDF_TYPE_BSSRDF) {
        if (bsdf.type() & BSDF_TYPE_REFRACTION) {
            bool into = Vector3D::dot(hitpoint.normal(), ray.direction()) < 0.0;
            const Vector3D orieintingNormal = into ? hitpoint.normal() : -hitpoint.normal();
            Vector3D reflectDir, transmitDir;
            double fresnelRe, fresnelTr;
            if (checkTotalReflection(into, ray.direction(), hitpoint.normal(), orieintingNormal, &reflectDir, &transmitDir, &fresnelRe, &fresnelTr)) {
                const Ray nextRay(hitpoint.position(), reflectDir);
                return radiance(scene, nextRay, params, rseq, bounces + 1, bounceLimit);
            } else {
                const double probability = 0.25 + REFLECT_PROBABILITY * 0.5;
                if (rands[1] < probability) {
                    // Reflection
                    const Ray nextRay(hitpoint.position(), reflectDir);
                    return bsdf.reflectance() * radiance(scene, nextRay, params, rseq, bounces, bounceLimit) * (fresnelRe / probability);
                } else {
                    // Transmit
                    return _integrator->irradiance(hitpoint.position(), bsdf) * (fresnelTr / (1.0 - probability));
                }
            }
        } else {
            const double probability = 0.25 + REFLECT_PROBABILITY * 0.5;
            Vector3D irad = _integrator->irradiance(hitpoint.position(), bsdf);
            throughput += irad * (1.0 - probability);
        }
    }

    if (bsdf.type() & BSDF_TYPE_LAMBERTIAN_BRDF) {
        // Estimate irradiance with photon map
        Photon query = Photon(hitpoint.position(), Vector3D(), ray.direction(), hitpoint.normal());
        std::vector<Photon> photons;
        _photonMap.findKNN(query, &photons, params.gatherPhotons(), _radius);

        const int numPhotons = static_cast<int>(photons.size());

        std::vector<Photon> validPhotons;
        std::vector<double> distances;
        double maxdist = 0.0;
        for (int i = 0; i < numPhotons; i++) {
            Vector3D diff = query - photons[i];
            double dist = diff.norm();
            if (std::abs(Vector3D::dot(hitpoint.normal(), diff) / dist) < _radius * _radius * 0.01) {
                validPhotons.push_back(photons[i]);
                distances.push_back(dist);
                maxdist = std::max(maxdist, dist);
            }
        }

        // Cone filter
        const int numValidPhotons = static_cast<int>(validPhotons.size());
        const double k = 1.1;
        Vector3D totalFlux = Vector3D(0.0, 0.0, 0.0);
        for (int i = 0; i < numValidPhotons; i++) {
            const double w = 1.0 - (distances[i] / (k * maxdist));
            const Vector3D v = bsdf.reflectance() * photons[i].flux() * invPI;
            totalFlux += w * v;
        }
        totalFlux /= (1.0 - 2.0 / (3.0 * k));
            
        if (maxdist > EPS) {
            throughput += totalFlux / (PI * maxdist * maxdist * roulette);
        }
    } else {
        double pdf = 1.0;
        Vector3D nextDir;
        bsdf.sample(ray.direction(), hitpoint.normal(), rands[1], rands[2], &nextDir, &pdf);

        Ray nextRay(hitpoint.position(), nextDir);
        throughput += bsdf.reflectance() * radiance(scene, nextRay, params, rseq, bounces + 1, bounceLimit) / (pdf * roulette);
    }
    return throughput;
}
