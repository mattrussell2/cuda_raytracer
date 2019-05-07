#ifndef MYAPPWINDOWH
#define MYAPPWINDOWH
#include <string>
#include <iostream>
#include <fstream>
#include <math.h>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Round_Button.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/names.h>
#include <FL/gl.h>
#include <FL/glut.h>
#include <FL/glu.h>

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