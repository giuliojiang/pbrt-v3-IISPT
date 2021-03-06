#ifndef IISPTRENDERRUNNER_H
#define IISPTRENDERRUNNER_H

#include <climits>
#include <unordered_map>

#include "integrators/iispt.h"
#include "integrators/iisptfilmmonitor.h"
#include "integrators/iisptnnconnector.h"
#include "integrators/iisptschedulemonitor.h"
#include "integrators/iispt_d.h"
#include "integrators/directlighting.h"
#include "rng.h"
#include "sampler.h"
#include "camera.h"
#include "cameras/hemispheric.h"
#include "tools/iisptmathutils.h"
#include "tools/iisptrng.h"
#include "tools/iisptpoint2i.h"
#include "samplers/sobol.h"
#include "tools/threadpool.h"
#include "tools/generalutils.h"
#include "integrators/directprogressiveintegrator.h"

namespace pbrt {

// ============================================================================
class IisptRenderRunner
{
private:
    // Fields -----------------------------------------------------------------

    static const int HEMISPHERIC_IMPORTANCE_SAMPLES = 16;

    int thread_no;

    bool stop = false;

    Point2i sampler_pixel_counter = Point2i(0, 0);

    // Shared objects

    std::shared_ptr<IisptScheduleMonitor> schedule_monitor;

    std::shared_ptr<IisptFilmMonitor> film_monitor_indirect;

    std::shared_ptr<IisptFilmMonitor> film_monitor_direct;

    std::shared_ptr<Camera> dcamera;

    std::shared_ptr<const Camera> main_camera;

    // Single objects

    std::shared_ptr<IisptNnConnector> nn_connector;

    std::unique_ptr<IisptRng> rng;

    std::unique_ptr<Sampler> sampler;

    Bounds2i pixel_bounds;

    std::unique_ptr<LightDistribution> lightDistribution;

    // Private methods --------------------------------------------------------

    void generate_random_pixel(int* x, int* y);

    bool find_intersection(RayDifferential r,
            const Scene &scene,
            MemoryArena &arena,
            SurfaceInteraction* isect_out,
            RayDifferential* ray_out,
            Spectrum* beta_out,
            Spectrum* background_out
            , Spectrum *emitted_out);

    double compute_filter_weight(
            int cx, // Centre sampling pixel
            int cy,
            int fx, // Current filter pixel
            int fy,
            float radius, // Filter radius,
            double* scaling_factor // Scaling factor to obtain a gaussian curve
                                   // which has point X=0, Y=1
            );

    Spectrum sample_hemisphere(const Interaction &it, int len,
            float *weights,
            HemisphericCamera **cameras
            );

    Spectrum path_uniform_sample_one_light(
            Interaction &it,
            const Scene &scene,
            MemoryArena &arena,
            bool handleMedia,
            const Distribution1D* lightDistrib
            );

    Spectrum estimate_direct_lighting(
            Interaction &it,
            const Point2f &uScattering,
            const Light &light,
            const Point2f &uLight,
            const Scene &scene,
            MemoryArena &arena,
            bool handleMedia,
            bool specular
            );

    void sampler_next_pixel();

    void compute_fpixel_weights(int len,
            Point2i *neighbour_points,
            HemisphericCamera **hemi_sampling_cameras,
            Point2i f_pixel,
            SurfaceInteraction &f_isect,
            int tilesize,
            RayDifferential &f_ray,
            float *out_probabilities
            , Point3f mainCameraOrigin);

    void normalizeMapsDownstream(IntensityFilm* intensity,
            NormalFilm* normals,
            DistanceFilm* distance
            , float &rmean, float &gmean, float &bmean);

    void transformMapsUpstream(IntensityFilm* intensity,
            float rmean
            , float gmean, float bmean);

    float tileToTileMinimumDistance(
            std::vector<HemisphericCamera*> &hemiSamplingCameras
            );

public:

    // Constructor ------------------------------------------------------------
    IisptRenderRunner(
            std::shared_ptr<IisptScheduleMonitor> schedule_monitor,
            std::shared_ptr<IisptFilmMonitor> film_monitor_indirect,
            std::shared_ptr<IisptFilmMonitor> film_monitor_direct,
            std::shared_ptr<const Camera> main_camera,
            std::shared_ptr<Camera> dcamera,
            std::shared_ptr<Sampler> sampler,
            int thread_no,
            Bounds2i pixel_bounds,
            std::shared_ptr<IisptNnConnector> nnConnector
            );

    // Public methods ---------------------------------------------------------

    virtual void run(const Scene &scene);

    void run_direct(const Scene &scene);

};

}

#endif // IISPTRENDERRUNNER_H
