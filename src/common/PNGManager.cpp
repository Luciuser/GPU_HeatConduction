#include "PNGManager.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include"stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"

COMMON_LFK::PNGManager::PNGManager()
{
	weight = -1;
	height = -1;
	comp = -1;
	stride_in_bytes = -1;
	rgb = nullptr;
}

COMMON_LFK::PNGManager::PNGManager(int _weight, int _height, unsigned char * _rgb)
{
	weight = _weight;
	height = _height;
	comp = COMMON_COMP;
	stride_in_bytes = weight * comp;
	rgb = new unsigned char[weight * height * comp];
	for (int i = 0; i < weight*height*comp; i++) {
		rgb[i] = _rgb[i];
	}
}

COMMON_LFK::PNGManager::PNGManager(std::string _name, int _weight, int _height, unsigned char * _rgb)
{
	name = _name;
	weight = _weight;
	height = _height;
	comp = COMMON_COMP;
	stride_in_bytes = weight * comp;
	rgb = new unsigned char[weight * height * comp];
	for (int i = 0; i < weight*height*comp; i++) {
		rgb[i] = _rgb[i];
	}
}

COMMON_LFK::PNGManager::~PNGManager()
{
	weight = -1;
	height = -1;
	comp = -1;
	stride_in_bytes = -1;
	if (rgb != nullptr) {
		delete[] rgb;
		rgb = nullptr;
	}
}

int COMMON_LFK::PNGManager::getRGB(int _weight, int _height, unsigned char ** outRGB)
{
	(*outRGB) = new unsigned char[comp];
	for (int i = 0; i < comp; i++) {
		(*outRGB)[i] = rgb[weight * _height + _weight + i];
	}

	return 0;
}

int COMMON_LFK::PNGManager::setData(int _weight, int _height, unsigned char * _rgb)
{
	weight = _weight;
	height = _height;
	comp = COMMON_COMP;
	stride_in_bytes = weight * comp;
	rgb = new unsigned char[weight * height * comp];
	for (int i = 0; i < weight*height*comp; i++) {
		rgb[i] = _rgb[i];
	}
	return 0;
}

int COMMON_LFK::PNGManager::writePNG(std::string fname)
{
	if (comp == -1) {
		std::cout << "please initial png data" << std::endl;
		return 0;
	}

	stbi_write_png(fname.c_str(), weight, height, comp, rgb, stride_in_bytes);
	return 0;
}
