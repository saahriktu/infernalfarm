/*
 * Infernal farm - game where you must catch radioactive eggs on
 * farm near atomic electrostation. 3 missed eggs will cause atomic explode
 * Copyright (C) 2010 Kurashov Artem Konstantinovich
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <curses.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <menu.h>

static WINDOW *gw;		// Game Window

static int eggx,		// Egg x
  eggdx,			// Egg dx
  eggy;				// Egg y

// Get random number
static int
get_random (int g_rand_max)
{
  return (int) ((g_rand_max + 1.0) * rand () / (RAND_MAX + 1.0));
}

// Put bird
static void
putbird (int dy, int dx)
{
  int i;
  mvwaddstr (gw, dy, (COLS - 7) * dx, "-------");
  for (i = 0; i < 4; i++)
    mvwaddch (gw, dy + 1 + i, (COLS - 15 - 2 * i) * dx + 7 + i, 92 - 45 * dx);
}

// Change y of egg or not
static void
nexteggy ()
{
  if (eggx < 7)
    return;
  if (eggx >= 7)
    eggy++;
  return;
}

// Check fall egg or not
static int
eggfallchk ()
{
  if (!eggdx)
    wmove (gw, eggy + 1, eggx);
  else
    wmove (gw, eggy + 1, COLS - 1 - eggx);
  if (winch (gw) == 32)
    return 0;
  else
    return 1;
}

int
main (int argc, char **argv)
{
//Variables
  int lifez = 3,		// Lifes
    bx,				// Basket x
    by,				// Basket y
    eggnotlive = 1;		// Egg death flag
  long scorez = 0,		// Scores
    gtype = 1;			// Game type
  char key0;			// User pressed key
  MENU *gtcm;			// Game Type Choose Menu
  ITEM *gtcmi0[3],		// gtcm Items
   *icur;			// Item CURrent
//Init
  //Menu items
  gtcmi0[0] = new_item ("Game A", "(Easy)");
  gtcmi0[1] = new_item ("Game B", "(Hard)");
  gtcmi0[2] = new_item (NULL, "");
  // Set random number generator by clock
  srand (time (0));
  //Curses init
  initscr ();
  clear ();
  noecho ();
  curs_set (0);
  cbreak ();
  keypad (stdscr, TRUE);
//Menu (Choose game type)
  //Create windows
  gw = newwin (4, 30, LINES / 2 - 2, COLS / 2 - 15);
  //Menu init
  gtcm = new_menu ((ITEM **) gtcmi0);
  set_menu_win (gtcm, gw);
  set_menu_sub (gtcm, derwin (gw, 2, 16, 1, 7));
  set_menu_mark (gtcm, "->");
  //Decorate
  box (gw, 0, 0);
  //Final
  post_menu (gtcm);
  mvwaddstr (gw, 0, 8, "Infernal farm");
  mvwaddstr (gw, 3, 3, "j-down,k-up,Enter-choose");
  //Draw it!
  wsyncup (gw);

  //Menu loop
  for (;;)
    {
      key0 = wgetch (gw);	// Store pressed key
      switch (key0)
	{			// Analyse pressed key
	case 'j':		// Down
	  menu_driver (gtcm, REQ_DOWN_ITEM);
	  break;
	case 'k':		// Up
	  menu_driver (gtcm, REQ_UP_ITEM);
	  break;
	case 10:		// Enter - choose accept
	  icur = current_item (gtcm);
	  if (icur->index == 0)
	    gtype = 45000;
	  else
	    gtype = 0;
	}
      wrefresh (gw);
      if (gtype != 1)
	break;
    }
  //Remove Menu
  delwin (gw);

//Main game code
  //Create game window
  gw = newwin (LINES, COLS, 0, 0);
  //Put birds
  putbird (10, 0);
  putbird (16, 0);
  putbird (10, 1);
  putbird (16, 1);
  nodelay (gw, TRUE);
  // Game loop
  for (;;)
    {
      // Draw it!
      mvwprintw (gw, 0, 0, "Lifes: %d", lifez);
      mvwprintw (gw, 0, COLS - 7, "       ", scorez);
      mvwprintw (gw, 0, COLS - 14, "Scores: %ld", scorez);
      wsyncup (gw);
      // If egg absent - create new one
      if (eggnotlive)
	{
	  eggdx = get_random (1);
	  eggx = 0;
	  eggy = 6 * get_random (1) + 9;
	  eggnotlive = 0;
	}
      // Draw egg
      if (!eggdx)
	mvwaddch (gw, eggy, eggx, 'O');
      else
	mvwaddch (gw, eggy, COLS - 1 - eggx, 'O');
      // Read user pressed key
      key0 = wgetch (gw);
      // Analyse user pressed key
      switch (key0)
	{
	case 'h':		// left-down
	  mvwaddch (gw, by, bx, ' ');
	  bx = 11;
	  by = 21;
	  mvwaddch (gw, by, bx, 'U');
	  break;
	case 'j':		// left-up
	  mvwaddch (gw, by, bx, ' ');
	  bx = 11;
	  by = 15;
	  mvwaddch (gw, by, bx, 'U');
	  break;
	case 'k':		// right-up
	  mvwaddch (gw, by, bx, ' ');
	  bx = COLS - 12;
	  by = 15;
	  mvwaddch (gw, by, bx, 'U');
	  break;
	case 'l':		// right-down
	  mvwaddch (gw, by, bx, ' ');
	  bx = COLS - 12;
	  by = 21;
	  mvwaddch (gw, by, bx, 'U');
	}
      // Erase egg
      if (!eggdx)
	mvwaddch (gw, eggy, eggx, ' ');
      else
	mvwaddch (gw, eggy, COLS - 1 - eggx, ' ');
      // Move egg
      eggx++;			// Next x
      nexteggy ();		// Next y
      // If x of egg maximal then it will fall to floor or basket
      if (eggx == 11)
	{
	  if (eggfallchk ())
	    {
	      scorez += 10;	// Catched!
	    }
	  else
	    lifez--;		// Egg broken
	  if (lifez == 0)
	    break;
	  eggnotlive = 1;	// Let's go next egg!
	}
      // Wait
      usleep (55000 + gtype - scorez / 4);
      // If scores maximal clear it
      if (scorez > 100000)
	scorez = 0;
    }
  endwin ();
  return 0;
}
