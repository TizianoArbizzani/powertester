#include <pgmspace.h>

// Icon width and height
const uint16_t IconMaxHold_width = 0x20;
const uint16_t IconMaxHold_height = 0x20;

// Icon Data
const unsigned short IconMaxHold[0x400] PROGMEM = {
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xC639, 0xC639, 0xCE59, 0xCE59, 0xCE79, 0xCE79,   // 0x0010 (16) pixels
0xCE59, 0xCE59, 0xCE59, 0xC659, 0xCE59, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0020 (32) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xC639, 0xCE59, 0xD69A, 0xDEDB, 0xE71C, 0xE73D, 0xEF5D, 0xEF5E,   // 0x0030 (48) pixels
0xEF5D, 0xEF5D, 0xE71C, 0xDEDB, 0xCE7A, 0xCE59, 0xC639, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0040 (64) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xC639, 0xCE59, 0xD69A, 0xDEFC, 0xE73D, 0xEF7E, 0xF79E, 0xF7BF, 0xF7BF, 0xF7BF,   // 0x0050 (80) pixels
0xF7BF, 0xF7BF, 0xF7BF, 0xF79E, 0xEF7E, 0xE71C, 0xCE9A, 0xC639, 0xC639, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0060 (96) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xC639, 0xCE59, 0xDEDB, 0xE71C, 0xEF5D, 0xEF9E, 0xF79E, 0xF7BF, 0xF7BF, 0xF7DF, 0xFFDF,   // 0x0070 (112) pixels
0xFFDF, 0xFFDF, 0xFFDF, 0xFFDF, 0xF7BF, 0xF79F, 0xEF5E, 0xDEDB, 0xCE59, 0xC618, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0080 (128) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0xC639, 0xCE79, 0xDEDB, 0xE73C, 0xEF5D, 0xEF7E, 0xEF9E, 0xF79E, 0xF7BF, 0xF7BF, 0xF7BF, 0xFFDF,   // 0x0090 (144) pixels
0xFFDF, 0xFFDF, 0xFFFF, 0xFFFF, 0xFFDF, 0xF7DF, 0xF7BF, 0xEF7E, 0xDF1C, 0xCE59, 0xBDF7, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x00A0 (160) pixels
0x0000, 0x0000, 0x0000, 0xC639, 0xCE59, 0xDEDB, 0xE71C, 0xE75D, 0xEF5D, 0xEF7E, 0xEF7E, 0xF79E, 0xF79E, 0xF7BF, 0xF7BF, 0xF7BF,   // 0x00B0 (176) pixels
0xF7DF, 0xFFDF, 0xFFDF, 0xFFDF, 0xFFFF, 0xFFFF, 0xF7BF, 0xF7BF, 0xEF7E, 0xDEFC, 0xC639, 0xB5B7, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x00C0 (192) pixels
0x0000, 0x0000, 0xC639, 0xCE59, 0xD6BB, 0xE71C, 0xE73D, 0xEF5D, 0xEF5D, 0xEF5D, 0xEF7E, 0xF79E, 0xF79E, 0xF79E, 0xF7BF, 0xF7BF,   // 0x00D0 (208) pixels
0xF7BF, 0xF7DF, 0xFFDF, 0xFFDF, 0xFFDF, 0xFFFF, 0xFFFF, 0xF7BF, 0xF79F, 0xEF5E, 0xDEDB, 0xC618, 0xAD76, 0x0000, 0x0000, 0x0000,   // 0x00E0 (224) pixels
0x0000, 0x0000, 0xC639, 0xCE7A, 0xDEFC, 0xE73D, 0xEF5D, 0xEF5D, 0xEF5D, 0xEF5D, 0xEF7E, 0xEF7E, 0xEF7E, 0xE71C, 0xBDD7, 0xBDD7,   // 0x00F0 (240) pixels
0xBDF7, 0xF79E, 0xF7BF, 0xF7DF, 0xFFDF, 0xFFDF, 0xFFFF, 0xFFFF, 0xF7BF, 0xEF7E, 0xE73D, 0xD69A, 0xAD75, 0x0000, 0x0000, 0x0000,   // 0x0100 (256) pixels
0x0000, 0xC639, 0xC659, 0xD6BB, 0xE71C, 0xEF5D, 0xEF5D, 0xEF5D, 0xEF5D, 0xEF5D, 0xEF7D, 0xEF7E, 0xEF7E, 0xC639, 0x9CD3, 0x9CD3,   // 0x0110 (272) pixels
0x9CD3, 0xDEFB, 0xF7BF, 0xF7BF, 0xF7BF, 0xF7DF, 0xF7DF, 0xFFFF, 0xFFFF, 0xF79E, 0xEF5D, 0xDEFC, 0xC638, 0x8430, 0x0000, 0x0000,   // 0x0120 (288) pixels
0x0000, 0xC639, 0xCE59, 0xDEFC, 0xE73D, 0xEF7E, 0xEF7E, 0xEF7E, 0xEF7E, 0xEF7D, 0xEF7D, 0xEF7E, 0xEF7E, 0xC638, 0x94B2, 0x94B2,   // 0x0130 (304) pixels
0x94B2, 0xDEFB, 0xF79E, 0xF79E, 0xF7BF, 0xF7BF, 0xF7BF, 0xF7DF, 0xFFFF, 0xFFDF, 0xEF7E, 0xE71C, 0xCE9A, 0x8430, 0x4A69, 0x0000,   // 0x0140 (320) pixels
0xC639, 0xC638, 0xD69A, 0xE71C, 0xEF7E, 0xF79E, 0xEF7E, 0xEF7E, 0xEF7E, 0xEF7E, 0xEF7D, 0xEF7D, 0xEF7D, 0xC618, 0x9492, 0x9492,   // 0x0150 (336) pixels
0x9492, 0xDEDB, 0xEF9E, 0xF79E, 0xF79E, 0xF7BF, 0xF7BF, 0xF7BF, 0xFFDF, 0xFFFF, 0xF79E, 0xE71D, 0xD6BB, 0xB596, 0x4208, 0x0000,   // 0x0160 (352) pixels
0xC639, 0xC618, 0xD6BB, 0xE73D, 0xF79E, 0xF79E, 0xEF7E, 0xEF7E, 0xEF7E, 0xEF7E, 0xEF7E, 0xEF7D, 0xEF5D, 0xC5F8, 0x8C71, 0x8C71,   // 0x0170 (368) pixels
0x8C71, 0xDEDB, 0xEF7E, 0xEF7E, 0xF79E, 0xF79E, 0xF7BF, 0xF7BF, 0xF7BF, 0xFFFF, 0xF7DF, 0xE73D, 0xD6DB, 0xCE59, 0x4229, 0x0000,   // 0x0180 (384) pixels
0xC638, 0xC618, 0xD6DB, 0xEF5D, 0xF79F, 0xF79F, 0xEF9E, 0xEF9E, 0xEF9E, 0xEF7E, 0xEF7E, 0xEF7D, 0xEF5D, 0xBDF7, 0x8C51, 0x8C51,   // 0x0190 (400) pixels
0x8C51, 0xD6BA, 0xEF7D, 0xEF7E, 0xEF7E, 0xF79E, 0xF79E, 0xF79E, 0xF79E, 0xFFDF, 0xF7BF, 0xE75D, 0xDEDB, 0xCE59, 0x52AA, 0x18E3,   // 0x01A0 (416) pixels
0xC638, 0xC639, 0xDEDB, 0xEF7E, 0xF7BF, 0xF7BF, 0xF79E, 0xE73D, 0xB5B6, 0xB596, 0xB596, 0xB596, 0xB596, 0x9CD3, 0x8430, 0x8430,   // 0x01B0 (432) pixels
0x8430, 0xA514, 0xB596, 0xB596, 0xB596, 0xB596, 0xBDF7, 0xF79E, 0xF79E, 0xF7BF, 0xF7BF, 0xEF5E, 0xDEDB, 0xCE5A, 0x6B6E, 0x18C3,   // 0x01C0 (448) pixels
0xC618, 0xC639, 0xDEDC, 0xF79F, 0xF7DF, 0xF7BF, 0xF79E, 0xC618, 0x8410, 0x8410, 0x8410, 0x8410, 0x8410, 0x8410, 0x8410, 0x8410,   // 0x01D0 (464) pixels
0x8410, 0x8410, 0x8410, 0x8410, 0x8410, 0x8410, 0x8410, 0xEF5D, 0xEF7E, 0xEF9E, 0xF79E, 0xEF5D, 0xDEDB, 0xCE5A, 0x8410, 0x1082,   // 0x01E0 (480) pixels
0xC618, 0xC639, 0xDEDC, 0xF7BF, 0xFFDF, 0xF7BF, 0xF79E, 0xBDF8, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF,   // 0x01F0 (496) pixels
0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0xEF5D, 0xEF7E, 0xEF7E, 0xEF7E, 0xE75D, 0xD6DB, 0xCE59, 0x8430, 0x0861,   // 0x0200 (512) pixels
0xC618, 0xC618, 0xD6DB, 0xF7BF, 0xFFDF, 0xF7BF, 0xF79E, 0xC618, 0x73AE, 0x73AE, 0x73AE, 0x73AE, 0x73AE, 0x73AE, 0x73AE, 0x73AE,   // 0x0210 (528) pixels
0x73AE, 0x73AE, 0x73AE, 0x73AE, 0x73AE, 0x73AE, 0x73AE, 0xEF7D, 0xEF5D, 0xE75D, 0xEF5D, 0xE71C, 0xD6BB, 0xC659, 0x7BCF, 0x0841,   // 0x0220 (544) pixels
0xC638, 0xBE18, 0xD6BB, 0xEF7E, 0xFFFF, 0xF7DF, 0xF79E, 0xF79E, 0xDEDB, 0xDEDB, 0xDEDB, 0xDEDB, 0xD6BB, 0x9CF4, 0x738E, 0x738E,   // 0x0230 (560) pixels
0x738E, 0xB575, 0xD69A, 0xD69A, 0xD6BA, 0xD6BA, 0xDEFC, 0xEF7E, 0xE73D, 0xE73D, 0xE71C, 0xDEFC, 0xD69A, 0xC639, 0x632C, 0x0020,   // 0x0240 (576) pixels
0xC639, 0xBDF8, 0xCE9A, 0xE73D, 0xFFFF, 0xFFDF, 0xEF7E, 0xF79E, 0xF79E, 0xF79E, 0xEF7E, 0xEF7E, 0xEF7E, 0xB596, 0x6B6D, 0x6B6D,   // 0x0250 (592) pixels
0x6B6D, 0xCE79, 0xEF5D, 0xEF5D, 0xEF5D, 0xEF5D, 0xE73D, 0xE73D, 0xE71C, 0xDF1C, 0xDEFC, 0xD6BB, 0xCE7A, 0xC639, 0x39E8, 0x0020,   // 0x0260 (608) pixels
0xC639, 0xBDF8, 0xCE59, 0xDEDC, 0xFFDF, 0xFFFF, 0xEF7E, 0xEF7E, 0xF79E, 0xF79E, 0xEF9E, 0xEF7E, 0xEF7E, 0xAD76, 0x6B4D, 0x6B4D,   // 0x0270 (624) pixels
0x6B4D, 0xCE79, 0xEF5D, 0xEF5D, 0xE73D, 0xE73D, 0xE71C, 0xE71C, 0xDEFC, 0xDEDB, 0xD6BB, 0xCE9A, 0xC639, 0xC618, 0x1082, 0x0000,   // 0x0280 (640) pixels
0xC639, 0xC618, 0xC618, 0xD69B, 0xEF7E, 0xFFFF, 0xF79E, 0xEF5E, 0xEF7E, 0xEF9E, 0xEF7E, 0xEF7E, 0xEF7E, 0xAD75, 0x632C, 0x632C,   // 0x0290 (656) pixels
0x632C, 0xCE59, 0xEF5D, 0xE73D, 0xE73D, 0xE71C, 0xDEFC, 0xDEFB, 0xDEDB, 0xD6BB, 0xCE9A, 0xCE59, 0xC618, 0x94B3, 0x0000, 0x0000,   // 0x02A0 (672) pixels
0x0000, 0xC639, 0xBDD7, 0xCE5A, 0xDEDC, 0xFFDF, 0xFFDF, 0xE73D, 0xEF5E, 0xEF7E, 0xEF7E, 0xEF7E, 0xEF7E, 0xAD55, 0x630C, 0x630C,   // 0x02B0 (688) pixels
0x630C, 0xCE59, 0xE73D, 0xE73D, 0xE71C, 0xDEFC, 0xDEDB, 0xD6BB, 0xD69A, 0xCE7A, 0xCE59, 0xC639, 0xC618, 0x4A49, 0x0000, 0x0000,   // 0x02C0 (704) pixels
0x0000, 0xC638, 0xBDF8, 0xBE18, 0xCE7A, 0xE73D, 0xFFFF, 0xEF5D, 0xE73D, 0xE75D, 0xEF5E, 0xEF5E, 0xEF5E, 0xAD76, 0x5AEB, 0x5AEB,   // 0x02D0 (720) pixels
0x5AEB, 0xCE79, 0xE71C, 0xE71C, 0xDEFC, 0xDEDB, 0xD6BB, 0xD69A, 0xCE7A, 0xCE59, 0xC639, 0xBDF8, 0xA535, 0x0020, 0x0000, 0x0000,   // 0x02E0 (736) pixels
0x0000, 0x0000, 0xC638, 0xBDD7, 0xC639, 0xCE9A, 0xEF5D, 0xFFDF, 0xE71C, 0xE71D, 0xE73D, 0xE73D, 0xE75D, 0xE73D, 0xC638, 0xC618,   // 0x02F0 (752) pixels
0xCE59, 0xEF7D, 0xE71C, 0xDEFC, 0xDEDB, 0xD6BB, 0xD69A, 0xCE79, 0xC659, 0xC639, 0xBDF8, 0xBE18, 0x39C7, 0x0000, 0x0000, 0x0000,   // 0x0300 (768) pixels
0x0000, 0x0000, 0xC618, 0xC618, 0xBDD7, 0xC639, 0xCE9A, 0xEF5D, 0xF79E, 0xDEFC, 0xDEFC, 0xE71C, 0xE71D, 0xE71D, 0xE71C, 0xE71C,   // 0x0310 (784) pixels
0xE71C, 0xDEFC, 0xDEDC, 0xDEDB, 0xD6BB, 0xD69A, 0xCE7A, 0xC659, 0xC618, 0xBDF8, 0xBDF8, 0x73AF, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0320 (800) pixels
0x0000, 0x0000, 0x0000, 0xBDF8, 0xC618, 0xBDD7, 0xC639, 0xCE7A, 0xDF1C, 0xEF5D, 0xDEDB, 0xDEDC, 0xDEFC, 0xDEFC, 0xDEFC, 0xDEFC,   // 0x0330 (816) pixels
0xDEDC, 0xDEDB, 0xD6BB, 0xD69A, 0xCE9A, 0xCE7A, 0xC659, 0xC618, 0xBDF8, 0xBDF8, 0x94B3, 0x0020, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0340 (832) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0xBDD7, 0xBDF8, 0xBDD7, 0xC618, 0xCE59, 0xD69A, 0xDEFC, 0xD6BB, 0xD6BB, 0xD6BB, 0xD6BB, 0xD6BB,   // 0x0350 (848) pixels
0xD6BB, 0xD69B, 0xD69A, 0xCE7A, 0xCE59, 0xC639, 0xC618, 0xBDF8, 0xBDF8, 0x94B3, 0x0861, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0360 (864) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xB5B6, 0xBDF7, 0xBDF7, 0xBDF8, 0xC639, 0xCE59, 0xCE7A, 0xCE7A, 0xCE7A, 0xCE9A, 0xCE7A,   // 0x0370 (880) pixels
0xCE7A, 0xCE7A, 0xCE5A, 0xC639, 0xC639, 0xBDF8, 0xBDF8, 0xBDF8, 0x7BCF, 0x0020, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0380 (896) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xAD55, 0xAD55, 0xBDF7, 0xBDD7, 0xBDF8, 0xBE18, 0xC639, 0xC639, 0xC639, 0xC639,   // 0x0390 (912) pixels
0xC639, 0xC639, 0xC618, 0xBDF8, 0xBDF8, 0xBDF8, 0xA535, 0x39E7, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x03A0 (928) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x7BEF, 0x8430, 0xAD76, 0xBDF8, 0xBDF8, 0xBDF8, 0xBDF8, 0xBDF8,   // 0x03B0 (944) pixels
0xBDF8, 0xBDF8, 0xBDF8, 0xBDF8, 0x94B2, 0x4229, 0x0020, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x03C0 (960) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x39C7, 0x31A6, 0x4208, 0x52AA, 0x6B6E, 0x8410, 0x8430,   // 0x03D0 (976) pixels
0x7BCF, 0x632C, 0x39E8, 0x1082, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x03E0 (992) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1082, 0x0861, 0x0841, 0x0020,   // 0x03F0 (1008) pixels
0x0020, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0400 (1024) pixels
};
