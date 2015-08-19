#define SUBSURFACE_INTEGRATOR_EXPORT
#include "subsurface_integrator.h"

#include <ctime>
#include <cassert>
#include <iostream>
#include <fstream>
#include <typeinfo>

#include "bssrdf.h"
#include "sampler.h"


SubsurfaceIntegrator::Octree::Octree()
    : _root(NULL)
    , _numCopies(NULL)
    , _parent(NULL)
{
}

SubsurfaceIntegrator::Octree::~Octree()
{
    release();
}

SubsurfaceIntegrator::Octree::Octree(const Octree& octree)
    : _root(NULL)
    , _numCopies(NULL)
    , _parent(NULL)
{
    this->operator=(octree);
}

SubsurfaceIntegrator::Octree& SubsurfaceIntegrator::Octree::operator=(const Octree& octree) {
    release();

    _numCopies = octree._numCopies;
    (*_numCopies) += 1;
    _root = octree._root;
    _parent = octree._parent;

    return *this;
}

void SubsurfaceIntegrator::Octree::release() {
    if (_numCopies != NULL) {
        if ((*_numCopies) == 0) {
            deleteNode(_root);
            delete _numCopies;
            _root = NULL;
            _numCopies = NULL;
        } else {
            (*_numCopies) -= 1;
        }
    }
}

void SubsurfaceIntegrator::Octree::deleteNode(SubsurfaceIntegrator::OctreeNode* node) {
    if (node != NULL) {
        for (int i = 0; i < 8; i++) {
            deleteNode(node->children[i]);
        }
        delete node;
    }
}

void SubsurfaceIntegrator::Octree::construct(SubsurfaceIntegrator* parent, std::vector<IrradiancePoint>& ipoints) {
    // Release current octree
    this->release();

    // Compute new octree
    this->_parent = parent;
    const int numHitpoints = static_cast<int>(ipoints.size());
        
    BBox bbox;
    for (int i = 0; i < numHitpoints; i++) {
        bbox.merge(ipoints[i].pos);
    }
    _numCopies = new int(0);
    _root = constructRec(ipoints, bbox);
}

SubsurfaceIntegrator::OctreeNode* SubsurfaceIntegrator::Octree::constructRec(std::vector<IrradiancePoint>& ipoints, const BBox& bbox) {
    if (ipoints.empty()) {
        return NULL;
    } else if (ipoints.size() == 1) {
        OctreeNode* node = new OctreeNode();
        node->pt = ipoints[0];
        node->bbox = bbox;
        node->isLeaf = true;
        return node;
    }

    const Vector3D posMid = (bbox.posMin() + bbox.posMax()) * 0.5;

    const int numPoints = static_cast<int>(ipoints.size());
    std::vector<std::vector<IrradiancePoint> > childPoints(8);
    for (int i = 0; i < numPoints; i++) {
        const Vector3D& v = ipoints[i].pos;
        int id = (v.x() < posMid.x() ? 0 : 4) + (v.y() < posMid.y() ? 0 : 2) + (v.z() < posMid.z() ? 0 : 1);
        childPoints[id].push_back(ipoints[i]);
    }

    // Compute child nodes
    OctreeNode* node = new OctreeNode();
    for (int i = 0; i < 8; i++) {
        BBox childBox;
        for (int j = 0; j < childPoints[i].size(); j++) {
            childBox.merge(childPoints[i][j].pos);
        }
        node->children[i] = constructRec(childPoints[i], childBox);
    }
    node->bbox = bbox;
    node->isLeaf = false;

    // Accumulate child nodes
    node->pt.pos = Vector3D(0.0, 0.0, 0.0);
    node->pt.normal = Vector3D(0.0, 0.0, 0.0);
    node->pt.area = 0.0;

    double weight = 0.0;
    int childCount = 0;
    for (int i = 0; i < 8; i++) {
        if (node->children[i] != NULL) {
            const double w = luminance(node->children[i]->pt.irad);
            node->pt.pos += w * node->children[i]->pt.pos;
            node->pt.normal += w * node->children[i]->pt.normal;
            node->pt.area += node->children[i]->pt.area;
            node->pt.irad += node->children[i]->pt.irad;
            weight += w;
            childCount += 1;
        }
    }

    if (weight > 0.0) {
        node->pt.pos    /= weight;
        node->pt.normal /= weight;
    }

    if (childCount != 0) {
        node->pt.irad /= childCount;
    }

    return node;
}

Vector3D SubsurfaceIntegrator::Octree::iradSubsurface(const Vector3D& pos, const BSSRDF& bssrdf) const {
    return iradSubsurfaceRec(_root, pos, bssrdf);
}

Vector3D SubsurfaceIntegrator::Octree::iradSubsurfaceRec(OctreeNode* node, const Vector3D& pos, const BSSRDF& bssrdf) const {
    if (node == NULL) return Vector3D(0.0, 0.0, 0.0);

    const double distSquared = (node->pt.pos - pos).squaredNorm();
    double dw = node->pt.area / distSquared;
    if (node->isLeaf || (dw < _parent->_maxError && !node->bbox.inside(pos))) {
        return Vector3D(bssrdf(distSquared) * node->pt.irad * node->pt.area);
    } else {
        Vector3D ret(0.0, 0.0, 0.0);
        for (int i = 0; i < 8; i++) {
            if (node->children[i] != NULL) {
                ret += iradSubsurfaceRec(node->children[i], pos, bssrdf);
            }
        }
        return ret;
    }
}

SubsurfaceIntegrator::SubsurfaceIntegrator()
    : octree()
    , photonMap()
    , dA(0.0)
{
}

SubsurfaceIntegrator::~SubsurfaceIntegrator()
{
}

void SubsurfaceIntegrator::initialize(const Scene& scene, const RenderParameters& params, const double areaRadius, const double maxError) {
    // Clear current photon map
    photonMap.clear();

    // Poisson disk sampling on SSS objects
    int objectID = -1;
    std::vector<Vector3D> points;
    std::vector<Vector3D> normals;

    // Extract triangles with BSSRDF
    std::vector<Triangle> triangles;
    for (int i = 0; i < scene.numTriangles(); i++) {
        if (scene.getBsdf(i).type() & BSDF_TYPE_BSSRDF) {
            triangles.push_back(scene.getTriangle(i));            
        }
    }
    Assertion(!triangles.empty(), "The scene does not have subsurface scattering object!!");

    // Poisson disk sampling
    sampler::poissonDisk(triangles, areaRadius, &points, &normals);

    // Copy material data
    this->dA = (0.5 * areaRadius) * (0.5 * areaRadius) * PI;
    this->_maxError = maxError;

    // Cast photons to compute irradiance at sample points
    buildPhotonMap(scene, params.photons(), 64);

    // Compute irradiance at sample points
    buildOctree(points, normals, params);
}

void SubsurfaceIntegrator::buildOctree(const std::vector<Vector3D>& points, const std::vector<Vector3D>& normals, const RenderParameters& params) {
    // Compute irradiance on each sampled point
    const int numPoints = static_cast<int>(points.size());
    std::vector<Vector3D> irads(numPoints);

    for(int i = 0; i < numPoints; i++) {
        // Estimate irradiance with photon map
        Vector3D irad = irradianceWithPM(points[i], normals[i], params);
        irads[i] = irad;
    }

    // Octree construction
    std::vector<IrradiancePoint> iradPoints(numPoints);
    for (int i = 0; i < numPoints; i++) {
        iradPoints[i].pos = points[i];
        iradPoints[i].normal = normals[i];
        iradPoints[i].area = dA;
        iradPoints[i].irad = irads[i];
    }
    octree.construct(this, iradPoints);
}

Vector3D SubsurfaceIntegrator::irradiance(const Vector3D& p, const BSDF& bsdf) const {
    Assertion(bsdf._bssrdf != NULL, "Specified object does not have BSSRDF!!");
    Vector3D Mo = octree.iradSubsurface(p, *bsdf._bssrdf);
    return Vector3D((1.0 / PI) * (1.0 - bsdf._bssrdf->Fdr()) * Mo);
}

void SubsurfaceIntegrator::buildPhotonMap(const Scene& scene, const int numPhotons, const int bounceLimit) {
    std::cout << "Shooting photons ..." << std::endl;
    int proc = 0;

    const int taskPerThread = (numPhotons + OMP_NUM_CORE - 1) / OMP_NUM_CORE;

    RandomSampler* rand = new RandomSampler[OMP_NUM_CORE];
    for (int i = 0; i < OMP_NUM_CORE; i++) {
        rand[i] = Random::generateSampler((unsigned long)time(NULL) + i);
    }

    // Shooting photons
    std::vector<std::vector<Photon> > photons(OMP_NUM_CORE);
    for (int i = 0; i < taskPerThread; i++) {
        ompfor (int threadID = 0; threadID < OMP_NUM_CORE; threadID++) {
            RandomSequence rseq;
            rand[threadID].request(200, &rseq);

            Photon photon = scene.envmap().samplePhoton(rseq, numPhotons);

            const Vector3D& lightNormal = photon.normal();
            const Vector3D& lightPos    = static_cast<Vector3D>(photon);
            Vector3D currentFlux = photon.flux();

            const double r1 = rseq.pop();
            const double r2 = rseq.pop();
        
            Vector3D nextDir;
            sampler::onHemisphere(lightNormal, &nextDir, r1, r2);
            Ray currentRay(lightPos, nextDir);

            for (int bounce = 0; ; bounce++) {
                double rands[3] = { rseq.pop(), rseq.pop(), rseq.pop() }; 

                // Remove photon with zero flux
                if (bounce >= bounceLimit || std::max(currentFlux.x(), std::max(currentFlux.y(), currentFlux.z())) <= 0.0) {
                    break;
                }

                Intersection isect;
                bool isHit = scene.intersect(currentRay, isect);
                if (!isHit) {
                    break;
                }

                const int objectID = isect.objectID();
                const BSDF& bsdf = scene.getBsdf(objectID);
                const Hitpoint& hitpoint = isect.hitpoint();

                double roulette = 1.0;
                if (bsdf.type() & BSDF_TYPE_BSSRDF) {
                    // Store photon
                    photons[threadID].push_back(Photon(hitpoint.position(), currentFlux, currentRay.direction(), hitpoint.normal()));

                    // Roulette
                    const double probability = (currentFlux.x() + currentFlux.y() + currentFlux.z()) / 3.0;
                    if (rands[0] >= probability) {
                        break;
                    }
                }

                double pdf = 1.0;
                bsdf.sample(currentRay.direction(), hitpoint.normal(), rands[1], rands[2], &nextDir, &pdf);
                currentRay = Ray(hitpoint.position(), nextDir);
                currentFlux = currentFlux * bsdf.reflectance() / (roulette * pdf);
            }
        }

        proc += OMP_NUM_CORE;
        if (proc % 1000 == 0) {
            printf("%6.2f %% processed ...\r", 100.0 * proc / numPhotons);
        }
    }
    printf("\n");
    delete[] rand;

    // Construct photon map
    std::vector<Photon> photonsAll;
    photonsAll.reserve(numPhotons);
    for (int i = 0; i < OMP_NUM_CORE; i++) {
        const int np = (int)photons[i].size();
        for (int j = 0; j < np; j++) {
            photonsAll.push_back(photons[i][j]);
        }
    }
    photonMap.clear();
    photonMap.construct(photonsAll);
}

Vector3D SubsurfaceIntegrator::irradianceWithPM(const Vector3D& p, const Vector3D& n, const RenderParameters& params) const {
    // Estimate irradiance with photon map
    Photon query = Photon(p, Vector3D(), Vector3D(), n);
    std::vector<Photon> photons;
    photonMap.findKNN(query, &photons, params.photons(), params.gatherRadius());

    const int numPhotons = static_cast<int>(photons.size());

    std::vector<Photon> validPhotons;
    std::vector<double> distances;
    double maxdist = 0.0;
    for (int i = 0; i < numPhotons; i++) {
        Vector3D diff = query - photons[i];
        double dist = diff.norm();
        if (std::abs(Vector3D::dot(n, diff) / dist) < params.gatherPhotons() * params.gatherPhotons() * 0.01) {
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
        const Vector3D v = photons[i].flux() * invPI;
        totalFlux += w * v;
    }
    totalFlux /= (1.0 - 2.0 / (3.0 * k));

    if (maxdist > EPS) {
        return totalFlux / (PI * maxdist * maxdist);
    }
    return Vector3D(0.0, 0.0, 0.0);
}
