#ifndef SPLINE_H
#define SPLINE_H

#include "geometry.h"

namespace TURAG {

#if 0
/*
 * scale angle between -pi and pi
 */
inline float scale_angle(float angle) {
    while (angle > M_PIf)
        angle -= 2.0f * M_PIf;

    while (angle <= -M_PIf)
        angle += 2.0f * M_PIf;

    return angle;
}

/*
 * calculate the angle of splinepoint
 */
/*
TODO: calc correct and symmetric
why not computing each segment 1 to pn-2 central and the 0 and pn-1 forward and backward?
*/
inline void calc_spline_angle(Pose *p, unsigned i, unsigned pn, int dir) {
    if (isPsiDontCare(p[i].psi)) {
        if (i > 0) {
            if (dir) {
                p[i].psi = atan2f(p[i+1].y - p[i-1].y, p[i+1].x - p[i-1].x);
            } else {
                p[i].psi = atan2f(p[i-1].y - p[i+1].y, p[i-1].x - p[i+1].x); // turn angle for drive backwards
            }
        } else {
            if (dir) {
                p[i].psi = atan2f(p[i+1].y - p[i].y, p[i+1].x - p[i].x);
            } else {
                p[i].psi = atan2f(p[i].y - p[i+1].y, p[i].x - p[i+1].x);	// turn angle for drive backwards
            }
        }
    }

    if (isPsiDontCare(p[i+1].psi)) {
        if (i < pn-2){
            if (dir) {
                p[i+1].psi = atan2f(p[i+2].y - p[i].y, p[i+2].x - p[i].x);
            } else {
                p[i+1].psi = atan2f(p[i].y - p[i+2].y, p[i].x - p[i+2].x);	// turn angle for drive backwards
            }
        } else {
            if (dir) {
                p[i+1].psi = atan2f(p[i+1].y - p[i].y, p[i+1].x - p[i].x);
            } else {
                p[i+1].psi = atan2f(p[i].y - p[i+1].y, p[i].x - p[i+1].x);	// turn angle for drive backwards
            }
        }
    }
}

inline float polyval3(float *c, float t) {
    return ((c[0] * t + c[1]) * t + c[2]) * t + c[3];
}

inline float polyval3_der(float *c, float t) {
    return (3.0f * c[0] * t + 2.0f * c[1]) * t + c[2];
}

inline float polyval3_dder(float *c, float t) {
    return 6.0f * c[0] * t + 2.0f * c[1];
}

inline float polyval3_ddder(float *c, float t) {
    UNUSED(t);
    return 6.0f * c[0];
}

inline float polyval5(float *c, float t) {
    return ((((c[0] * t + c[1]) * t + c[2]) * t + c[3]) * t + c[4]) * t + c[5];
}

inline float polyval5_der(float *c, float t) {
    return (((5.0f * c[0] * t + 4.0f * c[1]) * t + 3.0f * c[2]) * t +
                    2.0f * c[3]) *
                         t +
                 c[4];
}

inline float polyval5_dder(float *c, float t) {
    return ((20.0f * c[0] * t + 12.0f * c[1]) * t + 6.0f * c[2]) * t +
                 2.0f * c[3];
}

inline float polyval5_ddder(float *c, float t) {
    return (60.0f * c[0] * t + 24.0f * c[1]) * t + 6.0f * c[2];
}

/*
 * calculate the angle of splinepoint
 */
/*
TODO: calc correct and symmetric
why not computing each segment 1 to pn-2 central and the 0 and pn-1 forward and backward?
*/
inline void calc_spline_angle(Pose *p, unsigned i, unsigned pn, int dir) {
    if (isPsiDontCare(p[i].psi)) {
        if (i > 0) {
            if (dir) {
                p[i].psi = atan2f(p[i+1].y - p[i-1].y, p[i+1].x - p[i-1].x);
            } else {
                p[i].psi = atan2f(p[i-1].y - p[i+1].y, p[i-1].x - p[i+1].x); // turn angle for drive backwards
            }
        } else {
            if (dir) {
                p[i].psi = atan2f(p[i+1].y - p[i].y, p[i+1].x - p[i].x);
            } else {
                p[i].psi = atan2f(p[i].y - p[i+1].y, p[i].x - p[i+1].x);	// turn angle for drive backwards
            }
        }
    }

    if (isPsiDontCare(p[i+1].psi)) {
        if (i < pn-2){
            if (dir) {
                p[i+1].psi = atan2f(p[i+2].y - p[i].y, p[i+2].x - p[i].x);
            } else {
                p[i+1].psi = atan2f(p[i].y - p[i+2].y, p[i].x - p[i+2].x);	// turn angle for drive backwards
            }
        } else {
            if (dir) {
                p[i+1].psi = atan2f(p[i+1].y - p[i].y, p[i+1].x - p[i].x);
            } else {
                p[i+1].psi = atan2f(p[i].y - p[i+1].y, p[i].x - p[i+1].x);	// turn angle for drive backwards
            }
        }
    }
}

inline void calc_spline_param(spline_t *s) {

}

/*
 * calculate spline coefficients for polynom of 3rd degree
 *
 * (non-inline for testmenu)
 */
void calc_spline(spline_t *s, Pose *p, unsigned i, unsigned pn, signed dir) {
    s->type = CATMULL_ROM;

    turag_infof("calc_spline(type=%d, i=%d, pn=%d, dir=%d)", s->type, i, pn, dir);

    // calculate the angle of splinepoint
    calc_spline_angle(p, i, pn, dir);

    // spline form depends from distance between p[i] and p[i+1]
    float d12 = hypotf(p[i + 1].x - p[i].x, p[i + 1].y - p[i].y);
    float k = SPLINE_FORM_FACTOR * d12;

    float vx[4] = { k * cosf(p[i].psi), k * cosf(p[i + 1].psi), p[i].x,
                                    p[i + 1].x };
    float vy[4] = { k * sinf(p[i].psi), k * sinf(p[i + 1].psi), p[i].y,
                                    p[i + 1].y };

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
    int j;
    for (i = 0; i < 4; i++) {
        s->cx[i] = 0.0f;
        s->cy[i] = 0.0f;
        for (j = 0; j < 4; j++) {
            s->cx[i] += (float)m[i][j] * vx[j];
            s->cy[i] += (float)m[i][j] * vy[j];
        }
        turag_infof("calc_spline: cx[%d] = %f cy[%d] = %f vx[%d] = %f vy[%d] = %f",
                    i, s->cx[i], i, s->cy[i],
                    i, vx[i], i, vy[i]);
    }

    s->direct_dist = d12;

    turag_infof("calc_spline: direct_dist=%f", s->direct_dist);

    calc_spline_param(s);
}

inline void calc_spline_param5(spline_t *s) {

    float t, x, y, dx, dy, ddx, ddy;

    float delta_t = SPLINE_ITERATION_DISTANCE / s->direct_dist;
    float x_old = s->c5x[5];
    float y_old = s->c5y[5];

    s->kappa_max = 0.0f;
    s->length = 0.0f;
    for (t = delta_t; t <= 1.0f + delta_t; t = t + delta_t) {
        x = polyval5(s->c5x, t);
        y = polyval5(s->c5y, t);
        s->length += hypotf(x - x_old, y - y_old);
        x_old = x;
        y_old = y;

        dx = polyval5_der(s->c5x, t);
        dy = polyval5_der(s->c5y, t);
        ddx = polyval5_dder(s->c5x, t);
        ddy = polyval5_dder(s->c5y, t);
        s->kappa_max = fmaxf(s->kappa_max,
                                                fabsf((dx * ddy - ddx * dy)) / cbcf(hypotf(dx, dy)));
    }
    s->length = fmaxf(s->length, SPLINE_ITERATION_DISTANCE);
    s->kappa_max = fmaxf(s->kappa_max, 1e-10f);
    // turag_infof("    spline length: %f ", s->length);
    // turag_infof("    kappa max: %f ", s->kappa_max);
}

/*
 * calculate spline coefficients for polynom of 5th degree
 * (non-inline for testmenu)
 */
void calc_spline5(spline_t *s, Pose *p, unsigned i, unsigned pn, signed dir) {

    s->type = HERMITE_5;

    // calculate the angle of splinepoint
    calc_spline_angle(p, i, pn, dir);

    // spline form depends from distance between p[i] and p[i+1]
    float d12 = hypotf(p[i + 1].x - p[i].x, p[i + 1].y - p[i].y);
    float k = SPLINE_FORM_FACTOR * d12;

    float vx[4] = { p[i].x, k * cosf(p[i].psi), p[i + 1].x,
                                    k * cosf(p[i + 1].psi) };
    float vy[4] = { p[i].y, k * sinf(p[i].psi), p[i + 1].y,
                                    k * sinf(p[i + 1].psi) };

    int m[3][4] =  {{ -6,-3,  6,-3},
                    { 15, 8,-15, 7},
                    {-10,-6, 10,-4}};

    s->c5x[3] = 0.0f;
    s->c5y[3] = 0.0f;
    s->c5x[4] = vx[1];
    s->c5y[4] = vy[1];
    s->c5x[5] = vx[0];
    s->c5y[5] = vy[0];

    int j;
    for (i = 0; i < 3; i++) {
        s->c5x[i] = 0.0f;
        s->c5y[i] = 0.0f;
        for (j = 0; j < 4; j++) {
            s->c5x[i] += (float)m[i][j] * vx[j];
            s->c5y[i] += (float)m[i][j] * vy[j];

        }
    }
    /*
    for (i = 0; i < 6; i++) {
     turag_infof(" c5x[%d] = %f",i,s->c5x[i]);
     turag_infof(" c5y[%d] = %f",i,s->c5y[i]);
    }*/

    s->direct_dist = d12;

    calc_spline_param5(s);
}

inline void get_spline(Pose *p, float *bending, spline_t *s, float *t) {

    p->x = polyval3(s->cx, *t);
    p->y = polyval3(s->cy, *t);

    float dx = polyval3_der(s->cx, *t);
    float dy = polyval3_der(s->cy, *t);

    p->psi = atan2f(dy, dx);

    float ddx = polyval3_dder(s->cx, *t);
    float ddy = polyval3_dder(s->cy, *t);

    *bending = (dx * ddy - ddx * dy) / cbcf(hypotf(dx, dy));
}

inline void get_spline5(Pose *p, float *bending, spline_t *s, float *t) {

    p->x = polyval5(s->c5x, *t);
    p->y = polyval5(s->c5y, *t);

    float dx = polyval5_der(s->c5x, *t);
    float dy = polyval5_der(s->c5y, *t);

    p->psi = atan2f(dy, dx);

    float ddx = polyval5_dder(s->c5x, *t);
    float ddy = polyval5_dder(s->c5y, *t);

    *bending = (dx * ddy - ddx * dy) / cbcf(hypotf(dx, dy));
}
#endif

template<std::size_t order>
class Polynomial
{
private:
    inline float val3(float t, unsigned derivation) const {
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

    inline float val5(float t, unsigned derivation) const {
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

public:
    inline float val(float t, unsigned derivation=0) const {
        if (order == 3) {
            return val3(t, derivation);
        } else if (order == 5) {
            return val5(t, derivation);
        }
    }

    inline float getNode(unsigned index) const {
        return c[index];
    }

    // polynomial coefficients
    float c[order+1];
};

template<std::size_t order>
class Polynomial2D
{
public:
    // polynomial coefficients
    Polynomial<order> x, y;
};

/**
 * @brief Spline virtual base class for all spline types (Catmull-Rom, Hermite, Bezier, ...)
 */
class Spline
{
public:
    virtual bool calculate();
    virtual bool getMaxVelocity();
    virtual bool getStep();
    virtual constexpr unsigned getOrder();

protected:
    static constexpr Length spline_iteration_distance = 50*Units::mm;
};

/**
 * @brief SplineOrder Spline of a certain order
 */
template<std::size_t order>
class SplineOrder :
        public Spline
{
public:
    bool calculate() {
        return calculate_parameters();
    }

    bool getMaxVelocity() {
        return order*2;
    }

    bool getStep() {
        return order*2;
    }

    constexpr unsigned getOrder() const {
        return order;
    }

private:
    bool calculate_parameters(void) {
        static_assert(order == 3 || order == 5, "wrong spline order for this call");

        float t, x, y, dx, dy, ddx, ddy;

        float delta_t = spline_iteration_distance.to(Units::mm) / direct_dist.to(Units::mm);
        float x_old = c.x.getNode(order);
        float y_old = c.y.getNode(order);

        // max Krümmung
        kappa_max = 0.f;

        // Bogenlänge
        length = 0 * Units::mm;

        // Abtastung des Polynoms
        for (t = delta_t; t <= 1.0f + delta_t; t = t + delta_t) {
            // Wert an Stelle t
            x = c.x.val(t);
            y = c.y.val(t);

            // Gesamtlänge += Abschnittslänge
            float l_mm = length.to(Units::mm) + hypotf(x - x_old, y - y_old);
            length += l_mm * Units::mm;

            // merken der Werte für Abstandsberechnung im nächsten Iterationschritt
            x_old = x;
            y_old = y;

            // Wert der 1. Ableitung an Stelle t
            dx = c.x.val(t, 1);
            dy = c.y.val(t, 1);

            // Wert der 2. Ableitung an Stelle t
            ddx = c.x.val(t, 2);
            ddy = c.y.val(t, 2);

            // max Krümmung bestimmen
            kappa_max = fmaxf(kappa_max, fabsf((dx * ddy - ddx * dy)) / cbcf(hypotf(dx, dy)));
        }

        // Begrenzung...

        //...der Bogenlänge: min. spline_iteration_distance
        length = fmaxf(length.to(Units::mm), spline_iteration_distance.to(Units::mm)) * Units::mm;

        //...der Krümmung: min. 1e-10f
        kappa_max = fmaxf(kappa_max, 1e-10f);

#if 0
        turag_infof("calc_spline_param: spline length: %f", length);
        turag_infof("calc_spline_param: kappa max: %f", kappa_max);
#endif

        return true;
    }


    static constexpr std::size_t Order = order;

    Polynomial2D<order> c;
    Length direct_dist;
    Length length;
    float kappa_max;
};

using Spline5 = SplineOrder<5>;
using Spline3 = SplineOrder<3>;

} // namespace TURAG

#endif // SPLINE_H
