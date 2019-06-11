# cuda_raytracer

A recursive ray tracer written in C++ and Cuda C\
by Matt Russell\
May 2019\
\
I completed this as an extension of a final project in the graphics course at Tufts [175].\
\
The code relies on fltk for the graphical interface, and uses glm for standard matrix/vector operations.\
It requires a [cuda gpu](https://developer.nvidia.com/cuda-gpus)\
When running, simply type a name of any of the xml files in the data folder [i.e. "cones.xml"].\
Default recursion depth is 50.\
\
Feel free to generate your own cool scene files.\
\
With gratitude to Remco Chang from Tufts for teaching the graphics course, along with and Roger Alan and Peter Shirley for their great guides on cuda and ray tracing, respectively. Also to those who wrote various bits of the scene parsing code for xml files and ppm files - Nong Li, Remco Chang, Michael Shah, Eric Tamura, Yves Berquin, Tyge Løvset, and Lee Thomason.\
\
Also thanks to Rohan Deshpande and Ashley Suh, my project partners.
