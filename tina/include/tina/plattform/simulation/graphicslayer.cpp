#include "graphicslayer.h"
#include "interfaces/transform.h"

GraphicsLayer::GraphicsLayer(const char* name, PaintFunc func, bool default_on) :
  func_(func), name_(name)
{
  if (!default_on) {
    setVisible(false);
  }
}

QRectF GraphicsLayer::boundingRect() const
{
  static const qreal adjust = 2.;
  return SimRectMM(-1500. -adjust,     0. -adjust,
                    3000. +adjust,  2000. +adjust);
}

void GraphicsLayer::paint(QPainter *painter,
                          const QStyleOptionGraphicsItem *item,
                          QWidget *)
{
  func_(painter, item);
}
