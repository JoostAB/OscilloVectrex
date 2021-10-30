#include "OscilloVectrex.h"

Coord::Coord(coordElement x, coordElement y) {
  this->x = x;
  this->y = y;
}

bool Coord::equals(Coord* other) {
  return (other && (other->x == this->x) && (other->y == this->y));
}

void Coord::setCoord(coordElement x, coordElement y) {
  this->x = x;
  this->y = y;
}

OscilloVectrex::OscilloVectrex() {
}

OscilloVectrex::~OscilloVectrex() {
  dac_output_disable(X_CHANNEL);
  dac_output_disable(Y_CHANNEL);
}

Coord* OscilloVectrex::currentPoint() {
  return this->_currentPoint;
}

void OscilloVectrex::begin() {
  dac_output_enable(X_CHANNEL);
  dac_output_enable(Y_CHANNEL);
}

void OscilloVectrex::moveTo(Coord* p) {
  if (p->equals(this->_currentPoint)) return;

  _setChannelValue(X_CHANNEL, p->x);
  _setChannelValue(Y_CHANNEL, p->y);

  this->_currentPoint = p;
}

void OscilloVectrex::moveTo(coordElement x, coordElement y) {
    if (x == _currentPoint->x && y == _currentPoint->y) return

  _setChannelValue(X_CHANNEL, x);
  _setChannelValue(Y_CHANNEL, y);

  this->_currentPoint->setCoord(x,y);
}

void OscilloVectrex::setX(coordElement x) {
  if (x == _currentPoint->x) return;
  _setChannelValue(X_CHANNEL,x);
}

void OscilloVectrex::setY(coordElement y) {
  if (y == _currentPoint->y) return;
  _setChannelValue(Y_CHANNEL,y);
}

void OscilloVectrex::line(Coord* pStart, Coord* pEnd) {
  line(pStart->x, pStart->y, pEnd->x, pEnd->y);
}

void OscilloVectrex::line(coordElement xStart, coordElement yStart, coordElement xEnd, coordElement yEnd) {
  if ((xStart == xEnd) && (yStart == yEnd)) return;

  if (yStart == yEnd) this->hLine(xStart, yStart, xEnd);

  if (yStart == xEnd) this->vLine(xStart, yStart, yEnd);

  uint8_t steep = abs(yEnd - yStart) > abs(xEnd - xStart);

  if (steep) {
    jbswap(xStart, yStart);
    jbswap(xEnd, yEnd);
  }

  coordElement dx = xEnd - xStart;
  coordElement dy = abs(yEnd - yStart);
  coordElement err = dx / 2;
  
  int8_t xstep = 1;
  int8_t ystep = 1;

  if (xStart > xEnd)  xstep = -1;
    if (yStart > yEnd) ystep = -1;
  
  moveTo(xStart, yStart);

  while (xStart != xEnd) {
    xStart = xStart + xstep;
    if (steep) {
      moveTo(yStart, xStart);
    } else {
      moveTo(xStart, yStart);
    }

    err -= dy;
    if (err < 0) {
      yStart += ystep;
      err += dx;
    }
    
  }
}


/**
 * @brief Draw line to pEnd
 */
void OscilloVectrex::lineTo(Coord* pEnd) {
  lineTo(pEnd->x, pEnd->y);
}

void OscilloVectrex::lineTo(coordElement xEnd, coordElement yEnd) {
  line(_currentPoint->x, _currentPoint->y, xEnd, yEnd);
}

void OscilloVectrex::vLine(Coord* pStart, Coord* pEnd) { _vLine(pStart, pEnd->y ); }
void OscilloVectrex::vLine(coordElement xStart, coordElement yStart, coordElement yEnd) { 
  moveTo(xStart, yStart);
  vLineTo(yEnd);
}
void OscilloVectrex::vLineTo(Coord* pEnd) { _vLine(_currentPoint, pEnd->y); }
void OscilloVectrex::vLineTo(coordElement yEnd) { _vLine(_currentPoint, yEnd); }

void OscilloVectrex::_vLine(Coord* pStart, coordElement yEnd) {
  int8_t step = 1;
  if (pStart->y > yEnd) step = -1;
  
  moveTo(pStart);
  while (_currentPoint->y != yEnd) {
    this->_setChannelValue(Y_CHANNEL, _currentPoint->y + step);
  }
}

void OscilloVectrex::hLine(Coord* pStart, Coord* pEnd) { _hLine(pStart, pEnd->x ); }
void OscilloVectrex::hLine(coordElement xStart, coordElement yStart, coordElement xEnd) {
  moveTo(xStart, yStart);
  hLineTo(xEnd);
}
void OscilloVectrex::hLineTo(Coord* pEnd) { _hLine(_currentPoint, pEnd->x); }
void OscilloVectrex::hLineTo(coordElement xEnd) { _hLine(_currentPoint, xEnd); }

void OscilloVectrex::_hLine(Coord* pStart, coordElement xEnd) {
  int8_t step = 1;
  if (pStart->x > xEnd) step = -1;
  
  moveTo(pStart);
  while (_currentPoint->x != xEnd) {
    this->_setChannelValue(X_CHANNEL, _currentPoint->x + step);
  }
}

void OscilloVectrex::rect(Coord* p1, Coord* p2) { rect(p1->x, p1->y, p2->x, p2->y); }
void OscilloVectrex::rect (coordElement p1x, coordElement p1y, coordElement p2x, coordElement p2y) {
  moveTo(p1x, p1y);
  lineTo(p2x, p1y);
  lineTo(p2x,p2y);
  lineTo(p1x,p2y);
  lineTo(p1x, p1y);
}

void OscilloVectrex::arc(Coord* pStart, Coord* pEnd, uint8_t  angle) { 
  arc(pStart->x, pStart->y, pEnd->x, pEnd->y, angle); 
}
void OscilloVectrex::arc(coordElement xStart, coordElement yStart, coordElement xEnd, coordElement yEnd, uint8_t  angle) {
  coordElement dX = xEnd - xStart;
  coordElement dY = yEnd - yStart;
  
  coordElement steps = max(dX, dY);
  
  moveTo(xStart, yStart);

  

}

void OscilloVectrex::_setChannelValue(dac_channel_t channel, coordElement val) {
  // Add 127 to value, because val is -127 to 128, and voltage is 0 to 255
  dac_output_voltage(channel, val + 127);
}