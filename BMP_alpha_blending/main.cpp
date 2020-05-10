//
//  main.cpp
//  BMP_alpha_blending
//
//  Created by Александр on 01.05.2020.
//  Copyright © 2020 Alex. All rights reserved.
//

#include "BMP.hpp"

int main(int argc, const char * argv[]) {
    
    BMP image("Images/field.bmp");
    BMP image_up("Images/kot.bmp");
    
//    image.blend(image_up);
    image.blend_xmm(image_up);
    
    image.write("Images/kot_copy.bmp");
    
    image.close();
    image_up.close();
    
    cout << "OK" << endl;
    cout << "Best wishes" << endl;
    
    return 0;
}
