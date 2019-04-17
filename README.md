# README
This is the software used to implement a laser harp. A string can be implemented by creating an instance of the Beam class. By creating more instances of the class, an arbitrary number of beams can be implemented. This program was written for the Atmel ATMega2560 microcontroller.

## Program Overview
The function of the program is quite simple. An instance of the Beam class is created for each laser being used. The laser shines on a photoresistor which is part of a voltage divider along with a 10k resistor. The 10k resistor is connected between 5V and a digital input pin. The voltage at the digital pin will be the same as the voltage across the photoresistor. Depending on the voltage at the digital input pin, the MCU can determine if the laser is active (broken) or not. If the voltage is low (< 1V), then the beam is not broken and nothing should happen. If the voltage is greater than 1V, then the beam is broken.

The program is interrupt driven, meaning that each digital input will be configured as an external interrupt that triggers whenever the voltage on the digital input pin changes. When the voltage changes from LOW to HIGH, a flag will be toggled from 0 to 1 and a hardware timer will be configured to create a 100 ms delay. It will then take one read from the ADC channel corresponding to the beam that was interrupted. When the voltage changes from HIGH to LOW, then the timer is disabled and the flag is set back to zero.

The values from the ADC are used for determining the note that will be played. Each beam will have a range on one octave. Since the ADC returns a 10 bit value and the ATMega2560 is an 8 bit microcontroller, the returned values have to be stored in a 16 bit variable. This can cause problems when comparing 16 bit values when interrupts are enabled, since the ATMega2560 can only compare 8 bits at a time. If an interrupt occurs after the first byte was compared but before the second byte is compared, irregular behavior can occur. To solve this problem, an 8 bit value is used to tell the MCU which note it needs to play depending on what the value returned from the ADC is. Each of the notes that can be played will have their own range of ADC values, so the 8 bit variable takes the value of the note within whose range the ADC value falls.

The hardware timer being used to create a delay is configured to run in compare mode. This mode counts between the numbers specified in TCNT1 and OCR1. The rate of the timer compared to the system clock is selected with bits 2:0 of TCCR1B. Whenever a timer interrupt occurs, four successive reads are taken from the ADC and then averaged to get a more accurate reading.

## Functions
The functions used and their functions are as follows:

<ul>
<li> Setup:  Configures the I/O pins, timer, interrupts, and sleep mode.
<li> Loop:  This is where the notes are actually played. The program will check if a beam is broken and if one is, then it will determine which note to play and play it using the Tone function, which is an Arduino function.
<li> analogRead (pin):  Provides a high-level interface to the MCU's ADC. It's argument is the number of the analog pin from which the voltage is read and then fed to its channel of the ADC.
<li> attachInterrupt (pin, ISR, mode):  Provides an easy way to attach external interrupts to digital input pins. The first argument is the pin the interrupt is to be attached to. The second argument is the function that will be used as an ISR whenever an interrupt occurs on that pin. The third argument is used to determine when the interrupt should be triggered. This can either be a low level or rising or falling edge. It can also be set to trigger on both a rising and falling edge.
<li> tone (pin, frequency, duration):  Generates a square wave with a 50% duty cycle at a specified frequency. The first argument is the pin on which the signal should be generated. The second pin is the desired frequency of the signal in hertz. The third argument is optional and specifies the desired duration of the signal in miliseconds.
<li> noTone (pin):  Used to kill the note being generated on the specified pin by the tone function, since the tone function can only be used to genertate one note at a time. This function is called when a new note needs to be played or the currently active beam is no longer broken.
<li> playBeamx:  Interrupt Service Routine (ISR) for the beam. Each beam gets its own version, where x is the beam number the function is associated with. It toggles the flag and takes an initial reading from the ADC and enable the timer if the beam is broken or disable the timer and set the sleep enable bit if the beam is no longer broken.
<li> Timer 1 ISR:  ISR for Timer 1. It will check which beam is broken, then take four reads from the ADC and find the average of the reads.
</ul>

## Beam Class
An instance of the Beam class is created for each beam that is used. It consists of the following variables:

<ul>
<li> note:  A volatile 8-bit integer that is used as a flag for determining if the beam is active (broken).
<li> freq:  A volatile 16-bit integer that holds the average of the four ADC reads. It is set to -1 when the beam is inactive .
<li> freqReads[4]:  A 16-bit integer array that holds the values of the four ADC reads. These values are averaged and stored in FREQ.
<li> difference:  A volatile 8-bit integer used as a workaround to create atomic conditionals in the LOOP function. Since the ATMega2560 is an 8 bit MCU, problems can arise when comparing 16 bit values with interrupts enabled. Using an 8-bit value eliminates this risk.
</ul>

It is possible for the behavior of the system to become inverted when comparing 16 bit values. This means that a note will be played when the beam is not broken and stop when it is broken.
