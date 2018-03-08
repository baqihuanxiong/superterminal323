#ifndef Fieldpro_h
#define Fieldpro_h

#include "Arduino.h"

extern const byte rfidMap[548][4];

extern const String CMD_EMPTY;
extern const String CMD_HEAVY;
extern const String CMD_STOP;
extern const String CMD_RETREAT;

extern const int RETREAT_DISTANCE;

struct Zone {
  byte *card_start;
  byte *card_end;
  String *cmds;
  int *periods;
  int len;
};

struct Procedure {
  String *cmds;
  int *periods;
  int len;
};

Procedure getZoneProcedure(int index);

#endif
