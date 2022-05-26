#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include "lodepng.h"

#define TEST_P1 1
#define TEST_P2 1

extern char *yourName;
extern char *yourStudentID;

// These two functions should be implemented in lab2.cpp
void imageThresholding(unsigned char* buffer, int dim, unsigned char threshold);
void imageRotation(unsigned int* image, int dim);

int main() {
    int i, j;
    bool failed = false;

    std::vector<unsigned char> png;
    std::vector<unsigned char> image;
    std::vector<unsigned char> image2;
    unsigned int width, height;
    //lodepng::State state;
    std::string filename, outFilename;

    printf("Your Name: %s\nYour Student ID: %s\n", yourName, yourStudentID);

#if TEST_P1
    // Part 1 tests
	std::cout << "" << std::endl;
        
    unsigned char testStr1_1[3][3] = {{  0,  30,  60},
                                      { 90, 120, 150},
                                      {180, 210, 240}};

    unsigned char exptectedStr1_1[3][3] = {{  0,   0,   0},
                                           {255, 255, 255},
                                           {255, 255, 255}};

    imageThresholding(&testStr1_1[0][0], 3, 90);
    for (i = 0; i < 3; ++i)
        for (j = 0; j < 3; ++j) {
            if (testStr1_1[i][j] != exptectedStr1_1[i][j]) {
                std::cout << "Part 1: Test 1 failed at index: " << i << "," << j << " got: " << int(testStr1_1[i][j]) << " expected: " << int(exptectedStr1_1[i][j]) << std::endl;
                failed = true;
            }
        }
    if (!failed)
        std::cout << "Part 1: Test 1 passed." << std::endl;
    

    unsigned char testStr1_2[3][3] = {{  0,  30,  60 },
                                      { 90, 120, 150 },
                                      {180, 210, 240 }};

    unsigned char exptectedStr1_2[3][3] = {{  0,   0,   0},
                                           {  0,   0, 255},
                                           {255, 255, 255}};
    imageThresholding(&testStr1_2[0][0], 3, 128);
    for (i = 0; i < 3; ++i)
        for (j = 0; j < 3; ++j) {
            if (testStr1_2[i][j] != exptectedStr1_2[i][j]) {
                std::cout << "Part 1: Test 2 failed at index: " << i << "," << j <<
                    " got: " << int(testStr1_2[i][j]) << " expected: " << int(exptectedStr1_2[i][j]) << std::endl;
                failed = true;
            }
        }
    if (!failed)
        std::cout << "Part 1: Test 2 passed." << std::endl;


    unsigned char testStr1_3[3][3] = {{   0,  30,  60},
                                      {  90, 120, 150},
                                      { 180, 210, 240}};

    unsigned char exptectedStr1_3[3][3] = {{  0,   0,   0},
                                           {  0,   0,   0},
                                           {255, 255, 255}};

    imageThresholding(&testStr1_3[0][0], 3, 180);
    for (i = 0; i < 3; ++i)
        for (j = 0; j < 3; ++j) {
            if (testStr1_3[i][j] != exptectedStr1_3[i][j]) {
                std::cout << "Part 1: Test 3 failed at index: " << i << "," << j <<
                    " got: " << int(testStr1_3[i][j]) << " expected: " << int(exptectedStr1_3[i][j]) << std::endl;
                failed = true;
            }
        }
    if (!failed)
        std::cout << "Part 1: Test 3 passed." << std::endl;



    unsigned char testStr1_4[4][4] = {{  0,  30,  90, 120},
                                      {127,  45, 130,  60},
                                      {127, 128,   0, 130},
                                      { 27, 228, 255, 255}};
    unsigned char exptectedStr1_4[4][4] = {{0,   0,   0,   0},
                                           {0,   0, 255,   0},
                                           {0, 255,   0, 255},
                                           {0, 255, 255, 255}};
    failed = false;
    imageThresholding(&testStr1_4[0][0], 4, 128);
    for (i = 0; i < 4; ++i)
        for (j = 0; j < 4; ++j) {
            if (testStr1_4[i][j] != exptectedStr1_4[i][j]) {
                std::cout << "Part 1: Test 4 failed at index: " << i << "," << j <<
                    " got: " <<int(testStr1_4[i][j]) << " expected: " << int(exptectedStr1_4[i][j]) << std::endl;
                failed = true;
            }
        }
    if (!failed)
        std::cout << "Part 1: Test 4 passed." << std::endl;


    unsigned char testStr1_5[4][4] = {{ 0,   30,  90, 120},
                                      { 12,  45, 130,  60},
                                      { 12, 128,  90, 190},
                                      { 27, 228, 255, 255}};
    unsigned char exptectedStr1_5[4][4] = {{0,   0,   0,   0},
                                           {0,   0,   0,   0},
                                           {0,   0,   0,   0},
                                           {0, 255, 255, 255}};
    failed = false;
    imageThresholding(&testStr1_5[0][0], 4, 200);
    for (i = 0; i < 4; ++i)
        for (j = 0; j < 4; ++j) {
            if (testStr1_5[i][j] != exptectedStr1_5[i][j]) {
                std::cout << "Part 1: Test 5 failed at index: " << i << "," << j <<
                    " got: " <<int(testStr1_5[i][j]) << " expected: " << int(exptectedStr1_5[i][j]) << std::endl;
                failed = true;
            }
        }
    if (!failed)
        std::cout << "Part 1: Test 5 passed." << std::endl;

    filename = "lena.png";
    lodepng::decode(image, width, height, filename, LCT_GREY, 8U);
    if (image.size() == 0) {
        std::cerr << "lena.png not found. Make sure it exists in your solution directory." << std::endl;
        return -1;
    }
    assert(width == height);
    imageThresholding((unsigned char*)&image[0], width, 128);
    outFilename = "lena_binary.png";
    lodepng::encode(outFilename, image, width, height, LCT_GREY, 8U);
    image.clear();
    std::cout << "Check out lena_binary.png." << std::endl;

#endif

#if TEST_P2
    // Part 2 tests
	std::cout << "" << std::endl;

    unsigned int testStr2_1[3][3] = {{0x00000000, 0x10101010, 0x20202020},
                                     {0x30303030, 0x40404040, 0x50505050},
                                     {0x60606060, 0x70707070, 0x80808080}};


    unsigned int exptectedStr2_1[3][3] = {{0x20202020,  0x50505050, 0x80808080},
                                          {0x10101010,  0x40404040, 0x70707070},
                                          {0x00000000,  0x30303030, 0x60606060}};

    imageRotation(&testStr2_1[0][0], 3);
    failed = false;
    for (i = 0; i < 3; ++i)
    for (j = 0; j < 3; ++j) {
        if (testStr2_1[i][j] != exptectedStr2_1[i][j]) {
            std::cout << std::hex << "Part 2: Test 1 failed at index: "<< i<< ","<< j <<
                " got(Hex): " << testStr2_1[i][j] <<" expected(Hex): " << exptectedStr2_1[i][j]<<  std::endl;
            failed = true;
        }
    }
    if (!failed)
        std::cout << "Part 2: Test 1 passed." << std::endl;



    unsigned int testStr2_2[3][3] = {{0x00000000, 0x10101010, 0x20202020},
                                     {0x30303030, 0x40404040, 0x50505050},
                                     {0x60606060, 0x70707070, 0x80808080}};
    unsigned int exptectedStr2_2[3][3] = {{ 0x20202020, 0x50505050, 0x80808080},
                                          { 0x10101010, 0x40404040, 0x70707070},
                                          { 0x00000000, 0x30303030, 0x60606060}};
    imageRotation(&testStr2_2[0][0], 3);
    failed = false;
    for (i = 0; i < 3; ++i)
        for (j = 0; j < 3; ++j) {
            if (testStr2_2[i][j] != exptectedStr2_2[i][j]) {
                std::cout << std::hex<< "Part 2: Test 2 failed at index: " << i << "," << j <<
                    " got(Hex): " << testStr2_2[i][j] << " expected(Hex): " << exptectedStr2_2[i][j] << std::endl;
                failed = true;
            }
        }
    if (! failed)
        std::cout << "Part 2: Test 2 passed." << std::endl;


    unsigned int testStr2_3[4][4] = {{0, 1,   2,  3},
                                     {4, 5,   6,  7},
                                     {8, 9,  10, 11},
                                     {12, 13, 14, 15}};
    unsigned int exptectedStr2_3[4][4] = {{3, 7, 11, 15},
                                          {2, 6, 10, 14},
                                          {1, 5, 9, 13},
                                          {0, 4, 8, 12}};
    failed = false;
    imageRotation(&testStr2_3[0][0], 4);
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            if (testStr2_3[i][j]!= exptectedStr2_3[i][j]) {
                std::cout << "Part 2: Test 3 failed at index: " << i <<","<<j<<
                    " got(Hex): " << testStr2_3[i][j] << " expected(Hex): " << exptectedStr2_3[i][j] << std::endl;
                failed = true;
            }
        }
    }
    if (!failed)
        std::cout << "Part 2: Test 3 passed." << std::endl;


    unsigned int testStr2_4[4][4] = {{ 10, 1,   2, 13 },
                                     { 4,  5,   6,  7 },
                                     { 8,  9,  10, 11 },
                                     { 12, 13, 14, 15 }};
    unsigned int exptectedStr2_4[4][4] = {{13, 7, 11, 15},
                                          {2,  6, 10, 14},
                                          {1,  5, 9, 13},
                                          {10, 4, 8, 12}};
    failed = false;
    imageRotation(&testStr2_4[0][0], 4);
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            if (testStr2_4[i][j] != exptectedStr2_4[i][j]) {
                std::cout << "Part 2: Test 4 failed at index: " << i << "," << j <<
                    " got(Hex): " << testStr2_4[i][j] << " expected(Hex): " << exptectedStr2_4[i][j] << std::endl;
                failed = true;
            }
        }
    }
    if (!failed)
        std::cout << "Part 2: Test 4 passed." << std::endl;

    // Load a RGBA png (4-Bytes per pixel)
    // Rotate it to left
    filename = "authoritah.png";
    lodepng::decode(image, width, height, filename, LCT_RGBA, 8U);
    if (image.size() == 0) {
        std::cerr << "authoritah.png not found. Make sure you have this file in your solution directory." << std::endl;
        return -1;
    }
    assert(width == height);
    imageRotation((unsigned int*)&image[0], width);
    outFilename = "authoritah_rotated.png";
    lodepng::encode(outFilename, image, width, height, LCT_RGBA, 8U);
    std::cout << "Check out authoritah_rotated.png." << std::endl;
    #endif

	std::cout << "" << std::endl;

    return 0;
}