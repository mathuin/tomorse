/* tomorse.c - 20030205 - JMT */

/* Copyright (c) 2003 Jack Twilley <jmt@twilley.org> */

/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the */
/* "Software"), to deal in the Software without restriction, including */
/* without limitation the rights to use, copy, modify, merge, publish, */
/* distribute, sublicense, and/or sell copies of the Software, and to */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions: */

/* The above copyright notice and this permission notice shall be */
/* included in all copies or substantial portions of the Software. */

/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND */
/* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE */
/* LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION */
/* OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION */
/* WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

/* This program converts text into Morse code and stores the resulting
   data in 44.1kHz 16-bit stereo WAV files. */ 

/* 2003Dec19 -- JMT -- fixed Farnsworth settings.
   Thanks to Stanley M. Adams who emailed me to tell me I was lazy. :-) */

/* includes */
#include <stdio.h>
#include <math.h>
#include <sndfile.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>

/* defines */
#define PARIS_LENGTH 50
#define SECS_PER_MIN 60
#define SAMPLE_RATE 44100
#define CHANNELS 2
#define DEFAULT_SPEED 13
#define DEFAULT_CSPEED 20
#define DEFAULT_PITCH 880

#define MAX_AMPLITUDE 0.7
#define SLOPE 0.005 /* seconds to reach full volume */

/* constants */
double grade = SAMPLE_RATE * SLOPE * MAX_AMPLITUDE;
/* values below lifted from tomorse/morse.c */
/* adding @ as per ITU in 2004 */
char *charstr = "abcdefghijklmnopqrstuvwxyz0123456789.,?:;-/\"+|>~=@";
#define NCHARS 50
char *morse[NCHARS] = {
  ".-",
  "-...",
  "-.-.",
  "-..",
  ".",
  "..-.",
  "--.",
  "....",
  "..",
  ".---",
  "-.-",
  ".-..",
  "--",
  "-.",
  "---",
  ".--.",
  "--.-",
  ".-.",
  "...",
  "-",
  "..-",
  "...-",
  ".--",
  "-..-",
  "-.--",
  "--..",
  "-----",
  ".----",
  "..---",
  "...--",
  "....-",
  ".....",
  "-....",
  "--...",
  "---..",
  "----.",
  ".-.-.-",
  "--..--",
  "..--..",
  "---...",
  "-.-.-.",
  "-....-",
  "-..-.",
  ".-..-.",
  ".-.-.",
  ".-...",
  "-.--.",
  "...-.-",
  "-...-",
  ".--.-."
};

/* functions */

/* main */
int main(int argc, char **argv) {
  
  int speed = DEFAULT_SPEED;
  int cspeed = DEFAULT_CSPEED;
  int pitch = DEFAULT_PITCH;
  int spaceflag = 1;
  int lendit, lendah, lenies, lenics, leniws, farndit, index, c;
  double ta;
  double *dit, *dah, *ies, *ics, *iws;
  SF_INFO *sfinfo;
  SNDFILE *sfd;
  sf_count_t sfc;
  char *charptr;
  
  while ((c = getopt(argc, argv, "p:c:s:")) != -1)
    switch (c) {
    case 'p':
      pitch = atoi(optarg);
      break;
    case 'c':
      cspeed = atoi(optarg);
      break;
    case 's':
      speed = atoi(optarg);
      break;
    case '?':
    default:
      fprintf(stderr, "usage: %s [-p <pitch>] [-c <character speed>] [-s <word speed>]\n",
	      argv[0]);
      fprintf(stderr, "       default values are:\n");
      fprintf(stderr, "         pitch - %d Hz\n", DEFAULT_PITCH);
      fprintf(stderr, "         character speed - %d wpm\n", DEFAULT_CSPEED);
      fprintf(stderr, "         word speed - %d wpm\n", DEFAULT_SPEED);
      exit(1);
    }

  /* cspeed cannot be lower than speed */
  if (cspeed < speed)
    cspeed = speed;

  /* how long is a dit */
  lendit = SAMPLE_RATE * SECS_PER_MIN / (PARIS_LENGTH * cspeed);
  lendah = lendit * 3;

  /* how long are our spaces? */
  /* See http://www.arrl.org/files/infoserv/tech/code-std.txt 
     for more info on this equation */
  if (cspeed > speed) {
    ta = 60 / (double) speed - 37.2 / (double) cspeed;
    farndit = (int) (SAMPLE_RATE * ta) / 19;
  } else {
    farndit = SAMPLE_RATE * SECS_PER_MIN / (PARIS_LENGTH * speed);
  }
  lenies = lendit;
  lenics = farndit * 3;
  leniws = farndit * 7;

  /* construct arrays */
  dit = (double *)malloc(CHANNELS * lendit * sizeof(double));
  for (index = 0; index < lendit; index++)
    for (c = 0; c < CHANNELS; c++) {
      dit[(CHANNELS * index) + c] = 
	MAX_AMPLITUDE * sin(pitch*M_PI*2*index/SAMPLE_RATE);
      if (index < grade)
	dit[(CHANNELS * index) + c] *= (index / grade);
      if (index > (lendit - grade))
	dit[(CHANNELS * index) + c] *= ((lendit - index) / grade);
    }

  dah = (double *)malloc(CHANNELS * lendah * sizeof(double));
  for (index = 0; index < lendah; index++)
    for (c = 0; c < CHANNELS; c++) {
      dah[(CHANNELS * index) + c] = 
	MAX_AMPLITUDE * sin(pitch*M_PI*2*index/SAMPLE_RATE);
      if (index < grade)
	dah[(CHANNELS * index) + c] *= (index / grade);
      if (index > (lendah - grade))
	dah[(CHANNELS * index) + c] *= ((lendah - index) / grade);
    }

  /* ies is same length as a dit */
  ies = (double *)malloc(CHANNELS * lenies * sizeof(double));
  for (index = 0; index < lenies; index++)
    for (c = 0; c < CHANNELS; c++) 
      ies[(CHANNELS * index) + c] = 0.0;

  /* ics is three "farndits" in length */
  ics = (double *)malloc(CHANNELS * lenics * sizeof(double));
  for (index = 0; index < lenics; index++)
    for (c = 0; c < CHANNELS; c++) 
      ics[(CHANNELS * index) + c] = 0.0;

  /* iws is seven "farndits" in length */
  iws = (double *)malloc(CHANNELS * leniws * sizeof(double));
  for (index = 0; index < leniws; index++)
    for (c = 0; c < CHANNELS; c++) 
      iws[(CHANNELS * index) + c] = 0.0;

  /* set up file */
  sfinfo = (SF_INFO *)malloc(sizeof(SF_INFO));

  sfinfo->samplerate = SAMPLE_RATE;
  sfinfo->channels = CHANNELS;
  sfinfo->format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;

  sfd = sf_open("-", SFM_WRITE, sfinfo);

  /* for every character on stdin */
  for (c = tolower(getchar()); !feof(stdin); c = tolower(getchar())) {

    /* only write iws if whitespace follows non-whitespace */
    if (isspace(c) && spaceflag == 0) {
      sfc = sf_writef_double(sfd, iws, leniws);
      spaceflag = 1;
    } else {
      /* check if it's in the list */
      for (index = 0; index < NCHARS; index++)
	if (c == charstr[index])
	  break;

      /* if it's in the list */
      if (c == charstr[index]) {
	if (spaceflag == 0)
	  sfc = sf_writef_double(sfd, ics, lenics);
	else
	  spaceflag = 0;
	for (charptr = morse[index]; *charptr != NULL; charptr++) {
	  if (*charptr == '.')
	    sfc = sf_writef_double(sfd, dit, lendit);
	  else
	    sfc = sf_writef_double(sfd, dah, lendah);
	  if (*(charptr+1) != NULL)
	    sfc = sf_writef_double(sfd, ies, lenies);
	}
      }
    }
  }

  /* close the file */
  sf_close(sfd);

}
