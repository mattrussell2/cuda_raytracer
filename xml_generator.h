#pragma once
#ifndef XML_GENERATOR_H
#define XML_GENERATOR_H
#include<iostream>
#include<string>
#include<sstream>
#include<fstream>
#include<algorithm>
#include<time.h>
#include<stdlib.h>
#include<unordered_map>
using namespace std;


class xml_generator {

	static const int MAX_SPHERES = 500;
public:

	static float uniform(float lo, float hi) {
		return (hi - lo) * (float)rand() / RAND_MAX + lo;
	}

	static string generate_sphere(string name, float scale,	float xtrans, float ztrans,
									float r, float g, float b,float shininess) {
		ostringstream s;
		s << "\n\t\t\t\t<transblock name=\"" << name << "\">\
	\n\t\t\t\t\t<scale x=\"" << to_string(scale) << "\" y=\"" << to_string(scale) << "\" z=\"" << to_string(scale) << "\"/>\
	\n\t\t\t\t\t<translate x=\"" << to_string(xtrans) << "\" y=\"-2.25\" z=\"" << to_string(ztrans) << "\"/>\
    \n\t\t\t\t\t<object type=\"primitive\" name=\"sphere\">\
	\n\t\t\t\t\t<diffuse r=\" "<< r << "\" g=\" " << g << "\" b=\" " << b << "\"/>\
	\n\t\t\t\t\t<reflective r=\" " << r << "\" g=\" " << g << "\" b=\" " << b << "\"/>\
	\n\t\t\t\t\t<shininess value=\" " << shininess << "\"/>\
	\n\t\t\t\t\t</object>\
    \n\t\t\t\t</transblock>";
		return s.str();
	}

	static void generate_xml() {
		ifstream f("./data/spheres.xml");
		if (f.good()) remove("./data/spheres.xml");

		FILE *sphere_file = fopen("./data/spheres.xml", "w+");

		srand(time(NULL));

		fprintf(sphere_file, "<scenefile>\
				<globaldata>\n\
				<diffusecoeff v = \"0.5\" />\n\
				<specularcoeff v = \"0.75\" />\n\
				<ambientcoeff v = \"0.5\" />\n\
				</globaldata>\n\
\n\
				<cameradata>\n\
				<pos x = \"0\" y = \"0.5\" z = \"3.0\" />\n\
				<focus x = \"0\" y = \"0.25\" z = \"-5\" />\n\
				<up x = \"0\" y = \"1\" z = \"0\" />\n\
				</cameradata>\n\
\n\
				<lightdata>\n\
				<id v = \"0\" />\n\
				<type id = \"directional\" />\n\
				<color r = \"1\" g = \"1\" b = \"1\" />\n\
				<direction x = \"0\" y = \"-2\" z = \"2\" />\n\
				</lightdata>\n\
				<lightdata>\n\
				<id v = \"1\" />\n\
				<type id = \"point\" />\n\
				<color r = \"1\" g = \"1\" b = \"1\" />\n\
				<position x = \"0\" y = \"5\" z = \"-5\" />\n\
				</lightdata>\n\
				<object type = \"tree\" name = \"root\">\n\
				\n\t\t<transblock name = \"floor\">\n\
				\n\t\t\t<translate x = \"0\" y = \"-3\" z = \"-5\" />\n\
				\n\t\t\t<scale x = \"20\" y = \"5\" z = \"20\" />\n\
				\n\t\t\t<object type = \"primitive\" name = \"cube\">\n\
				\n\t\t\t<ambient r = \".5\" g = \"0.5\" b = \"0.5\" />\n\
				\n\t\t\t<diffuse r = \"0.5\" g = \"0.5\" b = \"0.5\" />\n\
				\n\t\t\t</object></transblock>\n\
				\n\t\t<transblock name = \"bigSphere\">\n\
				\n\t\t\t<translate x = \"0\" y = \"-3\" z = \"-5\" />\n\
				\n\t\t\t<scale x = \"5\" y = \"5\" z = \"5\" />\n\
				\n\t\t\t<object type = \"primitive\" name = \"sphere\">\n\
				\n\t\t\t<ambient r = \".5\" g = \"0.5\" b = \"0.5\" />\n\
				\n\t\t\t<diffuse r = \"0.5\" g = \"0.5\" b = \"0.5\" />\n\
				\n\t\t\t<reflective r = \"1\" g = \"1\" b = \"1\" />\n\
				\n\t\t\t</object></transblock>\n\ ");
	
		float scale, xtrans, ztrans;
		for (int tree = 0; tree < MAX_SPHERES; tree++) {
			// EACH TREE CAN HAVE A "TYPE" WITH DIFFERENT COLORED LEAVES							
			scale = uniform(0.125, 0.175);
			scale = scale + uniform(-0.02f, 0.02f);

			xtrans = uniform(-30, 30);
			ztrans = uniform(-50, 3);
						
			float r = uniform(0,1);
			float g = uniform(0,1);
			float b = uniform(0,1);

			fprintf(sphere_file, generate_sphere("sphere", scale, xtrans, ztrans, r, g, b,rand() % 15).c_str());
		}

		fprintf(sphere_file, "\n</object>\n</scenefile>");	
		fclose(sphere_file);
		cout << "done generating file" << endl;
	};	
};
#endif
