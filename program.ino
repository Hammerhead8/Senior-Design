/* Test program using interrupts to trigger sounds for six beams
 * If two beams are broken at the same time, then the one broken first
 * will be played */

/* The formula for the value for OCR1A is as follows:
 * compare match register = [ 16,000,000Hz/ (prescaler * desired interrupt frequency) ] - 1
 *
 * The formula for the interrupt frequency is as follows:
 * interrupt frequency (Hz) = (Arduino clock speed 16,000,000Hz)
 *             / (prescaler * (compare match register + 1))
 *
 * Since we want an interrupt every 80 milliseconds, the desired interrupt
 * frequency is 12.5 Hz
 *
 * Using a prescale value of f_clk / 64, this gives a OCRA1 value of
 * [16000000 / (64 * 12.5)) - 1 = 19999 */

/* Writing 0 to bits 0, 1, and 2 of TCCR3B disables the counter just FYI */
//#include <avr/sleep.h> /* For sleep_enable */
#include "beam.h" /* For Beam class */
#include "notes.h" /* For note frequency definitions */

/* Create an instance of the class Beam for each beam being used called beamx */
Beam beam1;
/* Uncomment these as more beams are added */
//Beam beam2;
//Beam beam3;
//Beam beam4;
//Beam beam5;
//Beam beam6;

/* Configure the input and output pins */
/* Also configure the timer and enable interrupts */
void
setup ()
{
  /* DDRx sets the direction of the pin as either input (0) or output (1) */
  DDRG = 0x20; /* Configure digital pin 4 as an output */
  PORTG &= 0x00; /* Start with pin 4 set LOW */
  DDRE &= ~(0x10); /* Configure pin 2 as an input */
  DDRF &= 0x00;/* Configure the analog pins as inputs */
  
  /* The Power Reduction Registers (PRR0 and PRR1) can be used to turn
   * off things that aren't needed to save power */
  PRR0 = 0xa6;
  PRR1 = 0x3f;

  /* The Sleep Mode Control Register (SMCR) is used to select the sleep
   * mode for the MCU. writing 0x00 to it selects idle mode without
   * also setting the sleep enable bit. This will be done later.
   * Using any mode other than idle mode only allows for using level
   * interrupts (HIGH or LOW) in attachInterrupt. Since we need an
   * interrupt whenever the voltage changes on all of the beams,
   * we have to use idle mode. */
//  SMCR = 0x00;

  /* Configure timer 1 to interrupt every 100 milliseconds when reading from the ADC */
  /* WGM10 = 0, WGM11 = 0, WGM12 = 1, WGM13 = 0 */
  /* Whenever a timer interrupt occurs read from the ADC
   * This might help reduce the noise currently being heard */
  /* Remember:  100 ms is the maximum delay before a UI starts
   * to look laggy. */
  TCCR1A &= 0x00; /* Clear Timer/Counter 1 Control Register A */
  TCCR1B &= 0x00; /* Clear Timer/Counter 1 Control Register B */
  OCR1A = 24999; /* (16000000 / (12.5 * 64) - 1 = 24999 for an 100 ms delay */
//  TIMSK1 |= (1 << OCIE1A); /* Enable Timer 1 compare interrupts */
  TIMSK1 = 0x02; /* Enable Timer 1 compare interrupts */

  /* Enable external interrupts on pins 2, 3, 18, 19, 20, and 21
   * The interrupt service routines will toggle a flag in order to
   * play the note in the loop() function
   * We will use CHANGE for the condition since we need to toggle
   * the flag whenever the photoresistor is covered or uncovered */
  attachInterrupt (digitalPinToInterrupt (2), playBeam1, CHANGE);
//  attachInterrupt (digitalPinToInterrupt (3), playBeam2, CHANGE);
//  attachInterrupt (digitalPinToInterrupt (18), playBeam3, CHANGE);
//  attachInterrupt (digitalPinToInterrupt (19), playBeam4, CHANGE);
//  attachInterrupt (digitalPinToInterrupt (20), playBeam5, CHANGE);
//  attachInterrupt (digitalPinToInterrupt (21), playBeam6, CHANGE);

}

/* Main loop */
/* The analogRead() function returns a number between 0 and 1023. */
/* This number will be used to determine which note to play. */
void
loop ()
{
  /* Check if the first beam is broken */
  /* If it is then the second octave will be played */
  
//  SMCR |= 0x01; /* Set the Sleep Enable (SE) bit in SMCR */
//  sleep_cpu (); /* Put the MCU to sleep */
  
  /* Right now the first note takes up almost half of the available range when tested in the dark 
   * and the others each have a range of about an inch. */
  if (beam1.note != 0) {
    while (beam1.note != 0) {
      beam1.freq = analogRead (A0);
      if (beam1.freq <= 2) {
        tone (4, TONE1);
        while (beam1.freq <= 2) {}
        noTone (4);
      } /* End if */
      else if (beam1.freq >= 3 && beam1.freq <= 60) {
        tone (4, TONE2);
        while (beam1.freq >= 3 && beam1.freq <= 60) {}
        noTone (4);
      } /* End else if */
      else if (beam1.freq >= 61 && beam1.freq <= 120) {
        tone (4, TONE3);
        while (beam1.freq >= 61 && beam1.freq <= 120) {}
        noTone (4);
      } /* End else if */
      else if (beam1.freq >= 121 && beam1.freq <= 280) {
        tone (4, TONE4);
        while (beam1.freq >= 121 && beam1.freq <= 280) {}
        noTone (4);
      } /* End else if */
      else if (beam1.freq >= 281 && beam1.freq <= 460) {
        tone (4, TONE5);
        while (beam1.freq >= 281 && beam1.freq <= 460) {}
        noTone (4);
      } /* End else if */
      else if (beam1.freq >= 461 && beam1.freq <= 750) {
        tone (4, TONE6);
        while (beam1.freq >= 461 && beam1.freq <= 750) {}
        noTone (4);
      } /* End else if */
      else if (beam1.freq >= 751) {
        tone (4, TONE7);
        while (beam1.freq >= 751) {}
        noTone (4);
      } /* End else if */
      else if (beam1.freq <= 0) {
        noTone (4);
        while (beam1.freq <= 0) {}
      } /* End else if */
    } /* End while */
  } /* End if */
  
  /* Use an else statement to check if more than one beam is broken.
   * Also add something to the other conditionals to check that only one beam is broken. */

} /* End loop function */

/* ISR for the first beam
 * If BEAM1.NOTE is 1 then the beam is broken and the timer can be started.
 * If BEAM1.NOTE is 0 then the beam is no longer broken and the timer can be stopped.
 * Also, we need to kill any note that is being played and make the value in
 * BEAM1.FREQ useless by setting it to -1. */
void
playBeam1 ()
{
//  SMCR &= ~(0x01); /* Clear the Sleep Enable bit */
  beam1.note ^= 1;
  if (beam1.note == 1) {
    TCCR1B |= 0x0b; /* Enable Timer 1 */
    TCNT1 = 0; /* Start Timer 1 from 0 */
  }
  else {
    noTone (4); /* Stop the note from playing */
    beam1.freq = -1; /* Set FREQ to -1 to make its value useless */
//    PORTG &= 0x00; /* Set digital pin 4 low to prevent any shit from happening */
//    PORTE &= 0x00; /* Set digital pin 2 low to prevent any shit from happening */
    TCCR1B &= 0x00; /* Disable the timer */
  } /* End if */
} /* End function */

/* Add additional ISRs for the other beams as they are implemented */

/* Timer1 interrupt vector */
/* When an interrupt occurs we will take three readings from the ADC.
 * These readings will then be averaged to try to increase accuracy of detection. */
ISR (TIMER1_COMPA_vect)
{
  if (beam1.note != 0) {
    beam1.freqReads[0] = analogRead (A0);
    beam1.freqReads[1] = analogRead (A0);
    beam1.freqReads[2] = analogRead (A0);
    beam1.freqReads[3] = analogRead (A0);
    beam1.freq = (beam1.freqReads[0] + beam1.freqReads[1] + beam1.freqReads[2] + beam1.freqReads[3]) / 4;
//    beam1.freq = analogRead (A0);
  } /* End if */
/*  Uncomment these as more beams are added */
/*  else if (beam2.note != 0) {
      beam2.freqReads[0] = analogRead (A1);
      beam2.freqReads[1] = analogRead (A1);
      beam2.freqReads[2] = analogRead (A1);
      beam2.freq = (beam2.freqReads[0] + beam2.freqReads[1] + beam2.freqReads[2]) / 3;
  }
  else if (beam3.note != 0) {
    beam3.freqReads[0] = analogRead (A2);
    beam3.freqReads[1] = analogRead (A2);
    beam3.freqReads[2] = analogRead (A2);
    beam3.freq = (beam.freqReads[0] + beam3.freqReads[1] + beam3.freqReads[2]) / 3;
  }
  else if (beam4.note != 0) {
    beam4.freqReads[0] = analogRead (A3);
    beam4.freqReads[1] = analogRead (A3);
    beam4.freqReads[2] = analogRead (A3);
    beam4.freq = (beam4.freqReads[0] + beam4.freqReads[1] + beam4.freqReads[2]) / 3;
  }
  else if (beam5.note != 0) {
    beam5.freqReads[0] = analogRead (A4);
    beam5.freqReads[1] = analogRead (A4);
    beam5.freqReads[2] = analogRead (A4);
    beam5.freq = (beam5.freqReads[0] + beam5.freqReads[1] + beam5.freqReads[2]) / 3;
  }
  else if (beam6.note != 0) {
    beam6.freqReads[0] = analogRead (A5);
    beam6.freqReads[1] = analogRead (A5);
    beam6.freqReads[2] = analogRead (A5);
    beam6.freq = (
  }*/
} /* End ISR */
