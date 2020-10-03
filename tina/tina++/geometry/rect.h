#ifndef TINAPP_GEOMETRY_RECT_H
#define TINAPP_GEOMETRY_RECT_H

#include "geometry.h"

namespace TURAG {

/// \brief Rechteck
///
class Rect {
public:
	/// leeres Rechteck erstellen
	constexpr Rect() :
		lb_(), rt_()
	{ }

	/// \brief Rechteck aus zwei Punkten erstellen
	/// \param a Punkt 1
	/// \param b Punkt 2
	constexpr Rect(Point a, Point b) :
		lb_(min(a.x, b.x), min(a.y, b.y)),
		rt_(max(a.x, b.x), max(a.y, b.y))
	{ }

	/// \brief Rechteck aus Ursprungspunkt und Größe erstellen
	/// \param o Ursprungspunkt
	/// \param width Breite
	/// \param height Höhe
	constexpr Rect(Point o, Length width, Length height) :
		Rect(o, Point(o.x + width, o.y + height))
	{ }

	/// \brief Mittelpunkt von Rechteck bestimmen
	constexpr Point getCenter() const {
		return Point((lb_.x + rt_.x)/2, (lb_.y + rt_.y)/2);
	}

	constexpr Length getLeft() const { return lb_.x; }
	constexpr Length getBottom() const { return lb_.y; }
	constexpr Length getRight() const { return rt_.x; }
	constexpr Length getTop() const { return rt_.y; }

	constexpr Point getLeftBottom() const { return lb_; }
	constexpr Point getRightTop() const { return rt_; }
	constexpr Point getLeftTop() const { return Point(getLeft(), getTop()); }
	constexpr Point getRightBottom() const { return Point(getRight(), getBottom()); }

	constexpr Length getWidth() const { return rt_.x - lb_.x; }
	constexpr Length getHeight() const { return rt_.y - lb_.y; }
	constexpr Vector<Length> getSize() const { return Vector<Length>(getWidth(), getHeight()); }

	void inflate(Length dx, Length dy) {
		lb_.x -= dx/2;
		lb_.y -= dy/2;
		rt_.x += dx/2;
		rt_.y += dy/2;
	}

	void inflate(Length left, Length bottom, Length right, Length top) {
		lb_.x -= left;
		lb_.y -= bottom;
		rt_.x += right;
		rt_.y += top;
	}

private:
	/// Punkt links unten (kleinster x und y Wert)
	Point lb_;

	/// Punkt rechts oben (größter x und y Wert)
	Point rt_;
};

///
/// \brief Prüft ob Punkt in Rechteck liegt.
/// \relates Rechteck
///
/// Liegt der Punkt auf dem Rand des Rechtecks, so wird auch \a true zurück gegeben.
/// \param rect Rechteck
/// \param point Punkt
/// \return \a true, wenn Punkt in Rechteck liegt, sonst \a false
///
bool located_in(const Rect& rect, const Point& point);

/// \brief Prüft ob ein Rechteck sich in der Nähe von Punkt befindet
/// \relates Rechteck
/// Berührt der Kreis aus Punkt und Radius \a r den Rand des Rechtecks, so wird auch \a true zurück gegeben.
/// \param pos Position
/// \param rect Rechteck
/// \param r Radius des Kreises, der den maximal zulässigen Abstand zum Rechteck angibt, für den noch true zurück
/// gegeben wird.
/// \return \a true, wenn Kreis aus Punkt \a pos und Radius \a r das Rechteck \a rect schneidet, sonst \a false
bool in_range(const Point& pos, const Rect& rect, Length r);

/// \brief Prüft ob ein Rechteck sich in der Nähe von Punkt befindet
/// \relates Rechteck
/// Berührt der Kreis aus Punkt \a pos und Radius \a r den Rand des Rechtecks, so wird auch \a true zurück gegeben.
/// \param pos Position
/// \param rect Rechteck
/// \param r Radius des Kreises, der den maximal zulässigen Abstand zum Rechteck angibt, für den noch true zurück
/// gegeben wird.
/// \return \a true, wenn Kreis aus Punkt \a pos und Radius \a r das Rechteck \a rect schneidet, sonst \a false
inline bool in_range(const Pose& pos, const Rect& rect, Length r) {
	return in_range(pos.toPoint(), rect, r);
}

} // namespace TURAG

#endif // TINAPP_GEOMETRY_RECT_H
