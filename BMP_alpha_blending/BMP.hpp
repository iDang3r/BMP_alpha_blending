//
//  BMP.hpp
//  BMP_alpha_blending
//
//  Created by Александр on 02.05.2020.
//  Copyright © 2020 Alex. All rights reserved.
//

#ifndef BMP_hpp
#define BMP_hpp

#include <cstdio>
#include <iostream>
#include <cassert>
#include <immintrin.h>
#include <emmintrin.h>
#include <iomanip>
using std::cin;
using std::cout;
using std::dec;
using std::hex;
using std::endl;

typedef unsigned char  u_char;
typedef unsigned int   u_int;
typedef unsigned short u_short;

class BMP {
public:
    
    /* <<---- METHODS ---->> */
    
    BMP();
    BMP(const char* name_of_file);
    
    ~BMP();
    
    void open(const char* name_of_file);
    
    void close();
    
    void write(const char* name_of_file);
    
    void blend(const BMP& im_up);
    
    void blend_xmm(const BMP& im_up);
    
    bool is_open();
    
private:
    
    /* <<---- CLASSES ---->> */
    
    #pragma pack(push, 1)
    class BMP_signature {
    public:
        
        u_char  type_[2];
        u_int   file_size_;
        u_int   reserved_;
        u_int   pixels_adress_;
         
        u_int   header_size_;
        u_int   width_;
        u_int   height_;
        u_short planes_;
        u_short bit_count_;
        //--->
        u_int   compression_;
        u_int   image_size_;
        u_int   X_pels_per_meter_;
        u_int   Y_pels_per_meter_;
        u_int   colors_used_;
        u_int   colors_important_;
        
        // there can be a color table
        
        BMP_signature()  = default;
        ~BMP_signature() = default;
        
    };
    #pragma pack(pop)
    
    #pragma pack(push, 1)
    class Pixel {
    public:
        
        u_char blue_;
        u_char green_;
        u_char red_;
        u_char alpha_;
        
        Pixel()  = default;
        ~Pixel() = default;
        
        void print(bool hex = false) {
            
            if (hex)
                cout << hex;
            
            cout << (int)red_ << "." << (int)green_ << "." << (int)blue_ << "." << (int)alpha_ << endl;
            
            if (hex)
                cout << dec;
        }
    };
    #pragma pack(pop)
    
    /* <<---- FIELDS ---->> */
    
    char*  buff_;
    size_t buff_size_;
    
    BMP_signature* signature;
    Pixel* image_start_;
    
    int errno_;
    
    /* <<---- METHODS ---->> */
    
    void get_error_message();
    
    int open_(const char* name_of_file);
    
    int close_();
    
    int write_(const char* name_of_file);
    
    int blend_(const BMP& im_up);
    
    int blend_xmm_(const BMP& im_up);
    
    int get_buffer(FILE* file);
    
};

#endif /* BMP_hpp */
