//#include "cuda_wrapper_implementation.h"
#include "MyAppWindow.h"
#include "screen.h"

int main(int argc, char **argv) {	
	win = new MyAppWindow(screenWidth,screenHeight, "Camera");
	win->resizable(win);
	Fl::add_idle(MyAppWindow::idleCB);
	win->show();
	return(Fl::run());
}
