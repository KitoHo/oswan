/*
$Date$
$Rev$
*/

#ifndef WSBANDAI_H_
#define WSBANDAI_H_

#define BANDAI_X (48) // 配列サイズ
#define BANDAI_Y (48)

RECT bandaiRect[6] = {
    { 5,  9, 16, 23}, // Bのrect(left, top, right, bottom)
    {18,  9, 30, 23}, // A
    {32,  9, 42, 23}, // N
    { 5, 25, 16, 39}, // D
    {18, 25, 30, 39}, // A
    {32, 25, 34, 39}  // I
};

#define W (0xFFFF)
#define R (0xFF00)
static unsigned short bandai[BANDAI_Y][BANDAI_X] = {
/*                      1 1 1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2 2 2 3 3 3 3 3 3 3 3 3 3 4 4 4 4 4 4 4 4 */
/*  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 */
    W,W,W,W,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,W,W,W,W,
    W,W,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,W,W,
    W,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,W,
    W,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,W,
    R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,
    R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,
    R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,
    R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,
    R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,
    R,R,R,R,R,W,W,W,W,W,W,W,W,W,W,R,R,R,R,R,R,R,W,W,W,W,W,R,R,R,R,R,W,W,W,W,R,R,R,R,W,W,W,R,R,R,R,R,
    R,R,R,R,R,W,W,W,W,W,W,W,W,W,W,W,R,R,R,R,R,R,W,W,W,W,W,R,R,R,R,R,W,W,W,W,R,R,R,R,W,W,W,R,R,R,R,R,
    R,R,R,R,R,W,W,W,W,W,W,W,W,W,W,W,R,R,R,R,R,R,W,W,W,W,W,R,R,R,R,R,W,W,W,W,W,R,R,R,W,W,W,R,R,R,R,R,
    R,R,R,R,R,W,W,W,R,R,R,R,R,W,W,W,W,R,R,R,R,W,W,W,R,W,W,W,R,R,R,R,W,W,W,W,W,R,R,R,W,W,W,R,R,R,R,R,
    R,R,R,R,R,W,W,W,R,R,R,R,R,R,W,W,W,R,R,R,R,W,W,W,R,W,W,W,R,R,R,R,W,W,W,W,W,W,R,R,W,W,W,R,R,R,R,R,
    R,R,R,R,R,W,W,W,R,R,R,R,R,W,W,W,R,R,R,R,R,W,W,W,R,W,W,W,R,R,R,R,W,W,W,W,W,W,R,R,W,W,W,R,R,R,R,R,
    R,R,R,R,R,W,W,W,R,R,W,W,W,W,W,W,R,R,R,R,W,W,W,R,R,R,W,W,W,R,R,R,W,W,W,W,W,W,W,R,W,W,W,R,R,R,R,R,
    R,R,R,R,R,W,W,W,R,R,W,W,W,W,W,R,R,R,R,R,W,W,W,R,R,R,W,W,W,R,R,R,W,W,W,R,W,W,W,R,W,W,W,R,R,R,R,R,
    R,R,R,R,R,W,W,W,R,R,W,W,W,W,W,W,R,R,R,R,W,W,W,R,R,R,W,W,W,R,R,R,W,W,W,R,W,W,W,W,W,W,W,R,R,R,R,R,
    R,R,R,R,R,W,W,W,R,R,R,R,R,W,W,W,W,R,R,W,W,W,R,R,R,R,R,W,W,W,R,R,W,W,W,R,R,W,W,W,W,W,W,R,R,R,R,R,
    R,R,R,R,R,W,W,W,R,R,R,R,R,R,W,W,W,R,R,W,W,W,R,R,R,R,R,W,W,W,R,R,W,W,W,R,R,W,W,W,W,W,W,R,R,R,R,R,
    R,R,R,R,R,W,W,W,R,R,R,R,R,W,W,W,W,R,R,W,W,W,R,R,R,R,R,W,W,W,R,R,W,W,W,R,R,R,W,W,W,W,W,R,R,R,R,R,
    R,R,R,R,R,W,W,W,W,W,W,W,W,W,W,W,W,R,W,W,W,W,W,W,W,W,R,R,W,W,W,R,W,W,W,R,R,R,W,W,W,W,W,R,R,R,R,R,
    R,R,R,R,R,W,W,W,W,W,W,W,W,W,W,W,R,R,W,W,W,W,W,W,W,W,R,R,W,W,W,R,W,W,W,R,R,R,R,W,W,W,W,R,R,R,R,R,
    R,R,R,R,R,W,W,W,W,W,W,W,W,W,W,R,R,R,W,W,W,W,W,W,W,W,R,R,W,W,W,R,W,W,W,R,R,R,R,W,W,W,W,R,R,R,R,R,
    R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,
    R,R,R,R,R,W,W,W,W,W,W,W,W,W,W,R,R,R,R,R,R,R,W,W,W,W,W,R,R,R,R,R,W,W,W,R,R,R,R,R,R,R,R,R,R,R,R,R,
    R,R,R,R,R,W,W,W,W,W,W,W,W,W,W,W,R,R,R,R,R,R,W,W,W,W,W,R,R,R,R,R,W,W,W,R,R,R,R,R,R,R,R,R,R,R,R,R,
    R,R,R,R,R,W,W,W,W,W,W,W,W,W,W,W,R,R,R,R,R,R,W,W,W,W,W,R,R,R,R,R,W,W,W,R,R,R,R,R,R,R,R,R,R,R,R,R,
    R,R,R,R,R,W,W,W,R,R,R,R,R,W,W,W,W,R,R,R,R,W,W,W,R,W,W,W,R,R,R,R,W,W,W,R,R,R,R,R,R,R,R,R,R,R,R,R,
    R,R,R,R,R,W,W,W,R,R,R,R,R,R,W,W,W,R,R,R,R,W,W,W,R,W,W,W,R,R,R,R,W,W,W,R,R,R,R,R,R,R,R,R,R,R,R,R,
    R,R,R,R,R,W,W,W,R,R,R,R,R,R,W,W,W,R,R,R,R,W,W,W,R,W,W,W,R,R,R,R,W,W,W,R,R,R,R,R,R,R,R,R,R,R,R,R,
    R,R,R,R,R,W,W,W,R,R,R,R,R,R,W,W,W,R,R,R,W,W,W,R,R,R,W,W,W,R,R,R,W,W,W,R,R,R,R,R,R,R,R,R,R,R,R,R,
    R,R,R,R,R,W,W,W,R,R,R,R,R,R,W,W,W,R,R,R,W,W,W,R,R,R,W,W,W,R,R,R,W,W,W,R,R,R,R,R,R,R,R,R,R,R,R,R,
    R,R,R,R,R,W,W,W,R,R,R,R,R,R,W,W,W,R,R,R,W,W,W,R,R,R,W,W,W,R,R,R,W,W,W,R,R,R,R,R,R,R,R,R,R,R,R,R,
    R,R,R,R,R,W,W,W,R,R,R,R,R,R,W,W,W,R,R,W,W,W,R,R,R,R,R,W,W,W,R,R,W,W,W,R,R,R,R,R,R,R,R,R,R,R,R,R,
    R,R,R,R,R,W,W,W,R,R,R,R,R,R,W,W,W,R,R,W,W,W,R,R,R,R,R,W,W,W,R,R,W,W,W,R,R,R,R,R,R,R,R,R,R,R,R,R,
    R,R,R,R,R,W,W,W,R,R,R,R,R,W,W,W,W,R,R,W,W,W,R,R,R,R,R,W,W,W,R,R,W,W,W,R,R,R,R,R,R,R,R,R,R,R,R,R,
    R,R,R,R,R,W,W,W,W,W,W,W,W,W,W,W,R,R,W,W,W,W,W,W,W,W,R,R,W,W,W,R,W,W,W,R,R,R,R,R,R,R,R,R,R,R,R,R,
    R,R,R,R,R,W,W,W,W,W,W,W,W,W,W,W,R,R,W,W,W,W,W,W,W,W,R,R,W,W,W,R,W,W,W,R,R,R,R,R,R,R,R,R,R,R,R,R,
    R,R,R,R,R,W,W,W,W,W,W,W,W,W,W,R,R,R,W,W,W,W,W,W,W,W,R,R,W,W,W,R,W,W,W,R,R,R,R,R,R,R,R,R,R,R,R,R,
    R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,
    R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,
    R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,
    R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,
    W,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,W,
    W,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,W,
    W,W,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,W,W,
    W,W,W,W,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,W,W,W,W
};
#undef R
#undef W

#endif

