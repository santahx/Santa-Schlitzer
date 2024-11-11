#ifndef slic3r_FillRotatingTriangles_hpp_
#define slic3r_FillRotatingTriangles_hpp_

#include "../libslic3r.h"
#include "FillBase.hpp"
#include "../ClipperUtils.hpp"  // Falls scale_ hier definiert ist
#include <map>

namespace Slic3r {

class FillRotatingTriangles : public Fill
{
public:
    Fill* clone() const override { return new FillRotatingTriangles(*this); }
    ~FillRotatingTriangles() override {}

protected:
    // Definiere die Membervariablen direkt hier, um Fehler zu vermeiden
    coord_t _min_spacing{0};
    coord_t _line_spacing{0};

    void _fill_surface_single(
        const FillParams                &params,
        unsigned int                     thickness_layers,
        const std::pair<float, Point>   &direction,
        ExPolygon                        expolygon,
        Polylines                       &polylines_out) override;
};

} // namespace Slic3r

#endif // slic3r_FillRotatingTriangles_hpp_
