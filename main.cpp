//#include "cuda_wrapper_implementation.h"
#include "MyAppWindow.h"
#include "screen.h"

//todo: integrate camera with cuda.
//      add cylinder intersections.
//      add movement capabilities.
//      figure out efficiency w/ thread initialization.


int main(int argc, char **argv) {	
	win = new MyAppWindow(screenWidth,screenHeight, "Camera");
	win->resizable(win);
	Fl::add_idle(MyAppWindow::idleCB);
	win->show();
	return(Fl::run());
}