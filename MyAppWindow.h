#ifndef MYAPPWINDOWH
#define MYAPPWINDOWH
#include <string>
#include <iostream>
#include <fstream>
#include <math.h>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/names.h>
#include <FL/gl.h>
#include <FL/glut.H>
#include <FL/glu.h>

#include <FL/Fl_Window.H>
#include "MyGLCanvas.h"
#include "screen.h"
class MyAppWindow;
MyAppWindow *win;

class MyAppWindow : public Fl_Window {

public:	
	MyAppWindow(int W, int H, const char*L = 0);
	static void idleCB(void* userdata) {
		win->canvas->draw();		
	}

private:
	MyGLCanvas *canvas;

};

MyAppWindow::MyAppWindow(int W, int H, const char*L) : Fl_Window(W, H, L) {
	begin();
	int width = screenWidth;
	int height = screenHeight;
	canvas = new MyGLCanvas(10, 10, width, height - 20);
}
#endif
