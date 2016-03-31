#ifndef WIREMAP_H
#define WIREMAP_H

#include "param.h"
#include "line.h"

class WireMap
{
   public:
      WireMap(const char* wiremap_path, double width_xoffset_um, double width_yoffset_um, double width_zoffset_um, int uniform_or_fix);
      int GetNumSenseLayers();
      int GetNumCells(int cid);
      Line& GetWire(int cid, int icell);
      double GetZRO(int cid);
      double GetZHV(int cid);
      TVector3& GetOffset(int cid, int icell);
      void PrintOffset(int cid, int icell);

   private:
      int num_sense_layers_;
      int num_cells_[MAX_LAYER];
      Line wires_[MAX_LAYER][MAX_CELL];
      double zro_layers_[MAX_LAYER];
      double zhv_layers_[MAX_LAYER];
      void GenerateOffset(double width_x_um, double width_y_um, double width_z_um, int uniform_or_fix);
      TVector3 offset_wirepos_um[MAX_LAYER][MAX_CELL];
};

#endif
