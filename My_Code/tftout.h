#ifndef CODE_INIT_UTILS_PRINTFTFT_H_
#define CODE_INIT_UTILS_PRINTFTFT_H_
#include "main.h"

#define OUTF out_init()//循环中需要刷新，放在输出开始前
void out_init(void);
void iout(const char *format, ...);

#endif /* CODE_INIT_UTILS_PRINTFTFT_H_ */
