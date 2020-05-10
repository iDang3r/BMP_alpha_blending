//
//  BMP.cpp
//  BMP_alpha_blending
//
//  Created by Александр on 02.05.2020.
//  Copyright © 2020 Alex. All rights reserved.
//

#include "BMP.hpp"

#define wp(x) cout << setw(13) << #x << ": "; printf("%02vx\n", x);

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

BMP::BMP() : buff_(nullptr), buff_size_(0), image_start_(nullptr), signature(nullptr), errno_(OK) {}

BMP::BMP(const char* name_of_file) : BMP() {
    
    open(name_of_file);
}

BMP::~BMP() {
    
    if (buff_ != nullptr) {
        close();
    }
    
}

void BMP::open(const char* name_of_file) {
    
    if (open_(name_of_file)) {
        get_error_message();
    }
}

void BMP::close() {
    
    if (close_()) {
        get_error_message();
    }
}

void BMP::write(const char* name_of_file) {
    
    if (write_(name_of_file)) {
        get_error_message();
    }
    
}

void BMP::blend(const BMP& im_up) {
    
    if (blend_(im_up)) {
        get_error_message();
    }
    
}

void BMP::blend_xmm(const BMP& im_up) {
    
    if (blend_xmm_(im_up)) {
        get_error_message();
    }
    
}

bool BMP::is_open() {
    return (bool)buff_;
}
    
void BMP::get_error_message() {
    
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
            cout << "FATAL ERROR, ask your programmer for help" <<endl;
            break;
            
        case MEMORY_WAS_NOT_ALLOCATED:
            cout << "Memory for service purpouses was not allocated"<< endl;
            break;
            
        case NOT_BMP_FILE_FORMAT:
            cout << "Format of input file is not BMP" << endl;
            break;
            
        case INCORRECT_BIT_PER_PIXEL:
            cout << "Programm supports working only with 32bit" <<endl;
            break;
            
        case INCORRECT_FILE_TYPE:
            cout << "Programm supports working only with .BMP files"<< endl;
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
            cout << "Programm supports blending with images of thesame size" << endl;
            break;
            
        default:
            cout << "UNDEFINED ERROR" << endl;
            break;
    }
    
    if (errno_)
        exit(0);
    
    errno_ = OK;
}

int BMP::open_(const char* name_of_file) {
    
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

int BMP::close_() {
    
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

int BMP::write_(const char* name_of_file) {
    
    if (name_of_file == nullptr) {
        errno_ = NAME_OF_FILE_IS_NULL;
        return NAME_OF_FILE_IS_NULL;
    }
    
    if (buff_ == nullptr) {
        errno_ = IMAGE_IS_NOT_OPENED;
        return IMAGE_IS_NOT_OPENED;
    }
    FILE* output_file = fopen(name_of_file, "wb");
    size_t wrote_size = fwrite(buff_, sizeof(char), buff_size_,output_file);
    if (wrote_size != buff_size_) {
        errno_ = FAILED_WRITING_TO_FILE;
        return FAILED_WRITING_TO_FILE;
    }
    fclose(output_file);
    
    return OK;
}

int BMP::blend_(const BMP& im_up) {
    
    if (im_up.signature->height_ != signature->height_ || im_up.signature->width_ != signature->width_) {
        errno_ = IMAGES_SIZES_ARE_NOT_EQ;
        return IMAGES_SIZES_ARE_NOT_EQ;
    }
    
    size_t image_size = im_up.signature->height_ * im_up.signature->width_;
    
    for (int i = 0; i < image_size; i++) {
        
        short int
        x1 = image_start_[i].red_,
        x2 = image_start_[i].green_,
        x3 = image_start_[i].blue_,
        x4 = image_start_[i].alpha_,
        
        y1 = im_up.image_start_[i].red_,
        y2 = im_up.image_start_[i].green_,
        y3 = im_up.image_start_[i].blue_,
        y4 = im_up.image_start_[i].alpha_,
        
        a  = im_up.image_start_[i].alpha_;
        
        image_start_[i].red_   = (y1 * a + x1 * (255 - a)) >> 8;
        image_start_[i].green_ = (y2 * a + x2 * (255 - a)) >> 8;
        image_start_[i].blue_  = (y3 * a + x3 * (255 - a)) >> 8;
        image_start_[i].alpha_ = (y4 * a + x4 * (255 - a)) >> 8;
        
    }
    
    return OK;
}

int BMP::blend_xmm_(const BMP& im_up) {
    
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
        
        __m128i low_front = _mm_load_si128((__m128i*) (image_up + (i << 2)));
        __m128i low_back  = _mm_load_si128((__m128i*) (image    + (i << 2)));
    
        __m128i high_front = _mm_srli_si128(low_front, 8);
        __m128i high_back  = _mm_srli_si128(low_back,  8);
    
        low_front  = _mm_cvtepu8_epi16(low_front);
        low_back   = _mm_cvtepu8_epi16(low_back);
        high_front = _mm_cvtepu8_epi16(high_front);
        high_back  = _mm_cvtepu8_epi16(high_back);
        
        __m128i mask = _mm_set_epi8(128, 128, 128, 128, 128, 128,128, 128, 14, 14, 14, 14, 6, 6, 6, 6);
        __m128i low_alpha  = _mm_shuffle_epi8(low_front,  mask);
        __m128i high_alpha = _mm_shuffle_epi8(high_front, mask);
        
        __m128i not_low_alpha = _mm_set1_epi8((char)255);
        not_low_alpha = _mm_andnot_si128(low_alpha, not_low_alpha);
//           not_low_alpha = _mm_sub_epi16(not_low_alpha, low_alpha);
        
        __m128i not_high_alpha = _mm_set1_epi8((char)255);
        not_high_alpha = _mm_andnot_si128(high_alpha, not_high_alpha);
//           not_high_alpha = _mm_sub_epi16(not_high_alpha, high_alpha);
        
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
        
        mask = _mm_set_epi8(128, 128, 128, 128, 128, 128, 128, 128,15, 13, 11, 9, 7, 5, 3, 1);
        low_front  = _mm_shuffle_epi8(low_front, mask);
        high_front = _mm_shuffle_epi8(high_front, mask);
        
        high_front = _mm_movelh_ps(low_front, high_front);
        _mm_storeu_si128((__m128i*) (image + (i << 2)), high_front);
    }
    
    memcpy(image_start_, image, image_size << 2);
    
    free(image);
    free(image_up);
    
    return OK;
}

int BMP::get_buffer(FILE* file) {
    
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
