#ifndef GRAPHICSLAYER_H
#define GRAPHICSLAYER_H

#include <QGraphicsItem>
#include <QString>

#include <interfaces/robot-interface.h>

class GraphicsLayer : public QGraphicsItem, public LayerInterface {
public:
  typedef void (*PaintFunc)(QPainter *painter, const QStyleOptionGraphicsItem *option);

  GraphicsLayer(const char* name, PaintFunc func, bool default_on = true);
  ~GraphicsLayer() { }
  
  virtual QRectF boundingRect() const;
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget);

  QString getName() { return name_; }
  QGraphicsItem* getLayer() { return this; }

private:
  PaintFunc func_;
  QString name_;
};

#endif // GRAPHICSLAYER_H
