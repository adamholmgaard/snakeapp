#include <iostream>
#include <ncurses.h>
#include <string>
#include <map>
#include <algorithm>
using namespace std;

enum Direction { left, right, up, down };

class Snake {
  public:
    int xpos, ypos, length;
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


    void moveForward() {
      body.pop_back();

      switch (direction) {
        case Direction::right:
          body.insert(body.begin(), make_pair(xpos+1, ypos));
          xpos++;
          break;
        case up:
          body.insert(body.begin(), make_pair(xpos, ypos-1));
          ypos--;
          break;
        case Direction::left:
          body.insert(body.begin(), make_pair(xpos-1, ypos));
          xpos--;
          break;
        case down:
          body.insert(body.begin(), make_pair(xpos, ypos+1));
          ypos++;
          break;
        default:
          cout << "An error occured at moveForward\n";
      }

    }
};

enum Brick { empty, snake, apple, frame };

const int fieldheight = 11;
const int fieldwidth = 30;
Brick field[fieldheight][fieldwidth];

map<Brick, char> btoc;

void spawnApple(int seed) {
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
  field[s->body.back().second][s->body.back().first] = empty;
  s->moveForward();
  field[s->body.front().second][s->body.front().first] = snake;

  printw("xpos: %d\n", s->xpos);
  printw("ypos: %d\n", s->ypos);


}

int main() {
  // setup field
  fill(&field[0][0], &field[0][0] + sizeof(field) / sizeof(field[0][0]), empty);

  //spawn snake
  Snake s;  
  for (pair<int, int> b : s.body) {
    field[b.second][b.first] = snake;
  }

  //spawn borders
  fill_n(field[0], fieldwidth , frame); 
  fill_n(field[fieldheight - 1], fieldwidth, frame);
  for (int i = 1; i < fieldheight-1; i++) {
    field[i][0] = frame;
    field[i][fieldwidth-1] = frame; 
  }

  //spawn apple
  spawnApple(time(NULL));

  //setup brick to char map
  btoc[empty] = ' ';
  btoc[snake] = 'S';
  btoc[apple] = '@';
  btoc[frame] = '#';


  // setup game variables
  int iteration = 0;
  bool gameinprogress = true;

  // setup curses
  initscr();
  noecho();
  nodelay(stdscr, TRUE);

  // begin game
  while (gameinprogress) {
    printfield(); 

    if (iteration == 6) {
      s.rotate(up);
    }
    iterateGame(&s);

    printw("Iteration %d\n", iteration);

    refresh();
    getch();



    napms(1000);





    if (iteration == 10) {
      gameinprogress = false;
    }
    iteration++;

  }

  endwin();


} 
