#include "spline.h"

namespace TURAG {

// Spline

constexpr Length SplineBase::spline_iteration_distance;
constexpr float SplineBase::spline_form_factor;
constexpr Angle SplineBase::mc_no_angle;

void SplineBase::calc_spline_angle(Pose *p, unsigned i, unsigned pn, int dir)
{
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

// SplineOrder<X>

template<std::size_t order>
bool SplineOrder<order>::calculate(Pose *poses, unsigned pose_index,
								   unsigned pose_count, int direction)
{
	drive_direction = direction;

	calc_spline_angle(poses, pose_index, pose_count, direction);

	calc_spline(poses, pose_index);

	return calculate_parameters();
}

template<std::size_t order>
Pose SplineOrder<order>::getPoseStep(float t)
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

template<std::size_t order>
float SplineOrder<order>::getBendingStep(float t)
{
	// Wert der 1. Ableitung an Stelle t
	float dx = c.x.template val<1>(t);
	float dy = c.y.template val<1>(t);

	// Wert der 2. Ableitung an Stelle t
	float ddx = c.x.template val<2>(t);
	float ddy = c.y.template val<2>(t);

	float bending = (dx * ddy - ddx * dy) / cbcf(hypotf(dx, dy));
	return bending;
}

// calc spline catmullrom
template<>
void SplineOrder<3>::calc_spline(Pose *p, unsigned pi)
{
	// see getPoseStep()
	Angle aoff = 0 * Units::deg;
	if (drive_direction == -1) {
		aoff = 180 * Units::deg;
	}

	// spline form depends from distance between p[i] and p[i+1]
	float d12 = distance(p[pi], p[pi+1]).to(Units::mm);
	float k = spline_form_factor * d12;

	float vx[4] = { k * cos(p[pi].phi + aoff),
					k * cos(p[pi + 1].phi + aoff),
					p[pi].x.to(Units::mm),
					p[pi + 1].x.to(Units::mm) };
	float vy[4] = { k * sin(p[pi].phi + aoff),
					k * sin(p[pi + 1].phi + aoff),
					p[pi].y.to(Units::mm),
					p[pi + 1].y.to(Units::mm) };

	/* invertierte hermitesche kubische Matrix (siehe Wikipedia)
	 *
	 * errechnet Splineparameter anhand gegebener Randwerte:
	 * - Position p
	 * - Tangente bzw. Winkel m
	 *
	 *  v = [m0 m1 p0 p1]' siehe oben
	 */
	static constexpr float m[4][4] =
				   {{ 1, 1, 2,-2},
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
			c.x.c[i] += m[i][j] * vx[j];
			c.y.c[i] += m[i][j] * vy[j];
		}
	}

	direct_dist = d12 * Units::mm;
}

// calc spline hermite
template<>
void SplineOrder<5>::calc_spline(Pose *p, unsigned pi)
{
	// see getPoseStep()
	Angle aoff = 0 * Units::deg;
	if (drive_direction == -1) {
		aoff = 180 * Units::deg;
	}

	// spline form depends from distance between p[i] and p[i+1]
	float d12 = distance(p[pi], p[pi+1]).to(Units::mm);
	float k = spline_form_factor * d12;

	float vx[4] = { p[pi].x.to(Units::mm),
					k * cos(p[pi].phi + aoff),
					p[pi + 1].x.to(Units::mm),
					k * cos(p[pi + 1].phi + aoff) };
	float vy[4] = { p[pi].y.to(Units::mm),
					k * sin(p[pi].phi + aoff),
					p[pi + 1].y.to(Units::mm),
					k * sin(p[pi + 1].phi + aoff) };

	static constexpr float m[3][4] =
				   {{ -6,-3,  6,-3},
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
			c.x.c[i] += m[i][j] * vx[j];
			c.y.c[i] += m[i][j] * vy[j];

		}
	}

	direct_dist = d12 * Units::mm;
}

template<std::size_t order>
bool SplineOrder<order>::calculate_parameters()
{
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


// explicit instantiation of SplineOrder<X> class
template class SplineOrder<3>;
template class SplineOrder<5>;


} // namespace TURAG
