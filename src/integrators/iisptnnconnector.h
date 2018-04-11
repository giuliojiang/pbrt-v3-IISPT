#ifndef IISPTNNCONNECTOR_H
#define IISPTNNCONNECTOR_H

#include <memory>
#include "tools/childprocess.hpp"
#include "film/distancefilm.h"
#include "film/imagefilm.h"
#include "film/intensityfilm.h"
#include "film/normalfilm.h"

namespace pbrt {

// Represents an instance of a child process connected to the python
// neural network
class IisptNnConnector
{

private: // ===================================================================

    std::unique_ptr<ChildProcess> child_process;

    void pipe_image_film(std::shared_ptr<ImageFilm> film);

    std::shared_ptr<IntensityFilm> read_image_film(
            int &status
            );

public: // ====================================================================

    // Constructor
    IisptNnConnector();

    // Communicate
    std::shared_ptr<IntensityFilm> communicate(
            std::shared_ptr<IntensityFilm> intensity,
            std::shared_ptr<DistanceFilm> distance,
            std::shared_ptr<NormalFilm> normals,
            Float intensity_normalization,
            Float distance_normalization,
            int &status
            );

};

} // namespace pbrt

#endif // IISPTNNCONNECTOR_H
