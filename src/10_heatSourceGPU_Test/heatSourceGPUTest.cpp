/*
 * @Author: Bingyang Jin
 * @Date: 2022-10-26 20:35:07
 * @Editor: Bingyang Jin
 * @FilePath: /src/10_heatSourceGPU_Test/heatSourceGPUTest.cpp
 * @Description: create the file
 */

#include<iostream>

extern int drawWithGLUT(int argc, char** argv);

int main(int argc, char** argv) {
	
	std::cout << "Begin" << std::endl;

	drawWithGLUT(argc, argv);

	std::cout << "Finish" << std::endl;

	return 0;
}