/* Contains the class definition that goes with the program
 * rough_draft_program.ino
 *
 * The program using this class can be scaled for use with multiple beams.
 * Just create as many instances of this class as there are beams and it
 * will (hopefully) work without an issue. */

#ifndef BEAM_H
#define BEAM_H

class
Beam
{
  public:
    volatile uint8_t note = 0;
    volatile uint16_t freq = -1;
    volatile uint16_t freqReads[4];
};

#endif
