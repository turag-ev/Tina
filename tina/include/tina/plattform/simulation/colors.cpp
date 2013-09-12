#include "colors.h"

namespace Colors {

static
QColor hexcolor(unsigned hexcode) {
  return QColor((hexcode >> (2 * 8)) & 0xFF,
                (hexcode >> (1 * 8)) & 0xFF,
                (hexcode >> (0 * 8)) & 0xFF);
}

const QColor yellow(hexcolor(0xF0CA00));
const QColor pink(hexcolor(0xE1A6AD));
const QColor black(hexcolor(0x0A0A0D));
const QColor blue(hexcolor(0x0E518D));
const QColor red(hexcolor(0xA02128));
const QColor white(hexcolor(0xF7FBF5));

const QColor as_colors[] = {
  hexcolor(0x0000FF),
  hexcolor(0x0000CC),
  hexcolor(0xFF8800),
  hexcolor(0x000000)
};

} // namespace Colors
