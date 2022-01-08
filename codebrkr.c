#include <stdio.h>
#include <stdlib.h> /* rand */
#include <ctype.h> /* toupper */
#include <string.h> /* strlen */
#include <time.h> /* time function */

#include <conio.h> /* console */
#include <graph.h> /* color */
#include <i86.h> /* delay */

#include "msg.h"

#define BLACK 0
#define GREEN 2
#define PURPLE 5
#define YELLOW 6

#define CODELEN 5
#define GUESSES 5

void
randomize_str(char *str)
{
  int ch;
  int len;
  int swap;
  char tmp;

  len = strlen(str);

  srand( time(NULL) );

  /* simple shuffle of an array */

  /* THIS IS NOT A SECURE SHUFFLE, but we're just using this simple
     method to shuffle a string for a game, so it doesn't need to be
     very cryptographically secure. */

  for (ch = 0; ch < len; ch++) {
    /* pick a random other letter and swap this char for that one */

    swap = rand() % len;

    tmp = str[swap];

    str[swap] = str[ch];
    str[ch] = tmp;
  }
}

char *
read_guess(char *guess, int len)
{
  int ltr;
  int ch;

  /* use conio to read a string of length "len" */

  _settextcolor(14); /* br yellow */
  _setbkcolor(4); /* red */

  _settextwindow(3,2, 6,22); /* define the window: upper left */
  _clearscreen(_GWINDOW); /* just the window */

  /* print the prompt */

  _settextposition(2,5); /* centered in 20-col wide box, line 2 */
  _outtext("Make a guess:");

  _settextposition(3,11-(len/2)); /* centered in 20-col wide box, line 3 */
  for (ltr = 0; ltr < len; ltr++) {
    putch(0xfa); /* middot */
  }

  /* read the guess */

  ltr = 0;
  _settextposition(3,11-(len/2)); /* reset cursor */

  do {
    ch = getch();

    if (ch == 0) {
      getch(); /* clear the ext key */
    }

    /* only numbers */

    if ( isdigit(ch) ) {
      _settextposition(3,ltr + 11-(len/2)); /* position cursor */
      putch(ch);

      guess[ltr++] = ch;
    }

    /* if backspace */

    if (ch == 8) { /* BS */
      guess[ltr--] = '\0';

      if (ltr < 0) {
	ltr = 0; /* prevent underflow */
      }

      _settextposition(3,ltr + 11-(len/2)); /* erase prev letter */
      putch(0xfa); /* middot */
      _settextposition(3,ltr + 11-(len/2)); /* re-position cursor */
    }
  } while (ltr < len);

  return guess;
}

void
print_letter(char letter, int color, int lettertop, int letterleft)
{
  char *p;

  p = &letter; /* pointer assignment */

  /* use conio to display the letter in a color */

  _setbkcolor(color);
  _settextcolor(15); /* br white */

  /* this character box starts at top,left */
  /* this is row,col coordinates .. note that rows 1,2,3 are 3 rows high */

  _settextwindow(lettertop, letterleft, lettertop+2, letterleft+2);
  _clearscreen(_GWINDOW);

  _settextposition(2,2);
  _outmem(p, 1);

  /* slight pause so we get a kind of animation effect */

  delay(100);
}

void
print_secret(const char *secret, int len)
{
  int num;

  /* place this over the "guess" window */

  _settextcolor(0); /* black */
  _setbkcolor(7); /* white */
  _settextwindow(3,2, 9,22);
  _clearscreen(_GWINDOW);

  _settextposition(2,4);
  _outtext("The secret code:");

  for (num = 0; num < len; num++) {
    print_letter(secret[num], PURPLE, 6, 3+(num*4));
  }
}

void
print_help(void)
{
  /* help window on the right side */

  _setbkcolor(7); /* white */
  _settextcolor(0); /* black */
  _settextwindow(3,56, 22,79);
  _clearscreen(_GWINDOW);

  /* print the title and help text */

  _settextposition(2,7);
  _outtext("HOW TO PLAY");

  _settextposition(4,2);
  _outtext("You have 5 attempts to");
  _settextposition(5,2);
  _outtext("guess a random 5-digit");
  _settextposition(6,2);
  _outtext("code. Each number is");
  _settextposition(7,2);
  _outtext("used only once.");

  _settextposition(10,6);
  _outtext("Correct");

  _settextposition(14,6);
  _outtext("Correct but not in");
  _settextposition(15,6);
  _outtext("the right position");

  _settextposition(18,6);
  _outtext("Incorrect");

  /* print the sample digits .. these have a slight delay in them to make
     a kind of animation effect. we'll keep the animation here */

  print_letter('1', GREEN,  11, 57);
  print_letter('2', YELLOW, 15, 57);
  print_letter('3', BLACK,  19, 57);
}

int
compare_words(const char *word, const char *guess, int wordtop, int wordleft)
{
  int matches = 0;
  int ltr;
  int pos;
  int color;

  /* compare two strings and display the differences */

  for (ltr = 0 ; (word[ltr]) && (guess[ltr]) ; ltr++) {
    if (word[ltr] == guess[ltr]) {
      /* same letter */

      color = GREEN;
      print_letter( guess[ltr], color, wordtop, wordleft+(ltr*4) );

      matches++;
    }
    else {
      /* does this letter appear elsewhere? */

      color = BLACK;

      for (pos = 0; word[pos]; pos++) {
	if (word[pos] == guess[ltr]) {
	  color = YELLOW;
	}
      } /* for */

      print_letter( guess[ltr], color, wordtop, wordleft+(ltr*4) );
    }
  } /* for */

  return matches;
}

int
main()
{
  char guess[CODELEN+1]; /* the player's guess, with '\0' */

  char secret[] = "1234567890";

  int guessnum;
  int matches = 0;

  /* set up the screen */

  if (_setvideomode(_TEXTC80) == 0) {
    puts("Cannot set video mode: 80x25 color");
    return 1;
  }

  _setbkcolor(1); /* blue */
  _clearscreen(_GCLEARSCREEN); /* whole screen */

  print_title("CODEBREAKER - a number puzzle game");
  print_message("");

  /* show help message on right side of screen */

  print_help();

  /* randomize the code */

  randomize_str(secret);
  secret[CODELEN] = '\0'; /* truncate */

  /* let the user make 5 guesses */

  for (guessnum = 0; (guessnum < GUESSES) && (matches < CODELEN); guessnum++) {
    read_guess(guess, CODELEN); /* read the guess */

    /* assume boxes are 3x3 plus 1 col (or plus 1 row) between them */

    /* if 5 digits, starting at line 4 & col 30 more or less centers
       the boxes on the screen */

    matches = compare_words(secret, guess, 4+(guessnum*4), 30);
  }

  /* reveal the secret code */

  print_secret(secret, CODELEN);

  /* print win or lose */

  if (matches == CODELEN) {
    print_message("You guessed the secret code!");
  }
  else {
    print_message("Sorry, you didn't guess the code");
  }

  /* done */

  _outtext(" \263 press any key to exit"); /* (vert bar) append to text */
  if (getch() == 0) {
    getch(); /* call getch again to clear the ext character */
  }

  _setvideomode(_DEFAULTMODE);

  return 0;
}
