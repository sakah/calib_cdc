#include "wiremap.h"
#include "TMath.h"
#include "TRandom.h"

WireMap::WireMap(const char* wiremap_path, double width_xoffset_um, double width_yoffset_um, double width_zoffset_um, int uniform_or_fix)
{
   GenerateOffset(width_xoffset_um, width_yoffset_um, width_zoffset_um, uniform_or_fix);

   FILE* fp = fopen(wiremap_path, "r");
   if (fp==NULL) {
      fprintf(stderr, "ERROR: failed to open %s\n", wiremap_path);
      exit(1);
   }
   char line[1280];
   for (int i=0; i<10; i++) fgets(line, sizeof(line), fp); // skip header

   int wid;
   int cid;
   char type;
   char stereo_sign;
   double radius_at_0;
   double radius_at_ep;
   double wire_length;
   int num_wires;
   int num_skip_holes;
   double dphi;
   double stereo_drop;
   double stereo_angle;
   double first_hole_rad;
   int hole_idx_first_sense;

   num_sense_layers_ = 0;
   for (int cid=0; cid<MAX_LAYER; cid++) {
      num_cells_[cid] = 0;
   }
   printf("read wiremap\n");
   while(fgets(line, sizeof(line), fp)) {
      sscanf(line, "layer %d %d %c %c %lf %lf %lf %d %d %lf %lf %lf %lf %d",
            &wid, &cid, &type, &stereo_sign, &radius_at_0, &radius_at_ep, &wire_length, &num_wires, &num_skip_holes, &dphi, &stereo_drop, &stereo_angle, &first_hole_rad, &hole_idx_first_sense);
      //printf("%s", line);
      //continue;
      //printf("wid %d cid %d type %c stereo_sign %c radius_at_0 %f radius_at_ep %f wire_length %f num_wires %d num_skips %d dphi %f stereo_drop %f stereo_angle %f first_hole_rad %f hole_idx_first_sense %d\n", wid, cid, type, stereo_sign, radius_at_0, radius_at_ep, wire_length, num_wires, num_skips, dphi, stereo_drop, stereo_angle, first_hole_rad, hole_idx_first_sense);

      if (type=='F') {
         continue;
      }

      num_cells_[cid] = num_wires/2;

      double xro = 0;
      double yro = 0;
      double zro = +wire_length/2.0;
      double xhv = 0;
      double yhv = 0;
      double zhv = -wire_length/2.0;

      double drad_cell = TMath::TwoPi()/num_cells_[cid];
      double drad_hole = TMath::TwoPi()/num_wires;

      //printf("cid %d radius_at_ep %f hole_idx_first_sense %d\n", cid, radius_at_ep, hole_idx_first_sense);
      for (int icell=0; icell<num_cells_[cid]; icell++) {
         double rad_ro = drad_hole*hole_idx_first_sense + drad_cell*icell;
         double rad_hv = drad_hole*hole_idx_first_sense + drad_cell*(icell + num_skip_holes/2);
         xro = radius_at_ep * TMath::Cos(rad_ro);
         yro = radius_at_ep * TMath::Sin(rad_ro);
         xhv = radius_at_ep * TMath::Cos(rad_hv);
         yhv = radius_at_ep * TMath::Sin(rad_hv);

         // add offset
         double xoff = offset_wirepos_um[cid][icell].X()*1e-4; // um -> cm
         double yoff = offset_wirepos_um[cid][icell].Y()*1e-4; // um -> cm
         double zoff = offset_wirepos_um[cid][icell].Z()*1e-4; // um -> cm
         xro += xoff;
         yro += yoff;
         zro += zoff;
         xhv += xoff;
         yhv += yoff;
         zhv += zoff;

         wires_[cid][icell].MakeLine(TVector3(xro, yro, zro), TVector3(xhv, yhv, zhv));
         zro_layers_[cid] = zro;
         zhv_layers_[cid] = zhv;

         //if (icell==0) printf("icell %d yro %f zro %f zhv %f\n", icell, yro, zro, zhv);
      }

      num_sense_layers_++;
   }
   fclose(fp);
}

int WireMap::GetNumSenseLayers()
{
   return num_sense_layers_;
}

int WireMap::GetNumCells(int cid)
{
   return num_cells_[cid];
}

Line& WireMap::GetWire(int cid, int icell)
{
   return wires_[cid][icell];
}

double WireMap::GetZRO(int cid)
{
   return zro_layers_[cid];
}

double WireMap::GetZHV(int cid)
{
   return zhv_layers_[cid];
}

void WireMap::GenerateOffset(double width_x_um, double width_y_um, double width_z_um, int uniform_or_fix)
{
   gRandom->SetSeed(10); // fix seed to repeat event

   double x=0;
   double y=0;
   double z=0;
   for (int cid=0; cid<MAX_LAYER; cid++) {
      for (int icell=0; icell<MAX_CELL; icell++) {
         if (uniform_or_fix==RAND_UNIFORM) {
            x = gRandom->Uniform( -width_x_um/2.0, +width_x_um/2.0);
            y = gRandom->Uniform( -width_y_um/2.0, +width_y_um/2.0);
            z = gRandom->Uniform( -width_z_um/2.0, +width_z_um/2.0);
         } else if (uniform_or_fix==RAND_FIX) {
            x = width_x_um;
            y = width_y_um;
            z = width_z_um;
         }
         //printf("offset: x %f y %f z %f\n", x, y, z);
         offset_wirepos_um[cid][icell].SetXYZ(x, y, z);
      }
   }
}

TVector3& WireMap::GetOffset(int cid, int icell)
{
   return offset_wirepos_um[cid][icell];
}

void WireMap::PrintOffset(int cid, int icell)
{
   double x = offset_wirepos_um[cid][icell].X();
   double y = offset_wirepos_um[cid][icell].Y();
   double z = offset_wirepos_um[cid][icell].Z();
   printf("cid %d icell %d offset: x %4.2f (um) y %4.2f (um) z %4.2f (um)\n", cid, icell, x, y, z);
}
