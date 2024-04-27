#include <curses.h>

typedef struct _new_win_t
{
  int nlines, ncols;
  int begin_y, begin_x;
  WINDOW *win;
} new_win_t;

int
main ()
{
  initscr ();

  new_win_t my_win = { 10, 20, 5, 5, NULL };

  my_win.win
      = newwin (my_win.nlines, my_win.ncols, my_win.begin_y, my_win.begin_x);
  refresh ();

  box (my_win.win, 0, 0);
  mvwprintw (my_win.win, 0, 1, "h4x0r t00l");
  mvwprintw (my_win.win, 1, 1, "Line1");
  mvwprintw (my_win.win, 2, 1, "Line2");
  mvwprintw (my_win.win, 3, 1, "Line3");
  mvwprintw (my_win.win, 4, 1, "Line4");

  wrefresh (my_win.win);

  refresh ();
  getch ();
  endwin ();

  return 0;
}
