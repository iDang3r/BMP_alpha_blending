//
//  main.cpp
//  BMP_alpha_blending
//
//  Created by Александр on 01.05.2020.
//  Copyright © 2020 Alex. All rights reserved.
//

#include "BMP.hpp"

int main1(int argc, const char * argv[]) {

    BMP image_red  ("/Users/alex/Desktop/red.bmp");
    BMP image_green("/Users/alex/Desktop/green.bmp");
    BMP image_blue ("/Users/alex/Desktop/blue.bmp");

    image_red.blend(image_green);
    image_red.blend(image_blue);

    image_red.write("/Users/alex/Desktop/circles.bmp");

    image_red.close();
    image_green.close();
    image_blue.close();

    cout << "OK" << endl;

    return 0;
}

int main(int argc, const char * argv[]) {
    
    BMP image("/Users/alex/Desktop/zxc.bmp");
    BMP image_up("/Users/alex/Desktop/kot.bmp");
    
//    image.blend(image_up);
    image.blend_xmm(image_up);
    
    image.write("/Users/alex/Desktop/kot_copy.bmp");
    
    image.close();
    image_up.close();
    
    cout << "OK" << endl;
    
    return 0;
}
