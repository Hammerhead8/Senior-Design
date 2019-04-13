/* This file is part of the project "Program"
 * This file contains the frequencies for the notes
 * The notes here for a C major scale starting from C2
 * ranging up to B7. */
#ifndef NOTES_H
#define NOTES_H

enum
firstOctave
{
  NOTE1_1 = 65,
  NOTE2_1 = 73,
  NOTE3_1 = 82,
  NOTE4_1 = 87,
  NOTE5_1 = 98,
  NOTE6_1 = 110,
  NOTE7_1 = 123
};

enum
secondOctave
{
  NOTE1_2 = 262,
  NOTE2_2 = 294,
  NOTE3_2 = 330,
  NOTE4_2 = 349,
  NOTE5_2 = 392,
  NOTE6_2 = 440,
  NOTE7_2 = 494
};

enum
thirdOctave
{
  NOTE1_3 = 523,
  NOTE2_3 = 587,
  NOTE3_3 = 659,
  NOTE4_3 = 698,
  NOTE5_3 = 784,
  NOTE6_3 = 880,
  NOTE7_3 = 988
};

enum
fourthOctave
{
  NOTE1_4 = 523,
  NOTE2_4 = 587,
  NOTE3_4 = 659,
  NOTE4_4 = 698,
  NOTE5_4 = 784,
  NOTE6_4 = 880,
  NOTE7_4 = 988
};

enum
fifthOctave
{
  NOTE1_5 = 1047,
  NOTE2_5 = 1175,
  NOTE3_5 = 1319,
  NOTE4_5 = 1397,
  NOTE5_5 = 1568,
  NOTE6_5 = 1760,
  NOTE7_5 = 1967
};

enum
sixthOctave
{
  NOTE1_6 = 2093,
  NOTE2_6 = 2349,
  NOTE3_6 = 2637,
  NOTE4_6 = 2794,
  NOTE5_6 = 3136,
  NOTE6_6 = 3520,
  NOTE7_6 = 3951
};

#endif
