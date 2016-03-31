#ifndef PARAM_H
#define PARAM_H

#define g_debug_line 0
#define g_debug_track 0
#define g_debug_trackfinder 0

enum
{
   MAX_LAYER=20,
   MAX_CELL=310,
   MAX_CELL_HIT=10,
   MAX_TRACK=100,
   MAX_HITLIST=200
};

enum {
   REGION_ALL=0,
   REGION_UP,
   REGION_DOWN
};

enum {
   RAND_UNIFORM=0,
   RAND_FIX
};

#endif
