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

/* Writing 0 to bits 0, 1, and 2 of TCCR0B disables the counter just FYI */
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
  /* PINx sets the pin to either high or low */
  /* DDRx sets the direction of the pin as either input (0) or output (1) */
  DDRD &= 0x00;
  DDRD |= 0x10; /* Configure pin 4 as output */
  DDRD &= ~(0x04); /* Configure pin 2 as an input */
  DDRF &= 0x00; /* Configure the analog pins as inputs */

  /* Enable external interrupts on pins 2, 3, 18, 19, 20, and 21 */
  /* The interrupt service routines will toggle a flag in order to */
  /* play the note in the loop() function */
  /* We will use CHANGE for the condition since we need to toggle */
  /* the flag whenever the photoresistor is covered or uncovered */
  attachInterrupt (digitalPinToInterrupt (2), playNote1, CHANGE);

  /* Configure timer 1 to interrupt every 100 milliseconds when reading from the ADC */
  /* WGM10 = 0, WGM11 = 0, WGM12 = 1, WGM13 = 0 */
  /* Whenever a timer interrupt occurs read from the ADC */
  /* This might help reduce the noise currently being heard */
  /* Remember:  100 ms is the maximum delay before a UI starts
   * to look laggy. */
  TCCR1A &= 0x00; /* Clear Timer/Counter 1 Control Register A */
  TCCR1B &= 0x00; /* Clear Timer/Counter 1 Control Register B */
  OCR1A = 24999; /* (16000000 / (12.5 * 64) - 1 = 24999 for an 100 ms delay */
  TIMSK1 |= (1 << OCIE1A); /* Enable Timer 1 compare interrupts */
}

/* Main loop */
/* The analogRead() function returns a number between 0 and 1023. */
/* This number will be used to determine which note to play. */
void
loop ()
{
  /* Check if the first beam is broken */
  /* If it is then the second octave will be played */
  
  /* Right now the first note takes up almost half of the available range when tested in the dark 
   * and the others each have a range of about an inch.
   */
  if (beam1.note != 0) {
    while (beam1.note != 0) {
      beam1.freq = analogRead (A0);
      if (beam1.freq <= 5) {
        tone (4, TONE1);
        while (beam1.freq <= 5) {}
        noTone (4);
      } /* End if */
      else if (beam1.freq >= 6 && beam1.freq <= 40) {
        tone (4, TONE2);
        while (beam1.freq >= 16 && beam1.freq <= 40) {}
        noTone (4);
      } /* End else if */
      else if (beam1.freq >= 41 && beam1.freq <= 80) {
        tone (4, TONE3);
        while (beam1.freq >= 41 && beam1.freq <= 80) {}
        noTone (4);
      } /* End else if */
      else if (beam1.freq >= 81 && beam1.freq <= 150) {
        tone (4, TONE4);
        while (beam1.freq >= 81 && beam1.freq <= 150) {}
        noTone (4);
      } /* End else if */
      else if (beam1.freq >= 151 && beam1.freq <= 260) {
        tone (4, TONE5);
        while (beam1.freq >= 151 && beam1.freq <= 260) {}
        noTone (4);
      } /* End else if */
      else if (beam1.freq >= 261 && beam1.freq <= 390) {
        tone (4, TONE6);
        while (beam1.freq >= 261 && beam1.freq <= 390) {}
        noTone (4);
      } /* End else if */
      else if (beam1.freq >= 391) {
        tone (4, TONE7);
        while (beam1.freq >= 391) {}
        noTone (4);
      } /* End else if */
    } /* End while */
  } /* End if */
  
  /* Use an else statement to check if more than one beam is broken.
   * Also add something to the other conditionals to check that only one beam is broken. */

} /* End loop function */

/* ISR for the first beam */
void
playNote1 ()
{
  beam1.note ^= 1; /* toggle NOTE */
  if (beam1.note == 1) {
    TCCR1B |= 0x0b; /* Enable the timer */
    TCNT1 = 0; /* Start the timer at zero */
  }
  else {
    noTone (4); /* Stop the note from playing */
    beam1.freq = -1; /* Set FREQ to -1 to make its value useless */
    TCCR1B &= 0x00; /* Disable the timer */
  } /* End if */

  /* Add the same as above but for the other beams as they are implemented */

} /* End function */

/* Timer1 interrupt vector */
/* When an interrupt occurs we will take three readings from the ADC.
 * These readings will then be averaged to try to increase accuracy of detection. */
ISR (TIMER1_COMPA_vect)
{
  if (beam1.note != 0) {
    beam1.freqReads[0] = analogRead (A0);
    beam1.freqReads[1] = analogRead (A1);
    beam1.freqReads[2] = analogRead (A2);
    beam1.freqReads[3] = analogRead (A3);
//    beam1.freqReads[4] = analogRead (A4);
//    beam1.freqReads[5] = analogRead (A5);
    beam1.freq = (beam1.freqReads[0] + beam1.freqReads[1] + beam1.freqReads[2] + beam1.freqReads[3]) / 4;
  }
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
}
