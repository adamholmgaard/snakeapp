#include <iostream>
#include <ncurses.h>
#include <string>
#include <map>
#include <algorithm>
using namespace std;

int sscalls;
int napples;

enum Direction { left, right, up, down };

enum Brick { empty, apple, frame, rsnake, lsnake, usnake, dsnake };

const int fieldheight = 7;
const int fieldwidth = 14;
bool borderclip;
bool lost;
Brick field[fieldheight][fieldwidth];

// transform coordinates for snake so they dont touch the border
// assumes (x,y) pair, 0<=x<=fieldwidth and 0<=y<=fieldheight
pair<int, int> coordsWithoutBorders(int xc, int yc) {
  int x, y;
  if (xc == 0) {
    if (borderclip) {
      x = fieldwidth-2;
    } else {
      lost = true;
    }
  } else if (xc == fieldwidth-1) {
    if (borderclip) {
      x = 1;
    } else {
      lost = true;
    }
  } else {
    x = xc;
  }
  if (yc == 0) {
    if (borderclip) {
      y = fieldheight - 2;
    } else {
      lost = true;
    }
  } else if (yc == fieldheight-1) {
    if (borderclip) {
      y = 1;
    } else {
      lost = true;
    }
  } else {
    y = yc;
  }
  return make_pair(x,y);
}

class Snake {
  public:
    int xpos, ypos;
    Direction direction;
    vector< pair<int, int> > body;

    Snake() {
      xpos = 10;
      ypos = 5;
      direction = Direction::right; 
      for (int i = 0; i < 5; i++) {
        body.push_back(make_pair(xpos-i, ypos));     
      }
    }

    void rotate(Direction d) {
      direction = d;
    }

    int getLength() {
      return body.size();
    }

    Direction getDirection() {
      return direction;
    }

    Brick getCurrentSnake() {
      switch (direction) {
        case Direction::right:
          return Brick::rsnake;
        case Direction::left:
          return Brick::lsnake;
        case Direction::up:
          return Brick::usnake;
        case Direction::down:
          return Brick::dsnake;
      }
    }

    void moveForward() {
      body.pop_back();

      switch (direction) {
        case Direction::right:
          body.insert(body.begin(), coordsWithoutBorders(xpos+1, ypos));  
          xpos = coordsWithoutBorders(xpos+1, ypos).first;
          break;
        case up:
          body.insert(body.begin(), coordsWithoutBorders(xpos, ypos-1)); 
          ypos = coordsWithoutBorders(xpos, ypos-1).second;
          break;
        case Direction::left:
          body.insert(body.begin(), coordsWithoutBorders(xpos-1, ypos));
          xpos = coordsWithoutBorders(xpos-1, ypos).first;
          break;
        case down:
          body.insert(body.begin(), coordsWithoutBorders(xpos, ypos+1));
          ypos = coordsWithoutBorders(xpos, ypos+1).second;
          break;
        default:
          cout << "An error occured at moveForward\n";
      }
    }
};

bool isSnake(Brick b) {
  switch (b) {
    case usnake:
    case dsnake:
    case lsnake:
    case rsnake:
      return true;
    default:
      return false;
  }
}

map<Brick, chtype> btoc;

void spawnApple(int seed) {
  sscalls++;
  int randomxpos, randomypos;
  srand(seed);
  randomxpos = 1 + (rand() % (fieldwidth - 2));
  randomypos = 1 + (rand() % (fieldheight - 2));
  if (field[randomypos][randomxpos] == empty) {
    field[randomypos][randomxpos] = apple;
  } else {
    spawnApple(seed + 1);
  }
}

void printfield() {
  clear();
  for (int i = 0; i < fieldheight; i++) {
    for (int j = 0; j < fieldwidth; j++) {
      addch(btoc[field[i][j]]);
    }
    addstr("\n");
  }
}

void iterateGame(Snake* s) {
  pair<int, int> tail = make_pair(s->body.back().second, s->body.back().first);
  s->moveForward();
  Brick b = field[s->body.front().second][s->body.front().first];
  field[s->body.front().second][s->body.front().first] = s->getCurrentSnake();
  if (b == Brick::apple) {
    s->body.push_back(make_pair(tail.second, tail.first));
    if (s->getLength() < (fieldwidth - 2)*(fieldheight - 2)) {
      if (s->getLength() < (fieldwidth - 2)*(fieldheight - 2)-napples + 1) {
        spawnApple(time(NULL)+tail.second+tail.first);
      }
    }
  }else if (isSnake(b)) {
    lost = true;
  } else {
    field[tail.first][tail.second] = empty;
  }
}

void processinput(Snake* s, int input) { 
  Direction direction = s->getDirection();
  switch (input) {
    case KEY_UP:
      if (direction != Direction::down) {
        s->rotate(Direction::up);
      }
      break;
    case KEY_DOWN:
      if (direction != Direction::up) {
        s->rotate(Direction::down);
      }
      break;
    case KEY_LEFT:
      if (direction != Direction::right) {
        s->rotate(Direction::left);
      }
      break;
    case KEY_RIGHT:
      if (direction != Direction::left) {
        s->rotate(Direction::right);
      }
      break;
    default:
      break;
  }
}

int main(void) {
  sscalls = 0;
  // setup field
  fill(&field[0][0], &field[0][0] + sizeof(field) / sizeof(field[0][0]), empty);


  //spawn snake
  Snake s;  
  for (pair<int, int> b : s.body) {
    field[b.second][b.first] = s.getCurrentSnake();
  }
  napples = fieldwidth-2-s.xpos;
  for (int i = s.xpos+1; i < fieldwidth-1; i++) {
    field[s.ypos][i] = apple;
  }

  //spawn borders
  fill_n(field[0], fieldwidth , frame); 
  fill_n(field[fieldheight - 1], fieldwidth, frame);
  for (int i = 1; i < fieldheight-1; i++) {
    field[i][0] = frame;
    field[i][fieldwidth-1] = frame; 
  }

  //spawn apple
  //spawnApple(time(NULL));

  // setup curses
  initscr();
  noecho();
  nodelay(stdscr, TRUE);
  keypad(stdscr, TRUE);
  cbreak();
  
  //setup colors and brick to char map
  start_color();
  init_color(8,0,1000,255); // background color
  init_color(9,800,0,0); // apple red
  init_color(10,0,600,600); // blue-er background color
  init_pair(1,9,8); // apple
  init_pair(2,4,10); // snake
  init_pair(3,7,8); // empty
  btoc[empty] = ' ' | COLOR_PAIR(3);
  btoc[usnake] = ACS_UARROW | A_BOLD | COLOR_PAIR(2);
  btoc[dsnake] = 'v' | A_BOLD | COLOR_PAIR(2);
  btoc[lsnake] = ACS_LARROW | A_BOLD | COLOR_PAIR(2); 
  btoc[rsnake] = ACS_RARROW | A_BOLD | COLOR_PAIR(2);
  btoc[apple] = '@' | A_BOLD | COLOR_PAIR(1); 
  btoc[frame] = '#' | A_BOLD;

  //setup game variables
  int iteration = 0;
  int keyrefreshcount = 100;
  double iterationtime = 0.3; // in seconds
  int maxiterations = INT_MAX;
  bool developerprints = false;
  borderclip = false;
  lost = false;
  
  // begin game
  while (!lost) {
    printfield(); 

    if (developerprints) {
      printw("Iteration %d\n", iteration);
      printw("Length %d\n", s.getLength());
      printw("xpos: %d\n", s.xpos);
      printw("ypos: %d\n", s.ypos);
    }
    printw("capacity: %d\n", s.body.capacity());
    printw("size: %d\n", s.body.size());
    printw("calls to spawnApple: %d\n", sscalls);
    printw("napples: %d\n", napples);
    for (int i = 0; i < keyrefreshcount; i++) { 
      Direction currdirection = s.getDirection();
      int input = getch();
      processinput(&s, input);
      if (currdirection != s.getDirection()) {
        break;
      }
      napms(1000*iterationtime/keyrefreshcount);
    }
    iterateGame(&s);
    if (s.getLength() == (fieldwidth - 2) * (fieldheight - 2)) {
      printfield();
      addstr("You won!\n");
      refresh();
      napms(2000);
      endwin();
      return 0;
    }

    if (iteration >= maxiterations) {
      lost = true;
    }

    iteration++;
  }

  printw("You lost at length %d.\n", s.getLength());
  refresh();
  napms(2000);
  
  endwin();

  return 0;
} 
