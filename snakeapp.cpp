#include <iostream>
#include <myconio_mac.h>
#include <string>
#include <map>
#include <algorithm>
using namespace std;

enum Direction { left, right, up, down };

class Snake {
  public:
    int xpos, ypos, length;
    Direction direction;
    Snake() {
      xpos = 10;
      ypos = 5;
      length = 5;
      direction = Direction::right;      
    }
};

enum Brick { empty, snake, apple, border };

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

int main() {
  // setup snake and field
  Snake s;
  fill(&field[0][0], &field[0][0] + sizeof(field) / sizeof(field[0][0]), empty);

  //spawn snake
  for (int i = 0; i < s.length; i++) {
    field[s.ypos][s.xpos - i] = snake;
  }

  //spawn borders
  fill_n(field[0], fieldwidth , border); 
  fill_n(field[fieldheight - 1], fieldwidth, border);
  for (int i = 1; i < fieldheight-1; i++) {
    field[i][0] = border;
    field[i][fieldwidth-1] = border;
  }

  //spawn apple
  spawnApple(time(NULL));

  //setup brick to char map
  btoc[empty] = ' ';
  btoc[snake] = 'S';
  btoc[apple] = '@';
  btoc[border] = '#';


  //setup done; print field
  clrscr();
  for (int i = 0; i < fieldheight; i++) {
    for (int j = 0; j < fieldwidth; j++) {
      cout << btoc[field[i][j]];

    }
    cout << "\n";
  }

  //begin game
  bool gameinprogress = true;
  while (gameinprogress) {
    




    gameinprogress = false;
  }


} 
