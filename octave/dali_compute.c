

#include "mex.h"

#include "dali.h"

#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>


#define MIN(a,b)     (((a) < (b)) ? (a) : (b))


static int* inputtoint( const mxArray *val )
{
   int i;
   int n = mxGetNumberOfElements( val );
   int *out = malloc( n * sizeof(int) );
   double *in = (double*) mxGetData( val );
   for (i=0; i<n; i++)
      out[i] = (int)round( in[i]-1 ); /* Convert matlab indices to C indices. */
   return out;
}


static void parseInput( dali_params_t *params, int n, const mxArray *prhs[] )
{
   if ((n%2) != 0)
      mexErrMsgTxt("Must have additional parameters in the form of 'Name', value.");

   int i;
   char name[256];
   for (i=0; i<n; i+=2) {
      if (!mxIsChar(prhs[i]))
         mexErrMsgTxt("Parameter name must be a string.");
      mxGetString( prhs[i], name, sizeof(name) );

      if (strcasecmp(name,"mtype")==0) {
         char buf[256];
         mxGetString( prhs[i+1], buf, sizeof(buf) );
         if (strcasecmp(buf,"square")==0)
            params->mtype = DALI_MESH_TYPE_SQUARE_DENSE;
         else if (strcasecmp(buf,"circle")==0)
            params->mtype = DALI_MESH_TYPE_CIRCLE_DENSE;
         else if (strcasecmp(buf,"circle_var")==0)
            params->mtype = DALI_MESH_TYPE_CIRCLE_VARIABLE;
         else if (strcasecmp(buf,"circle_gauss")==0)
            params->mtype = DALI_MESH_TYPE_CIRCLE_GAUSS;
         else {
            char errmsg[128];
            snprintf( errmsg, sizeof(errmsg), "Unknown mesh type '%s'.", buf );
            mexErrMsgTxt( errmsg );
         }
      }
      else if (strcasecmp(name,"sz")==0)
         params->Sz = mxGetScalar( prhs[i+1] );
      else if (strcasecmp(name,"sz_coarse")==0)
         params->Sz_coarse = mxGetScalar( prhs[i+1] );
      else if (strcasecmp(name,"beta")==0)
         params->beta = mxGetScalar( prhs[i+1] );
      else if (strcasecmp(name,"wmax")==0)
         params->wmax = mxGetScalar( prhs[i+1] );
      else if (strcasecmp(name,"ncomp")==0)
         params->ncomp = mxGetScalar( prhs[i+1] );
      else if (strcasecmp(name,"ntime")==0)
         params->ntime = mxGetScalar( prhs[i+1] );
      else if (strcasecmp(name,"lanczos")==0)
         params->lanczos = mxGetScalar( prhs[i+1] );
      else if (strcasecmp(name,"verbose")==0)
         params->verbose = mxGetScalar( prhs[i+1] );
      else if (strcasecmp(name,"eigs_iter")==0)
         params->eigs_iter = mxGetScalar( prhs[i+1] );
      else if (strcasecmp(name,"mesh_K")==0)
         params->mesh_K = mxGetScalar( prhs[i+1] );
      else if (strcasecmp(name,"mesh_sigma")==0)
         params->mesh_sigma = mxGetScalar( prhs[i+1] );
      else if (strcasecmp(name,"use_si")==0)
         params->use_si = mxGetScalar( prhs[i+1] );
      else {
         char buf[128];
         snprintf( buf, sizeof(buf), "Unknown parameter '%s'.", name );
         mexErrMsgTxt( buf );
      }

     /*printf("%s = %f\n", name, mxGetScalar( prhs[i+1] ));*/
   }
}

static void mex_setValue( mxArray *pr, int pos, double val )
{
   mxArray *md = mxCreateDoubleMatrix(1,1,mxREAL);
   double *ptr = (double*) mxGetData( md );
   ptr[0] = val;
   mxSetFieldByNumber( pr, 0, pos, md );
}

void mexFunction( int nlhs, mxArray *plhs[],
      int nrhs, const mxArray *prhs[])
{
   /* Check for proper number of input and output arguments */
   if (nrhs < 3)
      mexErrMsgTxt("At least three input arguments required.");
   if (nlhs > 3)
      mexErrMsgTxt("Too many output arguments.");

   /* Check input parameters. */
   if (!mxIsDouble(prhs[0]))
      mexErrMsgTxt("Image parameter must be double matrix." );
   if (!mxIsDouble(prhs[1]))
      mexErrMsgTxt("U parameter must be double vector." );
   if (!mxIsDouble(prhs[2]))
      mexErrMsgTxt("V parameter must be double vector." );
   if (mxGetNumberOfElements(prhs[1]) != mxGetNumberOfElements(prhs[2]))
      mexErrMsgTxt("U and V parameters must be of the same length.");

   /* Load parameters. */
   dali_params_t params;
   dali_optsDefault( &params );
   parseInput( &params, nrhs-3, &prhs[3] );

   /* Get the data */
   const mwSize *dim = mxGetDimensions(prhs[0]);
   double *imdata    = (double*)mxGetData(prhs[0]);
   int w, h;
   w = dim[0];
   h = dim[1];
   /*printf( "Image is w=%d, h=%d\n", w, h );*/

   /* U and V vectors. */
   int *uc = inputtoint( prhs[1] );
   int *vc = inputtoint( prhs[2] );
   int n  = mxGetNumberOfElements(prhs[1]);
   /*printf( "Points to check out are %d\n", n );*/

   /* Make sure dimensions are sane. */
   int i;
   for (i=0; i<n; i++) {
      if ((uc[i] < 0) || (uc[i] >= w))
         mexErrMsgTxt( "Descriptor point out of range." );
      if ((vc[i] < 0) || (vc[i] >= h))
         mexErrMsgTxt( "Descriptor point out of range." );
   }

   /* Compute descriptors. */
   dali_info_t info;
   dali_t *desc = dali_compute( imdata, w, h, uc, vc, n, &params, &info );
   if (desc == NULL) {
      free( uc );
      free( vc );
      mexErrMsgTxt( "Error while calculating descriptor." );
   }

   /* Set up output. */
   int dims[3];
   dims[0] = desc->ulen * desc->vlen;
   dims[1] = desc->wlen;
   dims[2] = desc->n;
   /*printf( "Output dim is w=%d, h=%d, n=%d\n", dims[0], dims[1], dims[2] );*/
   plhs[0] = mxCreateNumericArray( 3, dims, mxDOUBLE_CLASS, mxREAL );
   double *dat = (double*)mxGetData(plhs[0]);

   /* Store output. */
   int id;
   int u, v, k;
   for (i=0; i<dims[2]; i++) {
      for (u=0; u < desc[i].ulen; u++) {
         for (v=0; v < desc[i].vlen; v++) {
            /* Get ID. */
            id = desc[i].shape[ u*desc[i].vlen + v ];
            /* Set default value. */
            if (id < 0) {
               for (k=0; k < desc[i].wlen; k++)
                  dat[ i*desc[i].ulen*desc[i].vlen*desc[i].wlen +
                       k*desc[i].ulen*desc[i].vlen +
                       u*desc[i].vlen + v ] = 0.;
               continue;
            }
            /* Actually set value. */
            for (k=0; k < desc[i].wlen; k++)
               dat[ i*desc[i].ulen*desc[i].vlen*desc[i].wlen +
                    k*desc[i].ulen*desc[i].vlen +
                    u*desc[i].vlen + v ] =
                     desc[i].desc[ id*desc[i].wlen + k ];
         }
      }
   }

   /* Store masks also. */
   if (nlhs > 1) {
      dims[0] = desc->ulen * desc->vlen;
      dims[1] = desc->n;
      plhs[1] = mxCreateNumericArray( 2, dims, mxDOUBLE_CLASS, mxREAL );
      dat = (double*)mxGetData(plhs[1]);
      memset( dat, 0, dims[0]*dims[1]*sizeof(double) );
      int um, vm;
      for (i=0; i<dims[1]; i++) {
         for (u=0; u < desc[i].ulen; u++) {
            for (v=0; v < desc[i].vlen; v++) {
               /* Get ID. */
               id = desc[i].shape[ u*desc[i].vlen + v ];
               /* Set to default value. */
               if (id < 0)
                  continue;
               /* Read data. */
               if (desc[i].mask[ id ])
                  dat[ i*dims[0] + u*desc[i].vlen + v ] = 1.;
            }
         }
      }
   }

   if (nlhs > 2) {
      const char *fieldnames[] = {
         "nodes_real", "nodes_total", "faces",
         "time_meshing",
         "time_laplacebeltrami",
         "time_eigenvectors",
         "time_hks",
         "time_hks_si",
         "time_elapsed"
      };
      plhs[2] = mxCreateStructMatrix( 1, 1, 9, fieldnames );

      mex_setValue( plhs[2], 0, (double)info.nodes_real );
      mex_setValue( plhs[2], 1, (double)info.nodes_total );
      mex_setValue( plhs[2], 2, (double)info.faces );

      mex_setValue( plhs[2], 3, info.time_meshing );
      mex_setValue( plhs[2], 4, info.time_laplacebeltrami );
      mex_setValue( plhs[2], 5, info.time_eigenvectors );
      mex_setValue( plhs[2], 6, info.time_hks );
      mex_setValue( plhs[2], 7, info.time_hks_si );
      mex_setValue( plhs[2], 8, info.time_elapsed );
   }

   /* Clean up. */
   free( uc );
   free( vc );
   dali_free( desc );
}





