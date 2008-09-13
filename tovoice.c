/* tovoice - 20061024 - JMT */

/* Copyright (c) 2006 Jack Twilley <jmt@twilley.org> */

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

/* This program converts text into its component characters and stores
   the resulting data in 44.1kHz 16-bit stereo WAV files. */

/* includes */
#include <stdio.h>
#include <sndfile.h>
#include <string.h>
#include <unistd.h>

/* defines */
#define DEFAULT_CHARDIR "."
#define CHARSTRLEN 37
#define SUFSTRLEN 6
#define SAMPLE_RATE 44100
#define CHANNELS 2
#define SAMPLE_LEN 2 /* two seconds max sample length */
#define BUFFER_LEN (SAMPLE_RATE * CHANNELS * SAMPLE_LEN)
#define MAXPATHLEN 80

/* constants */
/* from tomorse.c with a leading space minus the ones I don't have! */
char *charstr = " ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
char *sufstr = "/0.wav"; // basic suffix
/* functions */

/* main */
int main(int argc, char **argv) {
  char chardir[MAXPATHLEN];
  char wavpath[MAXPATHLEN+SUFSTRLEN];
  int c, index;
  float charwav[CHARSTRLEN][BUFFER_LEN];
  int wavlen[CHARSTRLEN];
  
  SNDFILE *sndfile;
  SF_INFO sfinfo;

  /* set default values */
  strcpy(chardir, DEFAULT_CHARDIR);

  /* deal with arguments */
  while ((c = getopt(argc, argv, "d:")) != -1)
    switch (c) {
    case 'd':
      strcpy(chardir, optarg);
      break;
    case '?':
    default:
      fprintf(stderr, "usage: %s [-d <directory>]\n", argv[0]);
      exit(1);
    }

  /* read in all the letters and numbers from the wav files */
  strcpy(wavpath, chardir);
  strcat(wavpath, sufstr);

  for (index = 0; index < CHARSTRLEN; index++) {
    /* build a filename *chardir/*charstr.wav */
    wavpath[strlen(chardir)+1] = charstr[index];
    printf("new file name is %s\n", wavpath);
    /* open that file and read in all the doubles */
    if ((sndfile = sf_open(wavpath, SFM_READ, &sfinfo)) == NULL) {
      puts(sf_strerror(NULL));
      exit(1);
    }
    wavlen[index] = sf_read_float(sndfile, charwav[index], BUFFER_LEN);
    if (wavlen[index] == BUFFER_LEN)
      fprintf(stderr, "%s: sound sample too long\n", wavpath);
    sf_close(sndfile);
  }

  /* open audio file */
  sfinfo.samplerate = SAMPLE_RATE;
  sfinfo.channels = CHANNELS;
  sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;

  sndfile = sf_open("-", SFM_WRITE, &sfinfo);
  
  /* for every character in text file */
  for (c = toupper(getchar()); !feof(stdin); c = toupper(getchar())) {

    /* if it's in the array, append that sound file to the out file */
    for (index = 0; index < CHARSTRLEN; index++)
      if (c == charstr[index])
	sf_writef_float(sndfile, charwav[index], wavlen[index]);
  }

  /* close audio file */
  sf_close(sndfile);
}
