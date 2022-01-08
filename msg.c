#include <graph.h> /* color */

#include <string.h> /* strlen */

void
print_title(const char *title)
{
  /* use conio to print a title line */

  _setbkcolor(3); /* cyan */
  _settextcolor(15); /* br white */

  _settextwindow(1,1, 1,80);
  _clearscreen(_GWINDOW);

  _settextposition( 1, 40-(strlen(title)/2) );
  _outtext(title);
}

void
print_message(const char *msg)
{
  /* use conio to print a message line */

  _setbkcolor(3); /* cyan */
  _settextcolor(15); /* br white */

  _settextwindow(25,1, 25,80);
  _clearscreen(_GWINDOW);

  _settextposition(1,2);
  _outtext(msg);
}
