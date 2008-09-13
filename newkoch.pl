#!/usr/bin/perl

# Copyright (c) 2003 Jack Twilley <jmt@twilley.org>

# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
# 
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

# Generate ten lines of ten five-letter groups of characters
# available from Koch method list.

# This script takes one argument -- a number between 2 and 43 representing
# how many characters from the letters array should be used.
# To generate text using K and M, use an argument of 2.
# To generate text using K, M and R, use an argument of 3.
# To generate text using all characters, use an argument of 43.

# constants
@letters = split(//,'KMRSUAPTLOWI.NJEF0Y,VG5/Q9ZH38B?427C1D6X=~+');
$numlines = 10;
$numgroups = 10;
$numchars = 5;

# variables
$progress = shift;

# header
for $char (1 .. $numchars) {
  print STDOUT $letters[$progress - 1];
}
print STDOUT " \n";

# routine
for $line (1 .. $numlines) {
  for $group (1 .. $numgroups) {
    for $char (1 .. $numchars) {
      print STDOUT $letters[rand $progress];
    }
    print STDOUT " ";
  }
  print STDOUT "\n";
}

# trailer
for $char (1 .. $numchars) {
  print STDOUT $letters[$progress];
}
print STDOUT " \n";
