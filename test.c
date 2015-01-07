/**
  *    DaLI: Deformation and Light Invariant Descriptor
  *    Edgar Simo-Serra, Carme Torras, Francesc Moreno-Noguer
  *    International Journal of Computer Vision (IJCV), 2015
  *
  * Copyright (C) <2011-2015>  <Francesc Moreno-Noguer, Edgar Simo-Serra>
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the version 3 of the GNU General Public License
  * as published by the Free Software Foundation.
  *
  * This program is distributed in the hope that it will be useful, but
  * WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  * General Public License for more details.      
  * You should have received a copy of the GNU General Public License
  * along with this program. If not, see <http://www.gnu.org/licenses/>.
  *
  * Edgar Simo-Serra, Institut de Robotica i Informatica Industrial (CSIC/UPC)
  * esimo@iri.upc.edu, http://www-iri.upc.es/people/esimo/
 **/


#include "dali.h"

#include <fenv.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


static double rand_double (void)
{
   return (double)rand() / (double)RAND_MAX;
}
static double rand_double_range( double low, double high )
{  
   return (low + (high-low)*rand_double());
}
static int rand_int_range( int low, int high )
{
   return (int)round( rand_double_range( (double)low, (double)high ) );
}

int main( int argc, char *argv[] )
{
   (void) argc;
   (void) argv;
  
   int i, j, n;
   dali_params_t params;
   double *im;
   int w, h;
   int uc[6] = { 10-1, 311-1, 120-1, 310-1, 150, 149 };
   int vc[6] = { 9-1,  1-1,  230-1,  200-1, 150, 149 };
   dali_t *desc;
   FILE *fout;

   srand( 9 );

   //fout = fopen( "imdata.dat", "w" );
   n = 6;
   w = 320;
   h = 240;
   im = calloc( w*h, sizeof(double) );
   for (i=0; i<h; i++) {
      for (j=0; j<w; j++) {
         //im[ i*w + j ] = i*j % 256;
         im[ i*w + j ] = ((i*j % 256 + i + 10*j) + rand_int_range(-30,30))%256;
         //fprintf( fout, "%f ", im[ i*w + j ] );
      }
      //fprintf( fout, "\n" );
   }
   //fclose( fout );

   dali_optsDefault( &params );
   params.Sz_coarse = 10;
   params.mtype = DALI_MESH_TYPE_CIRCLE_VARIABLE;
   //params.mtype = DALI_MESH_TYPE_CIRCLE_GAUSS;
   params.mesh_K = 1000.;
   params.mesh_sigma = 0.5;
   params.Sz    = 20;
   params.wmax  = 20;
   params.ncomp = 100;
   params.ntime = 100;
   //params.threads = 5;
   params.verbose = 1;
   dali_info_t info;
   desc = dali_compute( im, w, h, uc, vc, n, &params, &info );
   dali_fprintInfo( stdout, &info );
   printf( "Length = %d\n", desc->len );

#if 0
   int sz = params.Sz;
   int nr = (sz*2+1);
   char buf[128];
   double d;
   int u, v, k;
   int id;
   for (i=0; i<desc->n; i++) {
      snprintf( buf, sizeof(buf), "D%d.dat", i+1 );
      fout = fopen( buf, "w" );
      for (u=0; u<nr; u++) {
         for (v=0;v<nr; v++) {
            id = desc[i].shape[ u*desc[i].vlen + v ];
            if (id < 0) {
               for (k=0; k<desc[i].wlen; k++)
                  fprintf( fout, "%.18f ", 0. );
               continue;
            }
            for (k=0; k<desc[i].wlen; k++) {
               d = desc[i].desc[ id*desc[i].wlen + k ];
               fprintf( fout, "%.18f ", d );
            }
            fprintf( fout, "\n" );
         }
      }
      fclose(fout);
   }
#else
   int sz = params.Sz;
   int nr = (sz*2+1);
   char buf[128];
   double d;
   int u, v, k;
   int id;
   for (i=0; i<desc->n; i++) {
      snprintf( buf, sizeof(buf), "D%d.dat", i+1 );
      //fout = fopen( buf, "w" );
      for (u=0; u<desc->len; u++) {
         for (k=0; k<desc[i].wlen; k++) {
            d = desc[i].desc[ u*desc[i].wlen + k ];
            //fprintf( fout, "%.18f ", d );
         }
         //fprintf( fout, "\n" );
      }
      //fclose(fout);
   }
#endif

   double sigma = 0.5;
   if (n > 1) {
#if 0
      printf("Normal\n");
      for (i=0; i<n; i++)
         for (j=i+1; j<n; j++)
            printf( "%d vs %d: %f\n", i+1, j+1, dali_distance( &desc[i], &desc[j], sigma ) );
      printf("Transform\n");
      for (i=0; i<n; i++)
         for (j=i+1; j<n; j++)
            printf( "%d vs %d: %f\n", i+1, j+1, dali_distance_transform( &desc[i], &desc[j], sigma, 0., 1. ) );
      printf("Transform linear\n");
      for (i=0; i<n; i++)
         for (j=i+1; j<n; j++)
            printf( "%d vs %d: %f\n", i+1, j+1, dali_distance_transform_lin( &desc[i], &desc[j], sigma, 0., 1. ) );
#endif
      printf("Pure\n");
      for (i=0; i<n; i++)
         for (j=i+1; j<n; j++)
            printf( "%d vs %d: %f\n", i+1, j+1, dali_distance_pure( &desc[i], &desc[j] ) );
   }

   double rot, scale, dist1, dist2;
   printf( "Transform test:\n" );
#if 0
   for (rot = -5.; rot <= 5.0; rot += 5.0) {
      for (scale = 0.95; scale <= 1.05; scale += 0.05) {
         dist1 = dali_distance_transform( &desc[0], &desc[0], sigma, rot/180.*M_PI, scale );
         dist2 = dali_distance_transform_lin( &desc[0], &desc[0], sigma, rot/180.*M_PI, scale );
         printf( "   rot=%+.2f, scale=%+.2f || nearest=%.5f, linear=%.5f\n", rot, scale, dist1, dist2 );
      }
   }
   scale = 1.;
   for (rot = -5.; rot <= 5.0; rot += 5.0) {
      dist1 = dali_distance_transform( &desc[0], &desc[0], sigma, rot/180.*M_PI, scale );
      dist2 = dali_distance_transform_lin( &desc[0], &desc[0], sigma, rot/180.*M_PI, scale );
      printf( "   rot=%+.1f:  transform=%f,  linear=%f\n", rot, dist1, dist2 );
   }
#endif
   
   free( im );
   dali_free( desc );

   return 0;
}


