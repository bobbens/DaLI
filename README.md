DaLI: Deformation and Light Invariant Descriptor
================================================


OVERVIEW
--------

This code provides an implementation of the research paper:

```
  Edgar Simo-Serra, Carme Torras, Francesc Moreno-Noguer
  DaLI: Deformation and Light Invariant Descriptor
  International Journal of Computer Vision (IJCV), 2015
```

Which was originally published in

```
   F.Moreno-Noguer
   Deformation and Illumination Invariant Feature Point Descriptor
   Conference in Computer Vision and Pattern Recognition (CVPR), 2011
```

This allows local representation of image patches in such a way that they can be compared with strong invariance to both deformation and illumination.

The core of the code is written in C and is meant to be embedded in applications. It should be also possible to compile as a library and installed at a system level.


License
-------

```
  Copyright (C) <2011-2015> <Francesc Moreno-Noguer, Edgar Simo-Serra>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the version 3 of the GNU General Public License
  as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.      
  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.

  Edgar Simo-Serra, Institut de Robotica i Informatica Industrial (CSIC/UPC), January 2015.
  esimo@iri.upc.edu, http://www-iri.upc.es/people/esimo/
```


Installation
------------

The software depends on the [ceigs library](https://github.com/bobbens/ceigs) which provides an elegant C frontend to the [ARPACK library](http://www.caam.rice.edu/software/ARPACK/). Upon installing the ceigs library it should be possible to compile both the test application and the matlab/octave by simpling running make:

```
$ make
```


Usage
-----

The descriptor can be used both from octave (theoretically also matlab) and C. For an example of usage from C see the "test.c" file. As it should be compiled you can also run it by doing

```
$ ./test
```

You should get an output as the following:

```
Processing point 9 x 8...
   Computing mesh...
   Computing Laplace-Beltrami...
   Computing EigenVectors...
   Computing HKS...
   Computing HKS-SI...
   Done!
Processing point 310 x 0...
   Computing mesh...
   Computing Laplace-Beltrami...
   Computing EigenVectors...
   Computing HKS...
   Computing HKS-SI...
   Done!

 ...

2 vs 6: 3967.624281
3 vs 4: 1897.634498
3 vs 5: 2542.110789
3 vs 6: 2476.372066
4 vs 5: 2614.770836
4 vs 6: 2631.432283
5 vs 6: 956.697412
```

Additionally it is possible to run the application from octave. Currently matlab is not supported due to a library conflict, but in theory it should also be possible. To try the octave version from the "octave/" directory launch octave and run "test" as such:

```
>> test
```

You should get an output such as:

```
Processing point 9 x 8...
   Computing mesh...
   Computing Laplace-Beltrami...
   Computing EigenVectors...
   Computing HKS...
   Computing HKS-SI...
   Done!

 ...

3 vs 4: 2.238990
3 vs 5: 2.119231
3 vs 6: 2.050154
4 vs 5: 3.051406
4 vs 6: 3.014572
5 vs 6: 0.891666
```

If you use this code please cite:

```
 @Article{SimoSerraIJCV2015,
    author = {Edgar Simo-Serra and Carme Torras and Francesc Moreno Noguer},
    title = {{DaLI: Deformation and Light Invariant Descriptor}},
    journal = {International Journal of Computer Vision (IJCV)},
    pages = {1--19},
    year = 2015,
 }
```

Known Issues
------------

Currently it is not possible to run in matlab due to library conflict issues. However, it is possible to run it in octave or integrate it into any C application.


Changelog
---------

January 2015: Initial version 1.0 release


