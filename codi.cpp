#define VOXELS_ENABLED 1

#include <iostream>
#include <random>
#include "network.hpp"


#ifdef VOXELS_ENABLED
	#include "OpenGLWidget.h"
	#include <QtGui/QApplication>
#endif


constexpr int GSize = 30;
constexpr int Max_Iterations = 50;

std::random_device rd;
std::mt19937 rng_gen(rd());

network<GSize> nw;


int main(int argc, char ** argv)
{
#ifdef VOXELS_ENABLED
	//Create and show the Qt OpenGL window
	QApplication app(argc, argv);
	OpenGLWidget openGLWidget(0);
	openGLWidget.show();


	for(int i=0; i<Max_Iterations; i++) {
		std::cout << i << std::endl;
		nw.step_ca();
		nw.render_voxels(openGLWidget);
	}


	//Run the message pump.
	return app.exec();
#else
	for(int i=0; i<Max_Iterations; i++) {
		std::cout << i << std::endl;
		nw.step_ca();
		nw.render_voxels(openGLWidget);
	}

	return 0;
#endif
}
