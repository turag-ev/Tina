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

	enum : std::size_t { Order = order };

	// value & derivation

	template<std::size_t derivation = 0>
	inline float val(float t) const { return 0.f; }

	// item access

	constexpr float getNode(unsigned index) const {
        return c[index];
    }

	constexpr float& operator[](unsigned index) const {
		return c[index];
	}

	float& operator[](unsigned index) {
		return c[index];
	}

	// range

	using iterator = float*;
	using const_iterator = const float*;

	std::size_t size() const { return order + 1; }

	iterator begin() { return c; }
	const_iterator begin() const { return c; }
	iterator end() { return c + order + 1; }
	const_iterator end() const { return c + order + 1; }

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
	return 120.0f * c[0] * t + 24.0f * c[1];
}
template<> template<>
inline float Polynomial<5>::val<5>(float) const {
	return 120.0f * c[0];
}

/**
 * @brief 2-dimensional polynomial
 */
template<std::size_t order>
class Polynomial2D
{
public:
	enum : std::size_t { Order = order };

    // polynomial coefficients
    Polynomial<order> x, y;
};

/**
 * @brief Base class for splines
 * @details intern class for order independend code
 */
class SplineBase
{
protected:
    static constexpr Length spline_iteration_distance = 50 * Units::mm;
    static constexpr float spline_form_factor = 1.0f;
	static constexpr Angle mc_no_angle = 4.f * Units::rad;

	static inline bool isAngleDontCare(Units::Angle a)
	{
		return (abs(a - mc_no_angle) < 0.001f * Units::rad) || isnan(a);
	}

	/**
	 * @brief calc_spline_angle calculate missing angle of spline point
	 */
	static void calc_spline_angle(Pose *p, unsigned i, unsigned pn, int dir);

	Length direct_dist;
	Length length;
	float kappa_max = 0.f;
	int drive_direction = 0;
};

/**
 * @brief SplineOrder Spline of a certain order
 */
template<std::size_t order>
class SplineOrder : private SplineBase
{
	static_assert(order == 3 || order == 5, "only order 3 and 5 implemented");
public:

	enum : std::size_t { Order = order };

	bool calculate(Pose *poses, unsigned pose_index, unsigned pose_count, int direction);

	constexpr Length getLength() const     { return length; }
	constexpr float getKappaMax() const    { return kappa_max; }
	constexpr Length getDirectDist() const { return direct_dist; }
	constexpr unsigned getOrder() const    { return order; }

	Pose getPoseStep(float t);
	float getBendingStep(float t);

private:

	void calc_spline(Pose *poses, unsigned pose_index);
	bool calculate_parameters();

    Polynomial2D<order> c;
};

using Spline5 = SplineOrder<5>;
using Spline3 = SplineOrder<3>;

} // namespace TURAG

#endif // SPLINE_H
