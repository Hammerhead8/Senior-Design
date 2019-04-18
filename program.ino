/* Test program using interrupts to trigger sounds for six beams
 * If two beams are broken at the same time, then the one broken first
 * will be played */

/* The formula for the value for OCR1A is as follows:
   compare match register = [ 16,000,000Hz/ (prescaler * desired interrupt frequency) ] - 1

   The formula for the interrupt frequency is as follows:
   interrupt frequency (Hz) = (Arduino clock speed 16,000,000Hz)
               / (prescaler * (compare match register + 1))

   Since we want an interrupt every 80 milliseconds, the desired interrupt
   frequency is 12.5 Hz

   Using a prescale value of f_clk / 64, this gives a OCRA1 value of
   [16000000 / (64 * 12.5)) - 1 = 19999 */

/* Writing 0 to bits 0, 1, and 2 of TCCR3B disables the counter just FYI */
//#include <avr/sleep.h> /* For sleep_cpu */
#include "beam.h" /* For Beam class */
#include "notes.h" /* For note frequency definitions */

/* Create an instance of the class Beam for each beam being used called beamx */
Beam beam1;
Beam beam2;
Beam beam3;
Beam beam4;
Beam beam5;
Beam beam6;

/* Configure the input and output pins */
/* Also configure the timer and enable interrupts */
void
setup ()
{
  /* Set the clock prescaler to 1 to try to prevent weird timing shit.
     The default for the ATMega2560 is for the clock to be set to 8 MHz
     when a reset occurs. Setting the clock prescaler to 1 allows the 16 MHz
     resonator to be used as the clock source at its normal frequency */
  CLKPR = 0x80; /* Enable clock prescaler changes */
  CLKPR = 0x00; /* Set the clock prescaler to a division factor of 1 */

  /* DDRx sets the direction of the pin as either input (0) or output (1) */
  DDRG = 0x20; /* Configure digital pin 4 as an output */
  PORTG &= 0x00; /* Start with pin 4 set LOW */
  DDRD &= ~(0x0f); /* Configure pins 18, 19, 20, and 21 as input */
  DDRE &= ~(0x30); /* Configure pins 2 and 3 as an input */
  DDRF &= 0x00;/* Configure the analog pins as inputs */

  /* Configure the unused I/O pins as inputs and drive them low */
  /* Port A */
  DDRA &= 0x00;;
  PORTA |= 0xff;

  /* Port B */
  DDRB &= 0x00;
  PORTB |= 0xff;

  /* Port C */
  DDRC &= 0x00;
  PORTC |= 0xff;

  /* Port H */
  DDRH &= 0x00;
  PORTH |= 0xff;

  /* Port J */
  DDRJ &= 0x00;
  PORTJ |= 0xff;

  /* Port K */
  DDRK &= 0x00;
  PORTK |= 0xff;

  /* Port L */
  DDRL &= 0x00;
  PORTL |= 0xff;;

  /* The Power Reduction Registers (PRR0 and PRR1) can be used to turn
     off things that aren't needed to save power */
  PRR0 = 0xa6;
  PRR1 = 0x3f;

  /* The Sleep Mode Control Register (SMCR) is used to select the sleep
     mode for the MCU. writing 0x00 to it selects idle mode without
     also setting the sleep enable bit. This will be done later.
     Using any mode other than idle mode only allows for using level
     interrupts (HIGH or LOW) in attachInterrupt. Since we need an
     interrupt whenever the voltage changes on all of the beams,
     we have to use idle mode. */
//  SMCR = 0x00;

  /* Configure timer 1 to interrupt every 100 milliseconds when reading from the ADC */
  /* WGM10 = 0, WGM11 = 0, WGM12 = 1, WGM13 = 0 */
  /* Whenever a timer interrupt occurs read from the ADC
     This might help reduce the noise currently being heard */
  /* Remember:  100 ms is the maximum delay before a UI starts
     to look laggy. */
  TCCR1A &= 0x00; /* Clear Timer/Counter 1 Control Register A */
  TCCR1B &= 0x00; /* Clear Timer/Counter 1 Control Register B */
  OCR1A = 9999; /* (16000000 / (12.5 * 64) - 1 = 24999 for an 100 ms delay
                   but the MCU is fucking around so we need OCR1A = 9999
                   for a similar delay because reasons. */
  TIMSK1 = 0x02; /* Enable Timer 1 compare interrupts */

  /* Enable external interrupts on pins 2, 3, 18, 19, 20, and 21
     The interrupt service routines will toggle a flag in order to
     play the note in the loop() function
     We will use CHANGE for the condition since we need to toggle
     the flag whenever the photoresistor is covered or uncovered */
  attachInterrupt (digitalPinToInterrupt (2), playBeam1, CHANGE);
  attachInterrupt (digitalPinToInterrupt (3), playBeam2, CHANGE);
  attachInterrupt (digitalPinToInterrupt (21), playBeam3, CHANGE);
  attachInterrupt (digitalPinToInterrupt (20), playBeam4, CHANGE);
  attachInterrupt (digitalPinToInterrupt (19), playBeam5, CHANGE);
  attachInterrupt (digitalPinToInterrupt (18), playBeam6, CHANGE);

  noTone (4); /* Likes to play notes here, so stop any that started. */

}

/* Main loop */
/* The analogRead() function returns a number between 0 and 1023. */
/* This number will be used to determine which note to play. */
void
loop ()
{
  /* Check if the first beam is broken */
  /* If it is then the second octave will be played */

//    SMCR |= 0x01; /* Set the Sleep Enable (SE) bit in SMCR */
//    sleep_cpu (); /* Put the MCU to sleep */

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
      else { /* If DIFFERENCE is any other value */
        noTone(4);
      } /* End if */
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
        while (beam2.difference == 3) {}
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

  else if (beam3.note != 0) { /* If the third beam is active */
    while (beam3.note != 0) {
      if (beam3.difference == 1) { /* If the first note is being played */
        tone (4, NOTE1_3);
        while (beam3.difference == 1) {}
        noTone (4);
      }
      else if (beam3.difference == 2) { /* If the second note is being played */
        tone (4, NOTE2_3);
        while (beam3.difference == 2) {}
        noTone (4);
      }
      else if (beam3.difference == 3) { /* If the third note is being played */
        tone (4, NOTE3_3);
        while (beam3.difference == 3) {}
        noTone (4);
      }
      else if (beam3.difference == 4) { /* If the fourth note is being played */
        tone (4, NOTE4_3);
        while (beam3.difference == 4) {}
        noTone (4);
      }
      else if (beam3.difference == 5) { /* If the fifth note is being played */
        tone (4, NOTE5_3);
        while (beam3.difference == 5) {}
        noTone (4);
      }
      else if (beam3.difference == 6) { /* If the sixth note is being played */
        tone (4, NOTE6_3);
        while (beam3.difference == 6) {}
        noTone (4);
      }
      else if (beam3.difference == 7) { /* If the seventh note is being played */
        tone (4, NOTE7_3);
        while (beam3.difference == 7) {}
        noTone (4);
      } /* End if */
    } /* End while */
  } /* End else if */

  else if (beam4.note != 0) { /* If the fourth beam is active */
    while (beam4.note != 0) {
      if (beam4.difference == 1) { /* If the first note is being played */
        tone (4, NOTE1_4);
        while (beam4.difference == 1) {}
        noTone (4);
      }
      else if (beam4.difference == 2) { /* If the second note is being played */
        tone (4, NOTE2_4);
        while (beam4.difference == 2) {}
        noTone (4);
      }
      else if (beam4.difference == 3) { /* If the third note is being played */
        tone (4, NOTE3_4);
        while (beam4.difference == 3) {}
        noTone (4);
      }
      else if (beam4.difference == 4) { /* If the fourth note is being played */
        tone (4, NOTE4_4);
        while (beam4.difference == 5) {}
        noTone (4);
      }
      else if (beam4.difference == 5) { /* If the fifth note is being played */
        tone (4, NOTE5_4);
        while (beam4.difference == 5) {}
        noTone (4);
      }
      else if (beam4.difference == 6) { /* If the sixth note is being played */
        tone (4, NOTE6_4);
        while (beam4.difference == 6) {}
        noTone (4);
      }
      else if (beam4.difference == 7) { /* If the seventh note is being played */
        tone (4, NOTE7_4);
        while (beam4.difference == 7) {}
        noTone (4);
      } /* End if */
    } /* End while */
  } /* End else if */

  else if (beam5.note != 0) { /* If the fifth beam is active */
    while (beam5.note != 0) {
      if (beam5.difference == 1) { /* If the first note is being played */
        tone (4, NOTE1_5);
        while (beam5.difference == 1) {}
        noTone (4);
      }
      else if (beam5.difference == 2) { /* If the second note is being played */
        tone (4, NOTE2_5);
        while (beam5.difference == 2) {}
        noTone (4);
      }
      else if (beam5.difference == 3) { /* If the third note is being played */
        tone (4, NOTE3_5);
        while (beam5.difference == 3) {}
        noTone (4);
      }
      else if (beam5.difference == 4) { /* If the fourth note is being played */
        tone (4, NOTE4_5);
        while (beam5.difference == 4) {}
        noTone (4);
      }
      else if (beam5.difference == 5) { /* If the fifth note is being played */
        tone (4, NOTE5_5);
        while (beam5.difference == 5) {}
        noTone (4);
      }
      else if (beam5.difference == 6) { /* If the sixth note is being played */
        tone (4, NOTE6_5);
        while (beam5.difference == 6) {}
        noTone (4);
      }
      else if (beam5.difference == 7) { /* If the seventh note is being played */
        tone (4, NOTE7_5);
        while (beam5.difference == 7) {}
        noTone (4);
      } /* End if */
    } /* End while */
  } /* End else if */

  else if (beam6.note != 0) { /* If the sixth beam is active */
    while (beam6.note != 0) {
      if (beam6.difference == 1) { /* If the first note is being played */
        tone (4, NOTE1_6);
        while (beam6.difference == 1) {}
        noTone (4);
      }
      else if (beam6.difference == 2) { /* If the second note is being played */
        tone (4, NOTE2_6);
        while (beam6.difference == 2) {}
        noTone (4);
      }
      else if (beam6.difference == 3) { /* If the third note is being played */
        tone (4, NOTE3_6);
        while (beam6.difference == 3) {}
        noTone (4);
      }
      else if (beam6.difference == 4) { /* If the fourth note is being played */
        tone (4, NOTE4_6);
        while (beam6.difference == 4) {}
        noTone (4);
      }
      else if (beam6.difference == 5) { /* If the fifth note is being played */
        tone (4, NOTE5_6);
        while (beam6.difference == 5) {}
        noTone (4);
      }
      else if (beam6.difference == 6) { /* If the sixth note is being played */
        tone (4, NOTE6_6);
        while (beam6.difference == 6) {}
        noTone (4);
      }
      else if (beam6.difference == 7) { /* If the seventh note is being played */
        tone (4, NOTE7_6);
        while (beam6.difference == 7) {}
        noTone (4);
      } /* End if */
    } /* End while */
  } /* End if */

  /* TODO:  Use an else statement to check if more than one beam is broken.
     Also add something to the other conditionals to check that only one beam is broken. */

} /* End loop function */


/* ISR for the first beam
   If BEAM1.NOTE is 1 then the beam is broken and the timer can be started.
   If BEAM1.NOTE is 0 then the beam is no longer broken and the timer can be stopped.
   Also, we need to kill any note that is being played and make the value in
   BEAM1.FREQ useless by setting it to -1. */
void
playBeam1 ()
{
//  SMCR &= ~(0x01); /* Clear the Sleep Enable bit */
//  beam1.note ^= 1; /* Toggle NOTE */
  beam1.note = PINE & 0x10; /* Capture the state of pin 2 */

  if (beam1.note != 0) { /* If the beam is broken */
    beam1.freq = analogRead (A0); /* Read from the ADC */

    if (beam1.freq <= 30) { /* Range for the first note */
      beam1.difference = 1; /* Set DIFFERENCE to 1 for the first note */
    }
    else if (beam1.freq >= 31 && beam1.freq <= 60) { /* Range for the second note */
      beam1.difference = 2; /* Set DIFFERENCE to 2 for the second note */
    }
    else if (beam1.freq >= 61 && beam1.freq <= 120) { /* Range for the third note */
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
   the ISR for the first beam.
   NOTE is toggled between 0 and 1 every time the beam is broken.
   When NOTE == 1 then the beam is broken and the timer is started.
   When NOTE == 0 then the beam is no longer broken and the timer
   is stopped and any note that is playing is killed */
void
playBeam2 ()
{
//  SMCR &= ~(0x01); /* Clear the Sleep Enable bit */
//  beam2.note ^= 1; /* Toggle NOTE */

  beam2.note = PINE & 0x20; /* Capture the state of pin 3 */

  if (beam2.note != 0) { /* If the beam is broken */
    beam2.freq = analogRead (A1); /* Read from the ADC */

    if (beam2.freq <= 30) { /* Range for the first note */
      beam2.difference = 1; /* Set DIFFERENCE to 1 for the first note */
    }
    else if (beam2.freq >= 31 && beam2.freq <= 60) { /* Range for the second note */
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

/* ISR for the third beam. It works exactly the same as
   the ISR for the first beam.
   NOTE is toggled between 0 and 1 every time the beam is broken.
   When NOTE == 1 then the beam is broken and the timer is started.
   When NOTE == 0 then the beam is no longer broken and the timer
   is stopped and any note that is playing is killed */
void
playBeam3 ()
{
// SMCR &= ~(0x01); /* Clear the Sleep Enable bit */
// beam3.note ^= 1; /* Toggle NOTE */

  beam3.note = PIND & 0x01;
  
  if (beam3.note == 1) { /* If the beam is broken */
    beam3.freq = analogRead (A2); /* Get an initial reading from the ADC */

    if (beam3.freq <= 30) { /* Range of the first note */
      beam3.difference = 1;
    }
    else if (beam3.freq >= 31 && beam3.freq <= 60) { /* Range of the second note */
      beam3.difference = 2;
    }
    else if (beam3.freq >= 61 && beam3.freq <= 120) { /* Range of the third note */
      beam3.difference = 3;
    }
    else if (beam3.freq >= 121 && beam3.freq <= 280) { /* Range of the fourth note */
      beam3.difference = 4;
    }
    else if (beam3.freq >= 281 && beam3.freq <= 460) { /* Range of the fifth note */
      beam3.difference = 5;
    }
    else if (beam3.freq >= 461 && beam3.freq <= 750) { /* Range of the sixth note */
      beam3.difference = 6;
    }
    else if (beam3.freq >= 751) { /* Range of the seventh note */
      beam3.difference = 7;
    }

    TCCR1B |= 0x0b; /* Enable the timer */
    TCNT1 = 0; /* Start the timer from 0 */
  }

  else { /* If the beam is no longer broken */
    noTone (4);
    beam3.freq = -1;
    beam3.difference = -1;
    TCCR1B &= 0x00; /* Disable the timer */
  } /* End if */
} /* End ISR */

/* ISR for the fourth beam. It works exactly the same as
   the ISR for the first beam.
   NOTE is toggled between 0 and 1 every time the beam is broken.
   When NOTE == 1 then the beam is broken and the timer is started.
   When NOTE == 0 then the beam is no longer broken and the timer
   is stopped and any note that is playing is killed */
void
playBeam4 ()
{
//  SMCR &= ~(0x01); /* Clear the Sleep Enable bit */
//  beam4.note ^= 1; /* Toggle NOTE */

  beam4.note = PIND & 0x02; /* Capture the state of pin 20 */

  if (beam4.note != 0) { /* If the beam is broken */
    beam4.freq = analogRead (A3);

    if (beam4.freq <= 30) { /* Range of the first note */
      beam4.difference = 1;
    }
    else if (beam4.freq >= 31 && beam4.freq <= 60) { /* Range of the second note */
      beam4.difference = 2;
    }
    else if (beam4.freq >= 61 && beam4.freq <= 120) { /* Range of the third note */
      beam4.difference = 3;
    }
    else if (beam4.freq >= 121 && beam4.freq <= 280) { /* Range of the fourth note */
      beam4.difference = 4;
    }
    else if (beam4.freq >= 281 && beam4.freq <= 460) { /* Range of the fifth note */
      beam4.difference = 5;
    }
    else if (beam4.freq >= 461 && beam4.freq >= 750) { /* Range of the sixth note */
      beam4.difference = 6;
    }
    else if (beam4.freq >= 751) { /* Range of the seventh note */
      beam4.difference = 7;
    }

    TCCR1B |= 0x0b; /* Enable the timer */
    TCNT1 = 0; /* Start the timer from 0 */
  }

  else { /* If the beam is no longer broken */
    noTone (4);
    beam4.freq = -1;
    beam4.difference = -1;
    TCCR1B &= 0x00; /* Disable the timer */
  } /* End if */
} /* End ISR */

/* ISR for the fifth beam. It works exactly the same as
   the ISR for the first beam.
   NOTE is toggled between 0 and 1 every time the beam is broken.
   When NOTE == 1 then the beam is broken and the timer is started.
   When NOTE == 0 then the beam is no longer broken and the timer
   is stopped and any note that is playing is killed */
void
playBeam5 ()
{
//  SMCR &= ~(0x01); /* Clear the Sleep Enable bit */
//  beam5.note ^= 1; /* Toggle NOTE */

  beam5.note = PIND & 0x04; /* Capture the state of pin 19 */

  if (beam5.note != 0) { /* If the beam is broken */
    beam5.freq = analogRead (A4);

    if (beam5.freq <= 30) { /* Range of the first note */
      beam5.difference = 1;
    }
    else if (beam5.freq >= 31 && beam5.freq <= 60) { /* Range of the second note */
      beam5.difference = 2;
    }
    else if (beam5.freq >= 61 && beam5.freq <= 120) { /* Range of the third note */
      beam5.difference = 3;
    }
    else if (beam5.freq >= 121 && beam5.freq <= 280) { /* Range of the fourth note */
      beam5.difference = 4;
    }
    else if (beam5.freq >= 281 && beam5.freq <= 460) { /* Range of the fifth note */
      beam5.difference = 5;
    }
    else if (beam5.freq >= 461 && beam5.freq >= 750) { /* Range of the sixth note */
      beam5.difference = 6;
    }
    else if (beam5.freq >= 751) { /* Range of the seventh note */
      beam5.difference = 7;
    }

    TCCR1B |= 0x0b; /* Enable the timer */
    TCNT1 = 0; /* Start the timer from 0 */
  }

  else { /* If the beam is no longer broken */
    noTone (4);
    beam5.freq = -1;
    beam5.difference = -1;
    TCCR1B &= 0x00; /* Disable the timer */
  } /* End if */
} /* End ISR */

/* ISR for the sixth beam. It works exactly the same as
   the ISR for the first beam.
   NOTE is toggled between 0 and 1 every time the beam is broken.
   When NOTE == 1 then the beam is broken and the timer is started.
   When NOTE == 0 then the beam is no longer broken and the timer
   is stopped and any note that is playing is killed */
void
playBeam6 ()
{
//  SMCR &= ~(0x01); /* Clear the Sleep Enable bit */
//  beam6.note ^= 1; /* Toggle NOTE */

  beam6.note = PIND & 0x08; /* Capture the state of pin 18 */

  if (beam6.note != 0) { /* If the beam is broken */
    beam6.freq = analogRead (A5);

    if (beam6.freq <= 30) { /* Range of the first note */
      beam6.difference = 1;
    }
    else if (beam6.freq >= 31 && beam6.freq <= 60) { /* Range of the second note */
      beam6.difference = 2;
    }
    else if (beam6.freq >= 61 && beam6.freq <= 120) { /* Range of the third note */
      beam6.difference = 3;
    }
    else if (beam6.freq >= 121 && beam6.freq <= 280) { /* Range of the fourth note */
      beam6.difference = 4;
    }
    else if (beam6.freq >= 281 && beam6.freq <= 460) { /* Range of the fifth note */
      beam6.difference = 5;
    }
    else if (beam6.freq >= 461 && beam6.freq >= 750) { /* Range of the sixth note */
      beam6.difference = 6;
    }
    else if (beam6.freq >= 751) { /* Range of the seventh note */
      beam6.difference = 7;
    }

    TCCR1B |= 0x0b; /* Enable the timer */
    TCNT1 = 0; /* Start the timer from 0 */
  }

  else { /* If the beam is no longer broken */
    noTone (4);
    beam6.freq = -1;
    beam6.difference = -1;
    TCCR1B &= 0x00; /* Disable the timer */
  } /* End if */
} /* End ISR */

/* Timer1 interrupt vector */
/* When a timer interrupt occurs we will take three readings from the ADC.
   These readings will then be averaged to try to increase accuracy of detection. */
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

    if (beam1.freq <= 30) { /* Range of the first note */
      beam1.difference = 1; /* Set DIFFERENCE to 1 for the first note */
    }
    else if (beam1.freq >= 31 && beam1.freq <= 60) { /* Range for the second note */
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

    if (beam2.freq <= 30) { /* Range for the first note */
      beam2.difference = 1; /* Set DIFFERENCE to 1 for the first note */
    }
    else if (beam2.freq >= 31 && beam2.freq <= 60) { /* Range for the second note */
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

  else if (beam3.note != 0) { /* If the second beam is broken */
    /* Take four successive reads from channel 1 of the ADC */
    beam3.freqReads[0] = analogRead (A2);
    beam3.freqReads[1] = analogRead (A2);
    beam3.freqReads[2] = analogRead (A2);
    beam3.freqReads[3] = analogRead (A2);

    /* Sum the four reads together */
    beam3.freq = beam3.freqReads[0];
    beam3.freq += beam3.freqReads[1];
    beam3.freq += beam3.freqReads[2];
    beam3.freq += beam3.freqReads[3];

    /* Now find the arithmetic mean of the reads */
    beam3.freq /= 4;

    if (beam3.freq <= 30) { /* Range for the first note */
      beam3.difference = 1; /* Set DIFFERENCE to 1 for the first note */
    }
    else if (beam3.freq >= 31 && beam3.freq <= 60) { /* Range for the second note */
      beam3.difference = 2; /* Set DIFFERENCE to 2 for the second note */
    }
    else if (beam3.freq >= 61 && beam3.freq <= 120) { /* Range for the third note */
      beam3.difference = 3; /* Set DIFFERENCE to 3 for the third note */
    }
    else if (beam3.freq >= 121 && beam3.freq <= 280) { /* Range for the fourth note */
      beam3.difference = 4; /* Set DIFFERENCE to 4 for the fourth note */
    }
    else if (beam3.freq >= 281 && beam3.freq <= 460) { /* Range for the fifth note */
      beam3.difference = 5; /* Set DIFFERENCE to 5 for the fifth note */
    }
    else if (beam3.freq >= 281 && beam3.freq <= 750) { /* Range for the sixth note */
      beam3.difference = 6; /* Set DIFFERENCE to 6 for the sixth note */
    }
    else if (beam3.freq >= 751) { /* Range for the seventh note */
      beam3.difference = 7; /* Set DIFFERENCE to 7 for the seventh note */
    } /* End if */
  } /* End if */

  else if (beam4.note != 0) { /* If the fourth beam is broken */
    /* Take four successive reads from channel 1 of the ADC */
    beam4.freqReads[0] = analogRead (A3);
    beam4.freqReads[1] = analogRead (A3);
    beam4.freqReads[2] = analogRead (A3);
    beam4.freqReads[3] = analogRead (A3);

    /* Sum the four reads together */
    beam4.freq = beam4.freqReads[0];
    beam4.freq += beam4.freqReads[1];
    beam4.freq += beam4.freqReads[2];
    beam4.freq += beam4.freqReads[3];

    /* Now find the arithmetic mean of the reads */
    beam4.freq /= 4;

    if (beam4.freq <= 30) { /* Range for the first note */
      beam4.difference = 1; /* Set DIFFERENCE to 1 for the first note */
    }
    else if (beam4.freq >= 31 && beam4.freq <= 60) { /* Range for the second note */
      beam4.difference = 2; /* Set DIFFERENCE to 2 for the second note */
    }
    else if (beam4.freq >= 61 && beam4.freq <= 120) { /* Range for the third note */
      beam4.difference = 3; /* Set DIFFERENCE to 3 for the third note */
    }
    else if (beam4.freq >= 121 && beam4.freq <= 280) { /* Range for the fourth note */
      beam4.difference = 4; /* Set DIFFERENCE to 4 for the fourth note */
    }
    else if (beam4.freq >= 281 && beam4.freq <= 460) { /* Range for the fifth note */
      beam4.difference = 5; /* Set DIFFERENCE to 5 for the fifth note */
    }
    else if (beam4.freq >= 281 && beam4.freq <= 750) { /* Range for the sixth note */
      beam4.difference = 6; /* Set DIFFERENCE to 6 for the sixth note */
    }
    else if (beam4.freq >= 751) { /* Range for the seventh note */
      beam4.difference = 7; /* Set DIFFERENCE to 7 for the seventh note */
    } /* End if */
  } /* End if */

  else if (beam5.note != 0) { /* If the fifth beam is broken */
    /* Take four successive reads from channel 1 of the ADC */
    beam5.freqReads[0] = analogRead (A4);
    beam5.freqReads[1] = analogRead (A4);
    beam5.freqReads[2] = analogRead (A4);
    beam5.freqReads[3] = analogRead (A4);

    /* Sum the four reads together */
    beam5.freq = beam5.freqReads[0];
    beam5.freq += beam5.freqReads[1];
    beam5.freq += beam5.freqReads[2];
    beam5.freq += beam5.freqReads[3];

    /* Now find the arithmetic mean of the reads */
    beam5.freq /= 4;

    if (beam5.freq <= 30) { /* Range for the first note */
      beam5.difference = 1; /* Set DIFFERENCE to 1 for the first note */
    }
    else if (beam5.freq >= 31 && beam5.freq <= 60) { /* Range for the second note */
      beam5.difference = 2; /* Set DIFFERENCE to 2 for the second note */
    }
    else if (beam5.freq >= 61 && beam5.freq <= 120) { /* Range for the third note */
      beam5.difference = 3; /* Set DIFFERENCE to 3 for the third note */
    }
    else if (beam5.freq >= 121 && beam5.freq <= 280) { /* Range for the fourth note */
      beam5.difference = 4; /* Set DIFFERENCE to 4 for the fourth note */
    }
    else if (beam5.freq >= 281 && beam5.freq <= 460) { /* Range for the fifth note */
      beam5.difference = 5; /* Set DIFFERENCE to 5 for the fifth note */
    }
    else if (beam5.freq >= 281 && beam5.freq <= 750) { /* Range for the sixth note */
      beam5.difference = 6; /* Set DIFFERENCE to 6 for the sixth note */
    }
    else if (beam5.freq >= 751) { /* Range for the seventh note */
      beam5.difference = 7; /* Set DIFFERENCE to 7 for the seventh note */
    } /* End if */
  } /* End if */

  else if (beam6.note != 0) { /* If the sixth beam is broken */
    /* Take four successive reads from channel 1 of the ADC */
    beam6.freqReads[0] = analogRead (A5);
    beam6.freqReads[1] = analogRead (A5);
    beam6.freqReads[2] = analogRead (A5);
    beam6.freqReads[3] = analogRead (A5);

    /* Sum the four reads together */
    beam6.freq = beam6.freqReads[0];
    beam6.freq += beam6.freqReads[1];
    beam6.freq += beam6.freqReads[2];
    beam6.freq += beam6.freqReads[3];

    /* Now find the arithmetic mean of the reads */
    beam6.freq /= 4;

    if (beam6.freq <= 30) { /* Range for the first note */
      beam6.difference = 1; /* Set DIFFERENCE to 1 for the first note */
    }
    else if (beam6.freq >= 31 && beam6.freq <= 60) { /* Range for the sixth note */
      beam6.difference = 2; /* Set DIFFERENCE to 2 for the sixth note */
    }
    else if (beam6.freq >= 61 && beam6.freq <= 120) { /* Range for the third note */
      beam6.difference = 3; /* Set DIFFERENCE to 3 for the third note */
    }
    else if (beam6.freq >= 121 && beam6.freq <= 280) { /* Range for the fourth note */
      beam6.difference = 4; /* Set DIFFERENCE to 4 for the fourth note */
    }
    else if (beam6.freq >= 281 && beam6.freq <= 460) { /* Range for the fifth note */
      beam6.difference = 5; /* Set DIFFERENCE to 5 for the fifth note */
    }
    else if (beam6.freq >= 281 && beam6.freq <= 750) { /* Range for the sixth note */
      beam6.difference = 6; /* Set DIFFERENCE to 6 for the sixth note */
    }
    else if (beam6.freq >= 751) { /* Range for the seventh note */
      beam6.difference = 7; /* Set DIFFERENCE to 7 for the seventh note */
    } /* End if */
  } /* End if */
} /* End ISR */
