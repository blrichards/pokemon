// functions to procedurally generate screen
#include "PokeLib.h"
#include "People.h"
#include "Maps.h"
#include "Actions.h"
#include "Joystick.h"
#include "PokeBattle.h"
#include "Random.h"
#include "ST7735.h"
#include <stdint.h>

#define Down 0
#define Right 1
#define Up 2
#define Left 3
#define Stand 4
#define MidLink 0
#define LftLink 1
#define RgtLink 2

#define PlaySound 0x02
#define StopSound 0x03
#define LoadSound 0x00
#define WildBattle 0x10
#define PlayerBattle 0x20
#define GymBattle 0x30
#define SelectSound 0x40
#define BackGround 0x50

const static Map_t *maps[4] = {&PalletTown, &Route1, &Bedroom, &Home};

static int32_t cRow;
static int32_t cCol;
static int32_t cLeg;
static int32_t cDir;
static int32_t lastDir;
static int8_t upDown;
static int8_t leftRight;
static int32_t speed;
static Map_t Map;

void setDir(int8_t dir, int8_t *upDown, int8_t *leftRight) {
  switch (dir) {
  case Down:
    *upDown = 1;
    *leftRight = 0;
    break;
  case Right:
    *leftRight = 1;
    *upDown = 0;
    break;
  case Up:
    *upDown = -1;
    *leftRight = 0;
    break;
  case Left:
    *leftRight = -1;
    *upDown = 0;
    break;
  }
}

uint8_t spaceOccupied(int8_t upDown, int8_t leftRight) {
  if (Map.tiles[cRow + Player.yCoor + upDown][cCol + Player.xCoor + leftRight]
          ->occupied) {
    if (Map.tiles[cRow + Player.yCoor + upDown]
                 [cCol + Player.xCoor + leftRight]
                     ->has_action) {
      Map.tiles[cRow + Player.yCoor + upDown][cCol + Player.xCoor + leftRight]
          ->action();
    }
    return 1;
  } else {
    return 0;
  }
}

int8_t atEdge(uint8_t dir) {
  switch (dir) {
  case Down:
    return cRow >= Map.height - 9;
  case Right:
    return cCol >= Map.width - 10;
  case Up:
    return cRow <= 1;
  case Left:
    return cCol <= 0;
  }
  return 1;
}

static Tile_t peopleTiles[10];
static Tile_t linkTiles[10][4];

void SetPersonLinks(Person_t *person, uint32_t i) {
  int r, c;
  r = person->yCoor;
  c = person->xCoor;
  // set main tile of person
  peopleTiles[i] = *Map.tiles[r][c];
  peopleTiles[i].oldTile = Map.tiles[r][c];
  peopleTiles[i].occupied = 1;
  peopleTiles[i].has_person = 1;
  peopleTiles[i].has_action = 0;
  peopleTiles[i].has_dialogue = 1;
  peopleTiles[i].dialogue = person->dialogue;
  peopleTiles[i].person = person;
  // set link tile
  if (peopleTiles[i].person->height > 16) {
    linkTiles[i][Up] = *Map.tiles[r - 1][c];
    linkTiles[i][Up].oldTile = Map.tiles[r - 1][c];
    linkTiles[i][Up].linkTile = &peopleTiles[i];
    linkTiles[i][Up].linkDir = Down;
    linkTiles[i][Up].hasLink = 1;
    Map.tiles[r - 1][c] = &linkTiles[i][Up];
  }
  Map.tiles[r][c] = &peopleTiles[i];
}

void Map_Init(uint8_t map, uint32_t startR, uint32_t startC, uint8_t dir) {
  int i;
  Map = *maps[map];
  for (i = 0; i < Map.numPeople; i++) {
    SetPersonLinks(Map.people[i], i);
  }
  cLeg = 0;
  cDir = dir;
  cRow = startR;
  cCol = startC;
  ST7735_PrintMap(&Map, Player.stand[cDir], cRow, cCol, 0, 0, 0);
}

int8_t oppositeDir(int8_t dir) {
  if (dir == Up) {
    return Down;
  } else if (dir == Down) {
    return Up;
  } else if (dir == Left) {
    return Right;
  } else {
    return Left;
  }
}

void Reload_Map(void) {
  int random, i;
  for (i = 0; i < Map.numPeople; i++) {
    random = Random() % 15;
    if (random == 5) {
      if (Map.people[i]->canChangeDir) {
        Map.people[i]->cDir = Random() % 4;
      }
    }
  }
  ST7735_PrintMap(&Map, Player.stand[lastDir], cRow, cCol, 0, 0, 0);
}

void Speak(char *dialogue) {
  uint8_t personDir;
  personDir = oppositeDir(lastDir);
  Map.tiles[cRow + Player.yCoor + upDown][cCol + Player.xCoor + leftRight]
      ->person->cDir = personDir;
  ST7735_PrintMap(&Map, Player.stand[lastDir], cRow, cCol, 0, 0, 0);
  Say(Map.tiles[cRow + Player.yCoor + upDown][cCol + Player.xCoor + leftRight]
          ->dialogue);
}

void CheckForAction(void) {
  setDir(lastDir, &upDown, &leftRight);
  if (Map.tiles[cRow + Player.yCoor + upDown][cCol + Player.xCoor + leftRight]
          ->has_dialogue) {
    Speak(Map.tiles[cRow + Player.yCoor + upDown]
                   [cCol + Player.xCoor + leftRight]
                       ->dialogue);
  }
}

void CheckForPlayerBattle(void) {
  int i, j, delay;
  for (i = 0; i < Map.numPeople; i++) {
    if (Map.people[i]->isTrainer && !Map.people[i]->isDefeated) {
      setDir(Map.people[i]->cDir, &Map.people[i]->upDown,
             &Map.people[i]->leftRight);
      if (Map.people[i]->cDir == Left) {
        if (Player.yCoor + cRow == Map.people[i]->yCoor &&
            Map.people[i]->xCoor - (Player.xCoor + cCol) > 0 &&
            Map.people[i]->xCoor - (Player.xCoor + cCol) <= 5) {
          for (j = Map.people[i]->xCoor - (Player.xCoor + cCol) - 1; j > 0;
               j--) {
            Map.tiles[Map.people[i]->yCoor][Map.people[i]->xCoor] =
                peopleTiles[i].oldTile;
            Map.tiles[Map.people[i]->yCoor - 1][Map.people[i]->xCoor] =
                linkTiles[i][Up].oldTile;
            Map.people[i]->xCoor += Map.people[i]->leftRight;
            Map.people[i]->yCoor += Map.people[i]->upDown;
            SetPersonLinks(Map.people[i], i);
            Map.people[i]->cLeg ^= 1;
            for (delay = 8000000; delay > 0; delay--) {
            };
            Reload_Map();
          }
          Say(Map.people[i]->dialogue);
          Battle_RunFSM(*Map.people[i]);
          Map.people[i]->isDefeated = 1;
        }
      }
    }
  }
}

/*
Controls the player movement and screen movement graphics.
                Input: Movement direction...
                                0 - move down
                                1 - move right
                                2 - move up
                                3 - move left
                                4 - stand current direction
                Output:
                                0 - unsuccessful
                                1 - successful
*/
uint8_t Move(int8_t dir) {
  int pan;

  if (dir == Stand) {
    if (cDir == Stand) {
      return 1;
    }
    ST7735_PrintMap(&Map, Player.stand[cDir], cRow, cCol, 0, 0, 0);
    cDir = Stand;
    return 1;
  }

  setDir(dir, &upDown, &leftRight);

  if (spaceOccupied(upDown, leftRight) || atEdge(dir)) {
    if (cDir == Stand) {
      ST7735_PrintMap(&Map, Player.stand[dir], cRow, cCol, 0, 0, 0);
      return 0;
    }
    ST7735_PrintMap(&Map, Player.stand[dir], cRow, cCol, 0, 0, 0);
    lastDir = dir;
    cDir = Stand;
    return 0;
  } else {
    cRow += upDown;
    cCol += leftRight;
    cDir = dir;
  }

  if (BisPushed()) {
    speed = 6;
  } else {
    speed = 4;
  }

  for (pan = 16; pan > -1; pan -= speed) {
    ST7735_PrintMap(&Map, Player.walk[cDir][cLeg], cRow, cCol, upDown,
                    leftRight, pan);
  }

  CheckForPlayerBattle();

  lastDir = cDir;

  // do action if square has action
  if (Map.tiles[cRow + Player.yCoor][cCol + Player.xCoor]->has_action) {
    Map.tiles[cRow + Player.yCoor][cCol + Player.xCoor]->action();
  }

  cLeg ^= 1;
  return 1;
}
