INC=--include-path
CUDA=/usr/local/cuda-9.0/include/
CUDA_HELPER=/usr/local/cuda-9.0/include/cuda-samples/Common
INC_ALL=${INC} ${CUDA} ${INC} ${CUDA_HELPER}
CCFLAG=-std=c++11
#FLFLAG=-I/home/matt/Desktop/cuda_raytracer/FL/
FLCXX=-I/home/matt/Desktop/cuda_raytracer/glm -I/usr/local/include -I/usr/include/freetype2 -I/usr/include/libpng16 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_THREAD_SAFE -D_REENTRANT
#FLXTRA=
LKFLAG=-L/usr/local/lib -lfltk -lXrender -lXcursor -lXfixes -lXext -lXft -lfontconfig -lXinerama -lpthread -ldl -lm -lX11 `fltk-config --use-forms --use-gl --use-images --ldflags` #-L/home/matt/Desktop/cuda_raytracer/lib -lfltk -lXext -lX11 -lm

ppm.o:
	nvcc $(CCFLAG) -o ppm.o -c scene/ppm.cpp

tinystr.o:
	nvcc $(CCFLAG) -o tinystr.o -c scene/tinystr.cpp

tinyxml.o:
	nvcc $(CCFLAG) -o tinyxml.o -c scene/tinyxml.cpp

tinyxmlparser.o:
	nvcc $(CCFLAG) -o tinyxmlparser.o -c scene/tinyxmlparser.cpp

sceneparser.o: tinyxmlparser.o
	nvcc $(CCFLAG) $(FLCXX) -o sceneparser.o -c scene/SceneParser.cpp 

camera.o:
	nvcc $(CCFLAG) $(FLCXX) -o camera.o -c Camera.cpp 

canvas.o: MyAppWindow.h MyGLCanvas.h 
	nvcc $(CCFLAG) $(FLCXX) -o canvas.o -c MyGLCanvas.cpp

cu_main.o:
	nvcc $(CCFLAG) $(FLCXX) -c cu_main.cu ${INC_ALL}

main.o:
	nvcc $(CCFLAG) $(FLCXX) -c main.cpp

raytracer: main.o canvas.o camera.o cu_main.o tinyxmlparser.o sceneparser.o tinyxml.o tinystr.o ppm.o
	nvcc $(CCFLAG) $(FLCXX) main.o canvas.o camera.o cu_main.o tinyxmlparser.o sceneparser.o tinyxml.o tinystr.o ppm.o ${INC_ALL} ${LKFLAG}

clean:
	rm *.o
