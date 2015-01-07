

#include "mex.h"

#include "dali.h"

#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>


#define POW2(x)      ((x)*(x))


static dali_t* dali_from_input( int ulen, int vlen, int wlen,
      const double *data, const double *mask )
{
   int id;
   int u, v, k;
   int done;
   int ue, ve;
   dali_t *desc;
   
   desc        = calloc( 1, sizeof(dali_t) );
   desc->n     = 1;
   desc->ulen  = ulen;
   desc->vlen  = vlen;
   desc->wlen  = wlen;
   desc->len   = ulen*vlen;
   desc->sz    = (desc->ulen-1)/2;
   desc->desc  = malloc( desc->ulen*desc->vlen*desc->wlen * sizeof(double) );
   desc->mask  = malloc( desc->ulen*desc->vlen * sizeof(char) );
   desc->shape = malloc( desc->ulen*desc->vlen * sizeof(int) );
   desc->gauss = malloc( desc->ulen*desc->vlen * sizeof(double) );
   desc->sgauss = malloc( desc->ulen*desc->vlen * sizeof(double) );
   desc->sigma = malloc( sizeof(double) );

   /* Defaults. */
   *desc->sigma = INFINITY;

   /* Store output. */
   id = 0;
   for (u=0; u < desc->ulen; u++) {
      for (v=0; v < desc->vlen; v++) {
         /* Data. */
         for (k=0; k < desc->wlen; k++)
            desc->desc[ id*desc->wlen + k ] = 
                  data[ k*desc->ulen*desc->vlen + u*desc->vlen + v ];
         /* Mask. */
         desc->mask[ id ]  = (fabs(mask[ u*desc->vlen + v ]) > 1e-10);
         /* Shape. */
         desc->shape[ id ] = id;
         /* Gaussian. */
         desc->gauss[ id ] = -(double)(POW2(u-desc->sz)+POW2(v-desc->sz));
         /* Inc. */
         id++;
      }
   }

   return desc;
}


void mexFunction( int nlhs, mxArray *plhs[],
      int nrhs, const mxArray *prhs[])
{
   double sigma, rot, scale;
   int lin;

   /* Defaults. */
   sigma = 0.5;
   rot   = 0.;
   scale = 1.;
   lin   = 1;

   /* Check for proper number of input and output arguments */
   if (nrhs < 4)
      mexErrMsgTxt("At least four input arguments required.");
      if (nrhs > 8)
      mexErrMsgTxt("Too many input parameters.");

   /* Check input parameters. */
   if (!mxIsDouble(prhs[0]))
      mexErrMsgTxt("Descriptor 1 parameter must be double matrix." );
   if (!mxIsDouble(prhs[2]))
      mexErrMsgTxt("Descriptor 1 mask parameter must be a double matrix." );
   if (!mxIsDouble(prhs[1]))
      mexErrMsgTxt("Descriptor 2 parameter must be double matrix." );
   if (!mxIsDouble(prhs[3]))
      mexErrMsgTxt("Descriptor 2 mask parameter must be a double matrix." );
   if (nrhs > 4) {
      if (!mxIsDouble(prhs[4]) || (mxGetNumberOfElements(prhs[4]) != 1))
         mexErrMsgTxt("Sigma parameter must be a double.");
      sigma = mxGetScalar( prhs[4] );
   }
   if (nrhs > 5) {
      if (!mxIsDouble(prhs[5]) || (mxGetNumberOfElements(prhs[5]) != 1))
         mexErrMsgTxt("Rotation parameter must be a double.");
      rot = mxGetScalar( prhs[5] );
   }
   if (nrhs > 6) {
      if (!mxIsDouble(prhs[6]) || (mxGetNumberOfElements(prhs[6]) != 1))
         mexErrMsgTxt("Scale parameter must be a double.");
      scale = mxGetScalar( prhs[6] );
   }
   if (nrhs > 7) {
      if (!mxIsDouble(prhs[7]) || (mxGetNumberOfElements(prhs[7]) != 1))
         mexErrMsgTxt("Linear parameter must be a double.");
      lin = (fabs(mxGetScalar( prhs[7] )) > 1e-8);
   }

   int ulen, vlen, wlen;
   const mwSize *dimd1 = mxGetDimensions(prhs[0]);
   const mwSize *dimd2 = mxGetDimensions(prhs[1]);
   const mwSize *dimm1 = mxGetDimensions(prhs[2]);
   const mwSize *dimm2 = mxGetDimensions(prhs[3]);
   if ((dimd1[0] != dimd2[0]) ||
         (dimd1[0] != dimm1[0]) ||
         (dimd1[0] != dimm2[0]) ||
         (dimd1[1] != dimd2[1]))
      mexErrMsgTxt( "Dimensions of descriptors and masks do not match." );
   ulen = (int)sqrt( dimd1[0] );
   vlen = ulen;
   wlen = dimd1[1];

   /* Create temporary descriptors. */
   dali_t *desc1, *desc2;
   desc1 = dali_from_input( ulen, vlen, wlen,
         (double*)mxGetData(prhs[0]), (double*)mxGetData(prhs[2]) );
   desc2 = dali_from_input( ulen, vlen, wlen,
         (double*)mxGetData(prhs[1]), (double*)mxGetData(prhs[3]) );

   /* Calculate distance. */
   plhs[0] = mxCreateDoubleMatrix( 1, 1, mxREAL );
   double *dist = (double*)mxGetData(plhs[0]);
   /*printf( "scale=%.3f, rot=%.3f, lin=%d\n", scale, rot, lin );*/
   if ((scale == 1.) && (rot == 0.))
      dist[0] = dali_distance( desc1, desc2, sigma );
   else if (lin)
      dist[0] = dali_distance_transform_lin( desc1, desc2, sigma, rot, scale );
   else
      dist[0] = dali_distance_transform( desc1, desc2, sigma, rot, scale );
  
   /* Clean up. */
   dali_free( desc1 );
   dali_free( desc2 );
}





