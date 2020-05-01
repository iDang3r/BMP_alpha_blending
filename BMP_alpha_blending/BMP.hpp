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
using namespace std;

typedef unsigned char  u_char;
typedef unsigned int   u_int;
typedef unsigned short u_short;

class BMP {
public:
    
    BMP();
    BMP(const char* name_of_file);
    
    ~BMP();
    
    void open(const char* name_of_file);
    
    void close();
    
    bool is_open();
    
private:
    
    char*  buff_;
    size_t buff_size_;
    
    int errno_;
    
    enum ERRORS {
        
        OK                       = 0,
        IMAGE_IS_ALREADY_OPENED  = 1,
        NAME_OF_FILE_IS_NULL     = 2,
        FILE_WAS_NOT_OPEN        = 3,
        IMAGE_IS_ALREADY_CLOSED  = 4,
        FATAL_ERROR              = 5,
        MEMORY_WAS_NOT_ALLOCATED = 6,
        NOT_BMP_FILE_FORMAT      = 7,
        INCORRECT_BIT_PER_PIXEL  = 8,
        INCORRECT_FILE_TYPE      = 9,
        IMAGE_IS_NOT_OPENED      = 10,
        FAILED_WRITING_TO_FILE   = 11,
        INCORRECT_COLORS_COUNT   = 12,
        IMAGES_SIZES_ARE_NOT_EQ  = 13,
        
    };
    
    void get_error_message() {
        
        cout << "BMP::errno_: ";
        switch (errno_) {
            case OK:
                cout << "OK" << endl;
                break;
                
            case IMAGE_IS_ALREADY_OPENED:
                cout << "Image was already opened" << endl;
                break;
                
            case NAME_OF_FILE_IS_NULL:
                cout << "Name of input file is NULL" << endl;
                break;
                
            case FILE_WAS_NOT_OPEN:
                cout << "File was not opened" << endl;
                break;
                
            case IMAGE_IS_ALREADY_CLOSED:
                cout << "Image is already closed" << endl;
                break;
                
            case FATAL_ERROR:
                cout << "FATAL ERROR, ask your programmer for help" << endl;
                break;
                
            case MEMORY_WAS_NOT_ALLOCATED:
                cout << "Memory for service purpouses was not allocated" << endl;
                break;
                
            case NOT_BMP_FILE_FORMAT:
                cout << "Format of input file is not BMP" << endl;
                break;
                
            case INCORRECT_BIT_PER_PIXEL:
                cout << "Programm supports working only with 32bit" << endl;
                break;
                
            case INCORRECT_FILE_TYPE:
                cout << "Programm supports working only with .BMP files" << endl;
                break;
                
            case IMAGE_IS_NOT_OPENED:
                cout << "Open image before writing it" << endl;
                break;
                
            case FAILED_WRITING_TO_FILE:
                cout << "Writing to file was broken" << endl;
                break;
                
            case INCORRECT_COLORS_COUNT:
                cout << "Programm supports working only with fullcolored images" << endl;
                break;
                
            case IMAGES_SIZES_ARE_NOT_EQ:
                cout << "Programm supports blending with images of the same size" << endl;
                break;
                
            default:
                cout << "UNDEFINED ERROR" << endl;
                break;
        }
        
        if (errno_)
            exit(0);
        
        errno_ = OK;
    }
    
    int open_(const char* name_of_file) {
        
        if (buff_ != nullptr) {
            errno_ = IMAGE_IS_ALREADY_OPENED;
            return IMAGE_IS_ALREADY_OPENED;
        }
        
        if (name_of_file == nullptr) {
            errno_ = NAME_OF_FILE_IS_NULL;
            return NAME_OF_FILE_IS_NULL;
        }
        
        FILE* file = fopen(name_of_file, "rb");
        if (file == nullptr) {
            errno_ = FILE_WAS_NOT_OPEN;
            return FILE_WAS_NOT_OPEN;
        }
        
        int CODE = get_buffer(file);
        
        fclose(file);
        
        if (CODE != OK)
            return CODE;
        
        signature = (BMP_signature*)buff_;
        image_start_ = (Pixel*)(buff_ + signature->pixels_adress_);
        
        if (signature->type_[0] != 'B' || signature->type_[1] != 'M') {
            errno_ = INCORRECT_FILE_TYPE;
            return INCORRECT_FILE_TYPE;
        }
        
        if (signature->bit_count_ != 32) {
            errno_ = INCORRECT_BIT_PER_PIXEL;
            return INCORRECT_BIT_PER_PIXEL;
        }
        
        if (signature->colors_used_ != 0) {
            errno_ = INCORRECT_COLORS_COUNT;
            return INCORRECT_COLORS_COUNT;
        }
        
        return OK;
    }
    
    int close_() {
        
        if (buff_ == nullptr) {
            errno_ = IMAGE_IS_ALREADY_CLOSED;
            return IMAGE_IS_ALREADY_CLOSED;
        }
        
        if (signature == nullptr) {
            errno_ = FATAL_ERROR;
            return FATAL_ERROR;
        }
        
        free(buff_);
        
        buff_ = nullptr;
        buff_size_ = 0;
        signature = nullptr;
        image_start_ = nullptr;
        errno_ = OK;
        
        return OK;
    }
    
    int write_(const char* name_of_file) {
        
        if (name_of_file == nullptr) {
            errno_ = NAME_OF_FILE_IS_NULL;
            return NAME_OF_FILE_IS_NULL;
        }
        
        if (buff_ == nullptr) {
            errno_ = IMAGE_IS_NOT_OPENED;
            return IMAGE_IS_NOT_OPENED;
        }

        FILE* output_file = fopen(name_of_file, "wb");
        size_t wrote_size = fwrite(buff_, sizeof(char), buff_size_, output_file);
        if (wrote_size != buff_size_) {
            errno_ = FAILED_WRITING_TO_FILE;
            return FAILED_WRITING_TO_FILE;
        }

        fclose(output_file);
        
        return OK;
    }
    
    void blend_(const BMP& im_up) {
        
        for (int i = 0; i < 600 * 800; i++) {
            
            short int x1, x2, x3, x4, a;
            short int y1, y2, y3, y4;
            x1 = image_start_[i].red_;
            x2 = image_start_[i].green_;
            x3 = image_start_[i].blue_;
            x4 = image_start_[i].alpha_;
            
            y1 = im_up.image_start_[i].red_;
            y2 = im_up.image_start_[i].green_;
            y3 = im_up.image_start_[i].blue_;
        a = y4 = im_up.image_start_[i].alpha_;
            
            image_start_[i].red_   = (y1 * a + x1 * (255 - a)) >> 8;
            image_start_[i].green_ = (y2 * a + x2 * (255 - a)) >> 8;
            image_start_[i].blue_  = (y3 * a + x3 * (255 - a)) >> 8;
            image_start_[i].alpha_ = (y4 * a + x4 * (255 - a)) >> 8;
            
        }
        
    }
    
#define wp(x) cout << setw(13) << #x << ": "; printf("%02vx\n", x);
    
    int blend_xmm_(const BMP& im_up) {
        
        if (im_up.signature->height_ != signature->height_ || im_up.signature->width_ != signature->width_) {
            errno_ = IMAGES_SIZES_ARE_NOT_EQ;
            return IMAGES_SIZES_ARE_NOT_EQ;
        }
        
        size_t image_size = im_up.signature->height_ * im_up.signature->width_;
        
        char* image_up = (char*)calloc(image_size << 2, sizeof(char));
        memcpy(image_up, im_up.image_start_, image_size << 2);
        
        char* image = (char*)calloc(image_size << 2, sizeof(char));
        memcpy(image, image_start_, image_size << 2);
        
        for (int i = 0; i < image_size; i += 4) {
            
            __m128i low_front = _mm_load_si128((__m128i*) (image_up + i * 4));
            __m128i low_back  = _mm_load_si128((__m128i*) (image + i * 4));
        
            __m128i high_front = _mm_srli_si128(low_front, 8);
            __m128i high_back  = _mm_srli_si128(low_back,  8);
        
            low_front = _mm_cvtepu8_epi16(low_front);
            low_back  = _mm_cvtepu8_epi16(low_back);

            high_front = _mm_cvtepu8_epi16(high_front);
            high_back  = _mm_cvtepu8_epi16(high_back);

            __m128i mask = _mm_set_epi8(128, 128, 128, 128, 128, 128, 128, 128, 14, 14, 14, 14, 6, 6, 6, 6);
            __m128i low_alpha  = _mm_shuffle_epi8(low_front,  mask);
            __m128i high_alpha = _mm_shuffle_epi8(high_front, mask);
            
            __m128i not_low_alpha = _mm_set1_epi8((char)255);
            not_low_alpha = _mm_andnot_si128(low_alpha, not_low_alpha);
//            not_low_alpha = _mm_sub_epi16(not_low_alpha, low_alpha);

            __m128i not_high_alpha = _mm_set1_epi8((char)255);
            not_high_alpha = _mm_andnot_si128(high_alpha, not_high_alpha);
//            not_high_alpha = _mm_sub_epi16(not_high_alpha, high_alpha);

            low_alpha = _mm_cvtepu8_epi16(low_alpha);
            not_low_alpha = _mm_cvtepu8_epi16(not_low_alpha);

            high_alpha = _mm_cvtepu8_epi16(high_alpha);
            not_high_alpha = _mm_cvtepu8_epi16(not_high_alpha);

            low_front  = _mm_mullo_epi16(low_front, low_alpha);
            low_back   = _mm_mullo_epi16(low_back,  not_low_alpha);

            high_front = _mm_mullo_epi16(high_front, high_alpha);
            high_back  = _mm_mullo_epi16(high_back,  not_high_alpha);

            low_front  = _mm_add_epi16(low_front,  low_back);
            high_front = _mm_add_epi16(high_front, high_back);
            
            mask = _mm_set_epi8(128, 128, 128, 128, 128, 128, 128, 128, 15, 13, 11, 9, 7, 5, 3, 1);
            low_front  = _mm_shuffle_epi8(low_front, mask);
            high_front = _mm_shuffle_epi8(high_front, mask);

            high_front = _mm_movelh_ps(low_front, high_front);

            _mm_storeu_si128((__m128i*) (image + i * 4), high_front);
        }
        
        memcpy(image_start_, image, image_size << 2);
        
        free(image);
        free(image_up);
        
        return OK;
    }
    
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
    
    BMP_signature* signature;
    Pixel* image_start_;
    
    int get_buffer(FILE* file) {
        
        assert(file != nullptr);

        fseek(file, 0, SEEK_END);
        buff_size_ = ftell(file);
        rewind(file);
        
        assert(buff_ == nullptr);
        buff_ = (char*)calloc(buff_size_ + 1, sizeof(char));
        if (buff_ == nullptr) {
            errno_ = MEMORY_WAS_NOT_ALLOCATED;
            return MEMORY_WAS_NOT_ALLOCATED;
        }

        size_t got_size = fread(buff_, sizeof(char), buff_size_, file);
        if (buff_size_ != got_size) {
            errno_ = FATAL_ERROR;
            return FATAL_ERROR;
        }

        return OK;
    }
    
};


#endif /* BMP_hpp */
