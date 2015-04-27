#ifndef SPLINE_H
#define SPLINE_H

#include "geometry.h"
#include <cmath>

namespace TURAG {

/**
 * @brief Polynomial of a given order that can be used to calculate its own and its derivations' steps.
 */
template<std::size_t order>
class Polynomial
{
	static_assert(order == 3 || order == 5, "only order 3 and 5 implemented");
public:
	template<std::size_t derivation = 0>
	inline float val(float t) const { return 0.f; }
	float val(float t, unsigned derivation) const;

	constexpr float getNode(unsigned index) const {
        return c[index];
    }

	const float& operator[](unsigned index) const {
		return c[index];
	}

	float& operator[](unsigned index) {
		return c[index];
	}

    // polynomial coefficients
    float c[order+1];
};

template<> template<>
inline float Polynomial<3>::val<0>(float t) const {
	return ((c[0] * t + c[1]) * t + c[2]) * t + c[3];
}
template<> template<>
inline float Polynomial<3>::val<1>(float t) const {
	return (3.0f * c[0] * t + 2.0f * c[1]) * t + c[2];
}
template<> template<>
inline float Polynomial<3>::val<2>(float t) const {
	return 6.0f * c[0] * t + 2.0f * c[1];
}
template<> template<>
inline float Polynomial<3>::val<3>(float) const {
	return 6.0f * c[0];
}
template<>
inline float Polynomial<3>::val<0>(float t, unsigned derivation) const {
	switch (derivation) {
	case 0:
		return ((c[0] * t + c[1]) * t + c[2]) * t + c[3];
	case 1:
		return (3.0f * c[0] * t + 2.0f * c[1]) * t + c[2];
	case 2:
		return 6.0f * c[0] * t + 2.0f * c[1];
	case 3:
		return 6.0f * c[0];
	default:
		return 0.0f;
	}
}


template<> template<>
inline float Polynomial<5>::val<0>(float t) const {
	return ((((c[0] * t + c[1]) * t + c[2]) * t + c[3]) * t + c[4]) * t + c[5];
}
template<> template<>
inline float Polynomial<5>::val<1>(float t) const {
	return (((5.0f * c[0] * t + 4.0f * c[1]) * t + 3.0f * c[2]) * t +
			2.0f * c[3]) * t + c[4];
}
template<> template<>
inline float Polynomial<5>::val<2>(float t) const {
	return ((20.0f * c[0] * t + 12.0f * c[1]) * t + 6.0f * c[2]) * t +
			2.0f * c[3];
}
template<> template<>
inline float Polynomial<5>::val<3>(float t) const {
	return (60.0f * c[0] * t + 24.0f * c[1]) * t + 6.0f * c[2];
}
template<> template<>
inline float Polynomial<5>::val<4>(float t) const {
	return 60.0f * c[0] * t + 24.0f * c[1];
}
template<> template<>
inline float Polynomial<5>::val<5>(float) const {
	return 60.0f * c[0];
}
template<>
inline float Polynomial<5>::val(float t, unsigned derivation) const {
	switch (derivation) {
	case 0:
		return ((((c[0] * t + c[1]) * t + c[2]) * t + c[3]) * t + c[4]) * t + c[5];
	case 1:
		return (((5.0f * c[0] * t + 4.0f * c[1]) * t + 3.0f * c[2]) * t +
				2.0f * c[3]) * t + c[4];
	case 2:
		return ((20.0f * c[0] * t + 12.0f * c[1]) * t + 6.0f * c[2]) * t +
				2.0f * c[3];
	case 3:
		return (60.0f * c[0] * t + 24.0f * c[1]) * t + 6.0f * c[2];
	case 4:
		return 60.0f * c[0] * t + 24.0f * c[1];
	case 5:
		return 60.0f * c[0];
	default:
		return 0.0f;
	}
}

/**
 * @brief 2-dimensional polynomial
 */
template<std::size_t order>
class Polynomial2D
{
public:
    // polynomial coefficients
    Polynomial<order> x, y;
};

/**
 * @brief Spline interface for all types of splines (Catmull-Rom, Hermite, Bezier, ...)
 */
class Spline
{
public:
    virtual bool calculate(Pose *poses, unsigned pose_index, unsigned pose_count, int direction) = 0;
    virtual Pose getPoseStep(float t) = 0;
    virtual float getBendingStep(float t) = 0;
    virtual constexpr unsigned getOrder() = 0;
    virtual constexpr Length getLength() = 0;
    virtual constexpr float getKappaMax() = 0;
    virtual constexpr Length getDirectDist() = 0;

protected:
    static constexpr Length spline_iteration_distance = 50 * Units::mm;
    static constexpr float spline_form_factor = 1.0f;
    static constexpr float mc_no_angle = 4.f;

	static inline bool isAngleDontCare(Units::Angle a)
	{
		return (fabsf(a.to(Units::rad) - mc_no_angle) < 0.001f) || std::isnan(a.to(Units::rad));
	}

	/**
	 * @brief calc_spline_angle calculate missing angle of spline point
	 */
	static void calc_spline_angle(Pose *p, unsigned i, unsigned pn, int dir) {
		if (i < pn && isAngleDontCare(p[i].phi)) {
			if (i > 0) {
				p[i].phi = angle_between(p[i-1], p[i+1]);
			} else {
				p[i].phi = angle_between(p[i], p[i+1]);
			}

			if (dir == -1) {
				p[i].turn(180 * Units::deg); // turn angle for drive backwards
			}
		}

		if ((i+1) < pn && isAngleDontCare(p[i+1].phi)) {
			if (i < pn-2){
				p[i+1].phi = angle_between(p[i], p[i+2]);
			} else {
				p[i+1].phi = angle_between(p[i], p[i+1]);
			}

			if (dir == -1) {
				p[i+1].turn(180 * Units::deg); // turn angle for drive backwards
			}
		}
	}
};

/**
 * @brief SplineOrder Spline of a certain order
 */
template<std::size_t order>
class SplineOrder final :
        public Spline
{
	static_assert(order == 3 || order == 5, "only order 3 and 5 implemented");
public:
	bool calculate(Pose *poses, unsigned pose_index, unsigned pose_count, int direction) override {
        drive_direction = direction;

        if (order == 3) {
            calc_spline_angle(poses, pose_index, pose_count, direction);
            return calc_spline_catmullrom(poses, pose_index);
        } else if (order == 5) {
            calc_spline_angle(poses, pose_index, pose_count, direction);
            return calc_spline_hermite(poses, pose_index);
        }

        return false;
    }

	constexpr unsigned getOrder() const override {
        return order;
    }

	constexpr Length getLength() const override {
        return length;
    }

	constexpr float getKappaMax() const override {
        return kappa_max;
    }

	constexpr Length getDirectDist() const override {
        return direct_dist;
    }

	Pose getPoseStep(float t) override
    {
        Pose p;

		p.x = c.x.val(t) * Units::mm;
		p.y = c.y.val(t) * Units::mm;

        // calculate angle from tangent
		float dx = c.x.template val<1>(t);
		float dy = c.y.template val<1>(t);

        p.phi = atan2f(dy, dx) * Units::rad;

        // for backwards driven splines the angles phi correspond to the robot's angle
        // but to get the desired spline form without curls we need to pretend it's driven forward
        if (drive_direction == -1) {
            p.turn(180 * Units::deg);
        }

        return p;
    }

	float getBendingStep(float t) override
    {
		float dx = c.x.template val<1>(t);
		float dy = c.y.template val<1>(t);

		float ddx = c.x.template val<2>(t);
		float ddy = c.y.template val<2>(t);

        float bending = (dx * ddy - ddx * dy) / cbcf(hypotf(dx, dy));
        return bending;
    }

private:

	bool calc_spline_catmullrom(Pose *poses, unsigned pose_index)
    {
        //static_assert(order == 3, "wrong spline order for this call");

        Pose *p = poses;
        unsigned i = pose_index;

        // see getPoseStep()
        Angle aoff = 0 * Units::deg;
        if (drive_direction == -1) {
            aoff = 180 * Units::deg;
        }

        // spline form depends from distance between p[i] and p[i+1]
        float d12 = distance(p[i], p[i+1]).to(Units::mm);
        float k = spline_form_factor * d12;

		float vx[4] = { k * cos(p[i].phi + aoff),
						k * cos(p[i + 1].phi + aoff),
                        p[i].x.to(Units::mm),
                        p[i + 1].x.to(Units::mm) };
		float vy[4] = { k * sin(p[i].phi + aoff),
						k * sin(p[i + 1].phi + aoff),
                        p[i].y.to(Units::mm),
                        p[i + 1].y.to(Units::mm) };

        /* invertierte hermitesche kubische Matrix (siehe Wikipedia)
         *
         * errechnet Splineparameter anhand gegebener Randwerte:
         * - Position p
         * - Tangente bzw. Winkel m
         *
         *  v = [m0 m1 p0 p1]' siehe oben
         */
        int m[4][4]=   {{ 1, 1, 2,-2},
                        {-2,-1,-3, 3},
                        { 1, 0, 0, 0},
                        { 0, 0, 1, 0}};

        /*
         * Matrixmultiplikation c = M * v
         */
        for (int i = 0; i < 4; i++) {
            c.x.c[i] = 0.0f;
            c.y.c[i] = 0.0f;

            for (int j = 0; j < 4; j++) {
                c.x.c[i] += (float)m[i][j] * vx[j];
                c.y.c[i] += (float)m[i][j] * vy[j];
            }
        }

        direct_dist = d12 * Units::mm;

        bool ok = calculate_parameters();
        return ok;
    }

	bool calc_spline_hermite(Pose *poses, unsigned pose_index)
    {
        //static_assert(order == 5, "wrong spline order for this call");

        Pose *p = poses;
        unsigned i = pose_index;

        // see getPoseStep()
        Angle aoff = 0 * Units::deg;
        if (drive_direction == -1) {
            aoff = 180 * Units::deg;
        }

        // spline form depends from distance between p[i] and p[i+1]
        float d12 = distance(p[i], p[i+1]).to(Units::mm);
        float k = spline_form_factor * d12;

        float vx[4] = { p[i].x.to(Units::mm),
						k * cos(p[i].phi + aoff),
                        p[i + 1].x.to(Units::mm),
						k * cos(p[i + 1].phi + aoff) };
        float vy[4] = { p[i].y.to(Units::mm),
						k * sin(p[i].phi + aoff),
                        p[i + 1].y.to(Units::mm),
						k * sin(p[i + 1].phi + aoff) };

        int m[3][4] =  {{ -6,-3,  6,-3},
                        { 15, 8,-15, 7},
                        {-10,-6, 10,-4}};

        c.x.c[3] = 0.0f;
        c.y.c[3] = 0.0f;
        c.x.c[4] = vx[1];
        c.y.c[4] = vy[1];
        c.x.c[5] = vx[0];
        c.y.c[5] = vy[0];

        for (int i = 0; i < 3; i++) {
            c.x.c[i] = 0.0f;
            c.y.c[i] = 0.0f;

            for (int j = 0; j < 4; j++) {
                c.x.c[i] += (float)m[i][j] * vx[j];
                c.y.c[i] += (float)m[i][j] * vy[j];

            }
        }

        direct_dist = d12 * Units::mm;

        bool ok = calculate_parameters();
        return ok;
    }

	bool calculate_parameters(void)
    {
        static_assert(order == 3 || order == 5, "wrong spline order for this call");

        float t, dx, dy, ddx, ddy;

        float delta_t = spline_iteration_distance / direct_dist;
        Point p_old = {c.x.getNode(order) * Units::mm, c.y.getNode(order) * Units::mm};

        // max Krümmung
        kappa_max = 0.f;

        // Bogenlänge
        length = 0 * Units::mm;

        // Abtastung des Polynoms
        for (t = delta_t; t <= 1.0f + delta_t; t = t + delta_t) {
            // Wert an Stelle t
            Point p = {c.x.val(t) * Units::mm, c.y.val(t) * Units::mm};

            // Gesamtlänge += Abschnittslänge
            length += distance(p_old, p);

            // merken der Werte für Abstandsberechnung im nächsten Iterationschritt
            p_old = p;

            // Wert der 1. Ableitung an Stelle t
			dx = c.x.template val<1>(t);
			dy = c.y.template val<1>(t);

            // Wert der 2. Ableitung an Stelle t
			ddx = c.x.template val<2>(t);
			ddy = c.y.template val<2>(t);

            // max Krümmung bestimmen
            kappa_max = fmaxf(kappa_max, fabsf((dx * ddy - ddx * dy)) / cbcf(hypotf(dx, dy)));
        }

        // Begrenzung...

        //...der Bogenlänge: min. spline_iteration_distance
        length = max(length, spline_iteration_distance);

        //...der Krümmung: min. 1e-10f
        kappa_max = fmaxf(kappa_max, 1e-10f);

        return true;
    }

    static constexpr std::size_t Order = order;

    Polynomial2D<order> c;
    Length direct_dist;
    Length length;
    float kappa_max;
    int drive_direction;
};

using Spline5 = SplineOrder<5>;
using Spline3 = SplineOrder<3>;

} // namespace TURAG

#endif // SPLINE_H
