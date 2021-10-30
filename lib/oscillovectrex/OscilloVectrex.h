#ifndef _OSCILLO_VECTREX_H
#define _OSCILLO_VECTREX_H

#include <Arduino.h>
#include <driver/dac.h>

// DAC channel 1 is attached to GPIO25, DAC channel 2 is attached to GPIO26
// Therefore the x-probe should be connected to pin 25, and the y-probe to pin 26
#define X_CHANNEL DAC_CHANNEL_1
#define Y_CHANNEL DAC_CHANNEL_2

#define SINESTEPS 256

typedef int8_t coordElement;

/**
 * One full sinewave. Max 255, min 0.
 * Generated using https://www.daycounter.com/Calculators/Sine-Generator-Calculator.phtml
 */
const static byte SineTable[SINESTEPS] {
  /* First quarter /‾ 128 - 255 */
  0x80, 0x83, 0x86, 0x89, 0x8c, 0x8f, 0x92, 0x95, 0x98, 0x9c, 0x9f, 0xa2, 0xa5, 0xa8, 0xab, 0xae,
  0xb0, 0xb3, 0xb6, 0xb9, 0xbc, 0xbf, 0xc1, 0xc4, 0xc7, 0xc9, 0xcc, 0xce, 0xd1, 0xd3, 0xd5, 0xd8,
  0xda, 0xdc, 0xde, 0xe0, 0xe2, 0xe4, 0xe6, 0xe8, 0xea, 0xeb, 0xed, 0xef, 0xf0, 0xf2, 0xf3, 0xf4,
  0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfb, 0xfc, 0xfd, 0xfd, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff,

  /* Second quarter ‾\ 255 - 129 */
  0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfd, 0xfd, 0xfc, 0xfc, 0xfb, 0xfa, 0xf9, 0xf8, 0xf7, 0xf6,
  0xf5, 0xf4, 0xf2, 0xf1, 0xef, 0xee, 0xec, 0xeb, 0xe9, 0xe7, 0xe5, 0xe3, 0xe1, 0xdf, 0xdd, 0xdb,
  0xd9, 0xd7, 0xd4, 0xd2, 0xcf, 0xcd, 0xca, 0xc8, 0xc5, 0xc3, 0xc0, 0xbd, 0xba, 0xb8, 0xb5, 0xb2,
  0xaf, 0xac, 0xa9, 0xa6, 0xa3, 0xa0, 0x9d, 0x9a, 0x97, 0x94, 0x91, 0x8e, 0x8a, 0x87, 0x84, 0x81,

  /* Third quarter \_ 126 - 0 */
  0x7e, 0x7b, 0x78, 0x75, 0x71, 0x6e, 0x6b, 0x68, 0x65, 0x62, 0x5f, 0x5c, 0x59, 0x56, 0x53, 0x50,
  0x4d, 0x4a, 0x47, 0x45, 0x42, 0x3f, 0x3c, 0x3a, 0x37, 0x35, 0x32, 0x30, 0x2d, 0x2b, 0x28, 0x26,
  0x24, 0x22, 0x20, 0x1e, 0x1c, 0x1a, 0x18, 0x16, 0x14, 0x13, 0x11, 0x10, 0x0e, 0x0d, 0x0b, 0x0a,
  0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x03, 0x02, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00,

  /* Fourth quarter _/ 0 - 128 */
  0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x02, 0x02, 0x03, 0x04, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
  0x0b, 0x0c, 0x0d, 0x0f, 0x10, 0x12, 0x14, 0x15, 0x17, 0x19, 0x1b, 0x1d, 0x1f, 0x21, 0x23, 0x25,
  0x27, 0x2a, 0x2c, 0x2e, 0x31, 0x33, 0x36, 0x38, 0x3b, 0x3e, 0x40, 0x43, 0x46, 0x49, 0x4c, 0x4f,
  0x51, 0x54, 0x57, 0x5a, 0x5d, 0x60, 0x63, 0x67, 0x6a, 0x6d, 0x70, 0x73, 0x76, 0x79, 0x7c, 0x80
};

/**
 * @brief Swap both values
 */
#define jbswap(a, b) { a = a ^ b; b = a ^ b; a = a ^ b; }

/**
 * @brief Simple structure to hold a point on the screen
 * 
 * Center of screen is coord(0,0). 
 * Boytom-left is (-127,-127)
 * Top-right is (128,128)
 * 
 *     -127           0           128
 *  128  +------------+------------+ 128
 *       |            |            |
 *       |            |            |
 *       |            | (0,0)      |
 *    0  +------------+------------+ 0
 *       |            |            |
 *       |            |            |
 *       |            |            |
 *  -127 +------------+------------+ -127
 *     -127           0           128
 */
class Coord {
  public:
    /**
     * @brief Constructor
     * 
     * @param x coordinate
     * @param y coordinate
     */
    Coord(coordElement x, coordElement y);

    /**
     * @brief Constructor
     * 
     * @note Initializes to 0,0
     */
    Coord() : Coord(0,0) {};
  
  /**
   * @brief Set new coordinates
   * 
   * @param x coordinate
   * @param y coordinate
   */
  void setCoord(coordElement x, coordElement y);

  /**
   * @brief The coordinates
   */
  coordElement x, y;

  /**
   * @brief Compare values of two Coord instances
   * 
   * @param Pointer to other Coord
   * 
   * @return boolean
   * 
   * @note Returns only true if both have same x AND y coordinates. If other instance
   *       is not valid (NULL) false is returned
   */
  bool equals(Coord* other);
};

class  OscilloVectrex {
  public:
    OscilloVectrex();

    /**
     * @brief Shut down dacs
     */
    ~OscilloVectrex();

    void

      /**
       * @brief Start up dacs
       */
      begin(),

      /**
       * @brief: Move the beam to a new point on the screen.
       * 
       * @param New point as coord
       * 
       * @note Move is instantly, no line is being drawn (well, very slightly) between the points.
       *       If the connection is to be bright, draw all points in between. See line method.
       */
      moveTo(Coord* p),

      /**
       * @brief: Move the beam to a new point on the screen.
       * 
       * @param x coordinate
       * @param y coordinate
       * 
       * @note Move is instantly, no line is being drawn (well, very slightly) between the points.
       *       If the connection is to be bright, draw all points in between. See line method.
       */
      moveTo(coordElement x, coordElement y),

      /**
       * @brief Set only new x coordinate for beam
       * 
       * @param The new X
       * 
       * @note see setPoint
       */
      setX(coordElement x),

      /**
       * @brief Set only new y coordinate for beam
       * 
       * @param The new Y
       * 
       * @note see setPoint
       */
      setY(coordElement y),

      /**
       * @brief Draw a line from pStart to pEnd
       * 
       * Algorithm based on Bresenham's line algorithm
       *       (https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm)
       * 
       * @param The start coordinate
       * @param The end coordinate
       * 
       * @note If appropriate, it will call the (much faster) vLine or hLine methods
       */
      line(Coord* pStart, Coord* pEnd),
      line(coordElement xStart, coordElement yStart, coordElement xEnd, coordElement yEnd),

      /**
       * @brief Draw a line from current position to pEnd
       * 
       * Actually simply calls line method with current position as pStart
       * 
       * @param End coordinate
       */
      lineTo(Coord* pEnd),
      lineTo(coordElement xEnd, coordElement yEnd),

      /**
       * @brief Draw vertical line from pStart to pEnd
       * 
       * @param The start coordinate
       * @param The end coordinate
       * 
       * @note pEnd.x is ignored. pStart x is used for complete line
       */
      vLine(Coord* pStart, Coord* pEnd),
      vLine(coordElement xStart, coordElement yStart, coordElement yEnd),

      /**
       * @brief Draw vertical line from current position to pEnd
       * 
       * @param The end coordinate
       */
      vLineTo(Coord* pEnd),

      /**
       * @brief Draw vertical line from current position to yEnd
       * 
       * @param The end y coordinate
       */
      vLineTo(coordElement yEnd),
      

      /**
       * @brief Draw horizontal line from pStart to pEnd
       * 
       * @param The start coordinate
       * @param The end coordinate
       * 
       * @note pEnd.y is ignored. pStart y is used for complete line
       */
      hLine(Coord* pStart, Coord* pEnd),
      hLine(coordElement xStart, coordElement yStart, coordElement xEnd),

      /**
       * @brief Draw horizontal line from current position to pEnd
       * 
       * @param The end coordinate
       */
      hLineTo(Coord* pEnd),
      
      /**
       * @brief Draw horizontal line from current position to xEnd
       * 
       * @param The end x-coordinate
       */
      hLineTo(coordElement xEnd),

      rect(Coord* p1, Coord* p2),
      rect (coordElement p1x, coordElement p1y, coordElement p2x, coordElement p2y),

      /**
       * @brief Draw an arc from pStart to pEnd
       */
      arc(Coord* pStart, Coord* pEnd, uint8_t angle),
      arc(coordElement xStart, coordElement yStart, coordElement xEnd, coordElement yEnd, uint8_t angle);

    Coord* currentPoint();

  private:
    Coord* _currentPoint;
    void 
      _setChannelValue(dac_channel_t channel, coordElement val),
      _vLine(Coord* pStart, coordElement yEnd),
      _hLine(Coord* pStart, coordElement xEnd);
};

#endif /* _OSCILLO_VECTREX_H */