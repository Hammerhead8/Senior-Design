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
Beam beam2;
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
  DDRE &= ~(0x18); /* Configure pins 2 and 3 as an input */
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
  attachInterrupt (digitalPinToInterrupt (3), playBeam2, CHANGE);
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
  
  /* The ranges for the notes still need to be optimized */

  if (beam1.note != 0) { /* If the first beam is broken */
    while (beam1.note != 0) {
      if (beam1.difference == 1) { /* If the first note is being played */
        tone (4, NOTE1_1);
        while (beam1.difference == 1) {}
        noTone (4);
      }
      else if (beam1.difference == 2) { /* If the second note is being played */
        tone (4, NOTE2_1);
        while (beam1.difference == 2) {}
        noTone (4);
      }
      else if (beam1.difference == 3) { /* If the third note is being played */
        tone (4, NOTE3_1);
        while (beam1.difference == 3) {}
        noTone (4);
      }
      else if (beam1.difference == 4) { /* If the fourth note is being played */
        tone (4, NOTE4_1);
        while (beam1.difference == 4) {}
        noTone (4);
      }
      else if (beam1.difference == 5) { /* If the fifth note is being played */
        tone (4, NOTE5_1);
        while (beam1.difference == 5) {}
        noTone (4);
      }
      else if (beam1.difference == 6) { /* If the sixth note is being played */
        tone (4, NOTE6_1);
        while (beam1.difference == 6) {}
        noTone (4);
      }
      else if (beam1.difference == 7) { /* If the seventh note is being played */
        tone (4, NOTE7_1);
        while (beam1.difference == 7) {}
        noTone (4);
      } /* end if */
    } /* End while */
  } /* End if */

  else if (beam2.note != 0) { /* If the second beam is broken */
    while (beam2.note != 0) {
      if (beam2.difference == 1) { /* If the first note is being played */
        tone (4, NOTE1_2);
        while (beam2.difference == 1) {}
        noTone (4);
      }
      else if (beam2.difference == 2) { /* If the second note is being played */
        tone (4, NOTE2_2);
        while (beam2.difference == 2) {}
        noTone (4);
      }
      else if (beam2.difference == 3) { /* If the third note is being played */
        tone (4, NOTE3_2);
        while (beam2.difference == 3){}
        noTone (4);
      }
      else if (beam2.difference == 4) { /* If the fourth note is being played */
        tone (4, NOTE4_2);
        while (beam2.difference == 4) {}
        noTone (4);
      }
      else if (beam2.difference == 5) { /* If the fifth note is being played */
        tone (4, NOTE5_2);
        while (beam2.difference == 5) {}
        noTone (4);
      }
      else if (beam2.difference == 6) { /* If the sixth note is being played */
        tone (4, NOTE6_2);
        while (beam2.difference == 6) {}
        noTone (4);
      }
      else if (beam2.difference == 7) { /* If the seventh note is being played */
        tone (4, NOTE7_2);
        while (beam2.difference == 7) {}
        noTone (4);
      } /* End if */
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
  beam1.note ^= 1; /* Toggle NOTE */
  if (beam1.note == 1) { /* If the beam is broken */
    beam1.freq = analogRead (A0); /* Read from the ADC */

    if (beam1.freq <= 40) { /* Range for the first note */
      beam1.difference = 1; /* Set DIFFERENCE to 1 for the first note */
    }
    else if (beam1.freq >= 41 && beam1.freq <= 80) { /* Range for the second note */
      beam1.difference = 2; /* Set DIFFERENCE to 2 for the second note */
    }
    else if (beam1.freq >= 80 && beam1.freq <= 120) { /* Range for the third note */
      beam1.difference = 3; /* Set DIFFERENCE to 3 for the third note */
    }
    else if (beam1.freq >= 121 && beam1.freq <= 280) { /* Range for the fourth note */
      beam1.difference = 4; /* Set DIFFERENCE to 4 for the fourth note */
    }
    else if (beam1.freq >= 281 && beam1.freq <= 460) { /* Range for the fifth note */
      beam1.difference = 5; /* Set DIFFERENCE to 5 for the fifth note */
    }
    else if (beam1.freq >= 461 && beam1.freq <= 750) { /* Range for the sixth note */
      beam1.difference = 6; /* Set DIFFERENCE to 6 for the sixth note */
    }
    else if (beam1.freq >= 751) { /* Range for the seventh note */
      beam1.difference = 7; /* Set DIFFERENCE to 7 for the seventh note */
    } /* End if */

    TCCR1B |= 0x0b; /* Enable Timer 1 */
    TCNT1 = 0; /* Start Timer 1 from 0 */
  }
  else {
    noTone (4); /* Stop the note from playing */
    beam1.freq = -1; /* Set FREQ to -1 to make its value useless */
    beam1.difference = -1; /* Set DIFFERENCE to -1 to make is value useless */
    TCCR1B &= 0x00; /* Disable the timer */
  } /* End if */
} /* End function */


/* ISR for the second beam. It works exactly the same as
 * the ISR for the first beam.
 * NOTE is toggled between 0 and 1 every time the beam is broken.
 * When NOTE == 1 then the beam is broken and the timer is started.
 * When NOTE == 0 then the beam is no longer broken and the timer
 * is stopped and any note that is playing is killed */
void
playBeam2 ()
{
  beam2.note ^= 1; /* Toggle NOTE */
  if (beam2.note == 1) { /* If the beam is broken */
    beam2.freq = analogRead (A1); /* Read from the ADC */

    if (beam2.freq <= 40) { /* Range for the first note */
      beam2.difference = 1; /* Set DIFFERENCE to 1 for the first note */
    }
    else if (beam2.freq >= 41 && beam2.freq <= 80) { /* Range for the second note */
      beam2.difference = 2; /* Set DIFFERENCE to 2 for the second note */
    }
    else if (beam2.freq >= 81 && beam2.freq <= 120) { /* Range for the third note */
      beam2.difference = 3; /* Set DIFFERENCE to 3 for the third note */
    }
    else if (beam2.freq >= 121 && beam2.freq <= 280) { /* Range for the fourth note */
      beam2.difference = 4; /* Set DIFFERENCE to 4 for the fourth note */
    }
    else if (beam2.freq >= 281 && beam2.freq <= 460) { /* Range for the fifth note */
      beam2.difference = 5; /* Set DIFFERENCE to 5 for the fifth note */
    }
    else if (beam2.freq >= 281 && beam2.freq <= 750) { /* Range for the sixth note */
      beam2.difference = 6; /* Set DIFFERENCE to 6 for the sixth note */
    }
    else if (beam2.freq >= 751) { /* Range for the seventh note */
      beam2.difference = 7; /* Set DIFFERENCE to 7 for the seventh note */
    } /* End if */

    TCCR1B |= 0x0b; /* Enable the timer */
    TCNT1 = 0; /* Start the timer from 0 */
  } /* End if */

  else { /* If the beam is no longer broken */
    noTone (4); /* Stop the note from playing */
    beam2.freq = -1;
    beam2.difference = -1;
    TCCR1B &= 0x00; /* Disable the timer */
  } /* End else */
} /* End ISR */

/* Add additional ISRs for the other beams as they are implemented */

/* Timer1 interrupt vector */
/* When a timer interrupt occurs we will take three readings from the ADC.
 * These readings will then be averaged to try to increase accuracy of detection. */
ISR (TIMER1_COMPA_vect)
{
  if (beam1.note != 0) { /* If the first beam is broken */
    /* Take four successive reads from channel 0 of the ADC */
    beam1.freqReads[0] = analogRead (A0);
    beam1.freqReads[1] = analogRead (A0);
    beam1.freqReads[2] = analogRead (A0);
    beam1.freqReads[3] = analogRead (A0);

    /* Now add all four reads together */
    beam1.freq = beam1.freqReads[0];
    beam1.freq += beam1.freqReads[1];
    beam1.freq += beam1.freqReads[2];
    beam1.freq += beam1.freqReads[3];

    /* Now get the arithmetic mean of the reads */
    beam1.freq /= 4;

    if (beam1.freq <= 2) { /* Range of the first note */
      beam1.difference = 1; /* Set DIFFERENCE to 1 for the first note */
    }
    else if (beam1.freq >= 3 && beam1.freq <= 60) { /* Range for the second note */
      beam1.difference = 2; /* Set DIFFERENCE to 2 for the second note */
    }
    else if (beam1.freq >= 61 && beam1.freq <= 120) { /* Range for the third note */
      beam1.difference = 3; /* Set DIFFERENCE to 3 for the third note */
    }
    else if (beam1.freq >= 121 && beam1.freq <= 280) { /* Range for the fourth note */
      beam1.difference = 4; /* Set DIFFERENCE to 4 for the fourth note */
    }
    else if (beam1.freq >= 281 && beam1.freq <= 460) { /* Range for the fifth note */
      beam1.difference = 5; /* Set difference to 5 for the fifth note */
    }
    else if (beam1.freq >= 461 && beam1.freq <= 750) { /* Range for the sixth note */
      beam1.difference = 6; /*Set DIFFERENCE to 6 for the sixth note */
    }
    else if (beam1.freq >= 751) { /* Range for the seventh note */
      beam1.difference = 7; /* Set DIFFERENCE to 7 for the seventh note */
    } /* End if */
  } /* End if */

  else if (beam2.note != 0) { /* If the second beam is broken */
    /* Take four successive reads from channel 1 of the ADC */
    beam2.freqReads[0] = analogRead (A1);
    beam2.freqReads[1] = analogRead (A1);
    beam2.freqReads[2] = analogRead (A1);
    beam2.freqReads[3] = analogRead (A1);

    /* Sum the four reads together */
    beam2.freq = beam2.freqReads[0];
    beam2.freq += beam2.freqReads[1];
    beam2.freq += beam2.freqReads[2];
    beam2.freq += beam2.freqReads[3];

    /* Now find the arithmetic mean of the reads */
    beam2.freq /= 4;

    if (beam2.freq <= 2) { /* Range for the first note */
      beam2.difference = 1; /* Set DIFFERENCE to 1 for the first note */
    }
    else if (beam2.freq >= 3 && beam2.freq <= 60) { /* Range for the second note */
      beam2.difference = 2; /* Set DIFFERENCE to 2 for the second note */
    }
    else if (beam2.freq >= 61 && beam2.freq <= 120) { /* Range for the third note */
      beam2.difference = 3; /* Set DIFFERENCE to 3 for the third note */
    }
    else if (beam2.freq >= 121 && beam2.freq <= 280) { /* Range for the fourth note */
      beam2.difference = 4; /* Set DIFFERENCE to 4 for the fourth note */
    }
    else if (beam2.freq >= 281 && beam2.freq <= 460) { /* Range for the fifth note */
      beam2.difference = 5; /* Set DIFFERENCE to 5 for the fifth note */
    }
    else if (beam2.freq >= 281 && beam2.freq <= 750) { /* Range for the sixth note */
      beam2.difference = 6; /* Set DIFFERENCE to 6 for the sixth note */
    }
    else if (beam2.freq >= 751) { /* Range for the seventh note */
      beam2.difference = 7; /* Set DIFFERENCE to 7 for the seventh note */
    } /* End if */
  } /* End if */
} /* End ISR */
