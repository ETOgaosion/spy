//
// Created by 蓝色空间 on 2022/9/1.
//

#ifndef SPY_STRING_H
#define SPY_STRING_H

#define STR_ENABLE              6953475863468
#define STR_DISABLE             229463237649593
#define STR_TARGET              6954048093548
#define STR_CONSOLE             229462175725400
#define STR_CONFIG              6953399250043
#define STR_VERSION             249805560270004805
#define STR_V                   5861512
#define STR_HELP                6951207451432
#define STR_H                   5861498
#define STR_HARDWARE            7572446949630323

// target ops
#define STR_CREATE              6953402479289
#define STR_LIST                6385440353
#define STR_LOAD                6385446277
#define STR_START               210728208851
#define STR_SHUTDOWN            7572924928492385
#define STR_ELIMINATE           249886987627671869
#define STR_STATS               210728208916

unsigned long strhash(unsigned char *str);

#endif //SPY_STRING_H
