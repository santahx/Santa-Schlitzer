#include "../ClipperUtils.hpp"
#include "../ExPolygon.hpp"
#include "../ShortestPath.hpp"
#include "../Surface.hpp"
#include <cmath>  // Für trigonometrische Funktionen
#include "FillRotatingTriangles.hpp"

namespace Slic3r {

    // Die Methode gehört zur Klasse FillRotatingTriangles
    void FillRotatingTriangles::_fill_surface_single(
        const FillParams                &params,
        unsigned int                     thickness_layers,
        const std::pair<float, Point>   &direction,
        ExPolygon                        expolygon,
        Polylines                       &polylines_out)
    {
        // Setze den minimalen Abstand
        this->_min_spacing = scale_(this->spacing);
        assert(params.density > 0.0001f && params.density <= 1.f);

        // Berechne den Rotationswinkel für die Schicht
        float rotation_per_layer = 5.0f * thickness_layers;  // Rotation erhöht sich mit der Layerhöhe
        float rotation_angle = direction.first + rotation_per_layer;
        expolygon.rotate(-rotation_angle);

        this->_line_spacing = coord_t(coordf_t(this->_min_spacing) / params.density);

        // Erzeuge das Dreiecksmuster
        BoundingBox bounding_box = expolygon.contour.bounding_box();
        coord_t x_max = bounding_box.max(0) + SCALED_EPSILON;

        Lines triangles;
        for (coord_t x = bounding_box.min(0); x <= x_max; x += this->_line_spacing) {
            for (coord_t y = bounding_box.min(1); y <= bounding_box.max(1); y += this->_line_spacing) {
                // Erstelle drei Punkte für das Dreieck
                Point pt1(x, y);
                Point pt2(x + this->_line_spacing / 2, y + this->_line_spacing);
                Point pt3(x + this->_line_spacing, y);

                // Drehe das Dreieck relativ zur Bounding Box
                pt1.rotate(rotation_angle, bounding_box.center());
                pt2.rotate(rotation_angle, bounding_box.center());
                pt3.rotate(rotation_angle, bounding_box.center());

                // Füge das Dreieck als Linie in das Array hinzu
                triangles.push_back(Line(pt1, pt2));
                triangles.push_back(Line(pt2, pt3));
                triangles.push_back(Line(pt3, pt1));
            }
        }

        // Schnittoperation für Dreieckslinien
        Polylines polylines_src;
        for (const Line &triangle_line : triangles) {
            polylines_src.push_back(Polyline());
            polylines_src.back().points.push_back(triangle_line.a);
            polylines_src.back().points.push_back(triangle_line.b);
        }
        Polylines polylines = intersection_pl(polylines_src, offset(expolygon, scale_(0.02)));

        // Füge die resultierenden Polylines in polylines_out ein
        for (const auto &polyline : polylines) {
            polylines_out.push_back(polyline);
        }

        // Rotation rückgängig machen, um die Linien in die Originalposition zu bringen
        for (auto &polyline : polylines_out) {
            polyline.rotate(rotation_angle);
        }
    }

} // namespace Slic3r
