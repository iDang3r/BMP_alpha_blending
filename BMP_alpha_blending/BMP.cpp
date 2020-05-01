//
//  BMP.cpp
//  BMP_alpha_blending
//
//  Created by Александр on 02.05.2020.
//  Copyright © 2020 Alex. All rights reserved.
//

#include "BMP.hpp"

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

bool BMP::is_open() {
    return (bool)buff_;
}

