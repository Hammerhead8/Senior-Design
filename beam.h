/* Contains the class definition that goes with the program
 * program.ino
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
    volatile int8_t note = 0; /* Flag used for determining if the beam is active */
                              /* The program seems to work better when this is signed instead of unsigned */
    volatile int16_t freq = -1; /* The value read from the ADC which determines which note to play */
                                /* It is set to -1 when the beam is inactive */
    int16_t freqReads[4] = {0}; /* Holds the values of four consecutive reads from the ADC.
                          /* These values are then averaged and stored in FREQ */

    volatile int8_t difference = -1; /* Used as a workaround to create atomic
				      * conditionals in the LOOP function.
				      * Since the ATMega2560 is an 8 bit MCU
				      * problems can arise when comparing 16 bit values
				      * with interrupts enabled. Using an 8 bit value
				      * eliminates this risk. */
};

#endif
