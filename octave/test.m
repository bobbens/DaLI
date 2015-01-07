
u = [ 10 311 120 310, 151, 150 ];
v = [ 9 1 230 200, 151, 150 ];

w = 320;
h = 240;
im = [];
for i=1:h;
   for j=1:w;
      im(j,i) = mod( mod( (i-1)*(j-1), 256) + (i-1) + 10*(j-1), 256 );
   end
end

[ desc,  mask, info ] = dali_compute( im, u, v, 'sz', 20, 'sz_coarse', 10, 'wmax', 20, 'ncomp', 100, 'ntime', 100, 'verbose', 1 );
%[ desc,  mask, info ] = dali_compute( im, u, v, 'sz', 15, 'wmax', 20, 'ncomp', 100, 'ntime', 20, 'verbose', 2, 'use_si', 0 );
%[ desc,  mask, info ] = dali_compute( im, u, v, 'sz', 15, 'wmax', 20, 'ncomp', 100, 'ntime', 100, 'verbose', 1 );
%[ desc1, mask2] = dali_compute_m( im', u', v', 15, 20, 100, 100 );

n = size(u,2);
fprintf( '%d vs %d: %f\n', 1, 1, dali_distance( desc(:,:,1), desc(:,:,1), mask(:,1), mask(:,1) ) );
for i=1:n;
   for j=i+1:n;
      fprintf( '%d vs %d: %f\n', i, j, dali_distance( desc(:,:,i), desc(:,:,j), mask(:,i), mask(:,j) ) );
   end
end
