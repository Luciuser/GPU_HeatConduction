/*
 * @Author: Bingyang Jin
 * @Date: 2022-09-22 13:08:22
 * @Editor: Bingyang Jin
 * @FilePath: /src/01_drawJuliaSet/PNGManager.h
 * @Description: create the file
 */

 /*
  * @Author: Bingyang Jin
  * @Date: 2022-09-22 13:08:22
  * @Editor: Bingyang Jin
  * @FilePath: /src/01_drawJuliaSet/PNGManager.h
  * @Description: create the file
  */

#ifndef __PNG_manager__
#define __PNG_manager__

#include<iostream>
#include<string>

namespace COMMON_LFK {

#define COMMON_COMP 3 // the number of byte of one pixel

	class PNGManager {
	public:
		PNGManager();
		PNGManager(int _weight, int _height, unsigned char* _rgb);
		PNGManager(std::string _name, int _weight, int _height, unsigned char* _rgb);
		~PNGManager();

		int getRGB(int _weight, int _height, unsigned char** outRGB);
		int setData(int _weight, int _height, unsigned char* _rgb);
		int writePNG(std::string fname);

		std::string name = "";
		int weight;
		int height;
		int comp;
		int stride_in_bytes;
		unsigned char* rgb;
	};
	
}

#endif // __PNG_manager__