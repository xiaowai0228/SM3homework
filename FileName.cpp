#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

// 常量定义
#define SHA384_BLOCK_SIZE 64
#define SHA384_DIGEST_SIZE 48

// 工具函数：字符串反转
void reverse_str(char* str, int len) {
    for (int i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
}

// 十进制转二进制字符串（指定位数）
void dec_to_bin(uint32_t num, char* bin, int bits) {
    int i = 0;
    if (num == 0) {
        bin[i++] = '0';
    }
    else {
        while (num > 0) {
            bin[i++] = (num % 2) ? '1' : '0';
            num /= 2;
        }
    }
    while (i < bits) {
        bin[i++] = '0';
    }
    bin[i] = '\0';
    reverse_str(bin, i);
}

// 十六进制转二进制字符串
void hex_to_bin(const char* hex, char* bin) {
    int len = strlen(hex);
    *bin = '\0';
    for (int i = 0; i < len; i++) {
        char c = hex[i];
        char b[5];
        switch (c) {
        case '0': strcpy(b, "0000"); break;
        case '1': strcpy(b, "0001"); break;
        case '2': strcpy(b, "0010"); break;
        case '3': strcpy(b, "0011"); break;
        case '4': strcpy(b, "0100"); break;
        case '5': strcpy(b, "0101"); break;
        case '6': strcpy(b, "0110"); break;
        case '7': strcpy(b, "0111"); break;
        case '8': strcpy(b, "1000"); break;
        case '9': strcpy(b, "1001"); break;
        case 'a': case 'A': strcpy(b, "1010"); break;
        case 'b': case 'B': strcpy(b, "1011"); break;
        case 'c': case 'C': strcpy(b, "1100"); break;
        case 'd': case 'D': strcpy(b, "1101"); break;
        case 'e': case 'E': strcpy(b, "1110"); break;
        case 'f': case 'F': strcpy(b, "1111"); break;
        default: strcpy(b, "0000");
        }
        strcat(bin, b);
    }
}

// 二进制转十六进制字符串
void bin_to_hex(const char* bin, char* hex) {
    int len = strlen(bin);
    *hex = '\0';
    for (int i = 0; i < len; i += 4) {
        char b[5] = { 0 };
        strncpy(b, bin + i, 4);
        int val = strtol(b, NULL, 2);
        char h[2];
        sprintf(h, "%x", val);
        strcat(hex, h);
    }
}

// 循环左移
void rotate_left(char* result, const char* text, int num, int len) {
    for (int i = 0; i < len - num; i++) {
        result[i] = text[i + num];
    }
    for (int i = 0; i < num; i++) {
        result[len - num + i] = text[i];
    }
    result[len] = '\0';
}

// 异或操作
void xor_bits(char* result, const char* a, const char* b) {
    int len = strlen(a);
    for (int i = 0; i < len; i++) {
        result[i] = (a[i] == b[i]) ? '0' : '1';
    }
    result[len] = '\0';
}

// 三值异或
void xor3_bits(char* result, const char* a, const char* b, const char* c) {
    char temp[1024];
    xor_bits(temp, a, b);
    xor_bits(result, temp, c);
}

// 或操作
void or_bits(char* result, const char* a, const char* b) {
    int len = strlen(a);
    for (int i = 0; i < len; i++) {
        result[i] = (a[i] == '1' || b[i] == '1') ? '1' : '0';
    }
    result[len] = '\0';
}

// 三值或
void or3_bits(char* result, const char* a, const char* b, const char* c) {
    char temp[1024];
    or_bits(temp, a, b);
    or_bits(result, temp, c);
}

// 与操作
void and_bits(char* result, const char* a, const char* b) {
    int len = strlen(a);
    for (int i = 0; i < len; i++) {
        result[i] = (a[i] == '1' && b[i] == '1') ? '1' : '0';
    }
    result[len] = '\0';
}

// 三值与
void and3_bits(char* result, const char* a, const char* b, const char* c) {
    char temp[1024];
    and_bits(temp, a, b);
    and_bits(result, temp, c);
}

// 非操作
void not_bits(char* result, const char* a) {
    int len = strlen(a);
    for (int i = 0; i < len; i++) {
        result[i] = (a[i] == '1') ? '0' : '1';
    }
    result[len] = '\0';
}

// 置换函数
void substitute(char* result, const char* x, int mode) {
    char temp1[33], temp2[33];
    if (mode == 0) {
        rotate_left(temp1, x, 9, 32);
        rotate_left(temp2, x, 17, 32);
    }
    else {
        rotate_left(temp1, x, 15, 32);
        rotate_left(temp2, x, 23, 32);
    }
    xor3_bits(result, x, temp1, temp2);
}

// 消息填充
char* fill(const char* text) {
    int text_len = strlen(text);
    char* text_bin = (char*)malloc(1024 * 1024); // 分配足够大的空间
    *text_bin = '\0';

    // 转换为二进制（每个字符加前导0）
    for (int i = 0; i < text_len; i++) {
        char bin[9];
        dec_to_bin((uint8_t)text[i], bin, 7);
        strcat(text_bin, "0"); // 加前导0
        strcat(text_bin, bin);
    }

    // 添加'1'
    strcat(text_bin, "1");

    // 添加'0'直到长度 ≡ 448 mod 512
    int len = strlen(text_bin);
    while (len % 512 != 448) {
        strcat(text_bin, "0");
        len++;
    }

    // 添加原始长度（64位）
    char length_bin[65];
    dec_to_bin(len - 1, length_bin, 64); // 减1是因为之前加了一个'1'
    strcat(text_bin, length_bin);

    return text_bin;
}

// 消息拓展
void expand(const char* b, char w[132][9]) {
    char bin[33];
    char temp[33];
    char temp2[33];

    // w[0]~w[15]
    for (int i = 0; i < 16; i++) {
        strncpy(bin, b + i * 32, 32);
        bin[32] = '\0';
        bin_to_hex(bin, w[i]);
    }

    // w[16]~w[67]
    for (int j = 16; j < 68; j++) {
        char w16[33], w9[33], w3[33], temp1[33], temp2[33], temp3[33];

        hex_to_bin(w[j - 16], w16);
        hex_to_bin(w[j - 9], w9);
        hex_to_bin(w[j - 3], w3);

        rotate_left(temp1, w3, 15, 32);
        xor3_bits(temp2, w16, w9, temp1);
        substitute(temp3, temp2, 1);

        char w13[33], w6[33], temp4[33], temp5[33];
        hex_to_bin(w[j - 13], w13);
        hex_to_bin(w[j - 6], w6);

        rotate_left(temp4, w13, 7, 32);
        xor3_bits(temp5, temp3, temp4, w6);

        bin_to_hex(temp5, w[j]);
    }

    // w[68]~w[131]
    for (int j = 0; j < 64; j++) {
        char wj[33], wj4[33], temp[33];
        hex_to_bin(w[j], wj);
        hex_to_bin(w[j + 4], wj4);
        xor_bits(temp, wj, wj4);
        bin_to_hex(temp, w[j + 68]);
    }
}

// FF布尔函数
void FF(char* result, const char* x, const char* y, const char* z, int j) {
    if (j >= 0 && j <= 15) {
        xor3_bits(result, x, y, z);
    }
    else {
        char temp1[33], temp2[33], temp3[33];
        and_bits(temp1, x, y);
        and_bits(temp2, x, z);
        and_bits(temp3, y, z);
        or3_bits(result, temp1, temp2, temp3);
    }
}

// GG布尔函数
void GG(char* result, const char* x, const char* y, const char* z, int j) {
    if (j >= 0 && j <= 15) {
        xor3_bits(result, x, y, z);
    }
    else {
        char temp1[33], not_x[33], temp2[33];
        and_bits(temp1, x, y);
        not_bits(not_x, x);
        and_bits(temp2, not_x, z);
        or_bits(result, temp1, temp2);
    }
}

// 压缩函数
void compress(const char w[132][9], const char* iv, char* output) {
    char A[9], B[9], C[9], D[9], E[9], F[9], G[9], H[9];
    strncpy(A, iv, 8); A[8] = '\0';
    strncpy(B, iv + 8, 8); B[8] = '\0';
    strncpy(C, iv + 16, 8); C[8] = '\0';
    strncpy(D, iv + 24, 8); D[8] = '\0';
    strncpy(E, iv + 32, 8); E[8] = '\0';
    strncpy(F, iv + 40, 8); F[8] = '\0';
    strncpy(G, iv + 48, 8); G[8] = '\0';
    strncpy(H, iv + 56, 8); H[8] = '\0';

    for (int j = 0; j < 64; j++) {
        const char* T = (j <= 15) ? "79cc4519" : "7a879d8a";

        // 计算SS1
        char A_bin[33], E_bin[33], T_bin[33], T_rot[33];
        hex_to_bin(A, A_bin);
        hex_to_bin(E, E_bin);
        hex_to_bin(T, T_bin);

        char A_rot12[33];
        rotate_left(A_rot12, A_bin, 12, 32);
        rotate_left(T_rot, T_bin, j % 32, 32);

        uint32_t a_val = strtol(A_rot12, NULL, 2);
        uint32_t e_val = strtol(E_bin, NULL, 2);
        uint32_t t_val = strtol(T_rot, NULL, 2);
        uint32_t ss1_val = (a_val + e_val + t_val) % 0x100000000;

        char ss1_bin[33];
        dec_to_bin(ss1_val, ss1_bin, 32);
        char SS1[33];
        rotate_left(SS1, ss1_bin, 7, 32);

        // 计算SS2
        char SS2[33];
        xor_bits(SS2, SS1, A_rot12);

        // 准备B、C、F、G的二进制值
        char B_bin[33], C_bin[33], F_bin[33], G_bin[33];
        hex_to_bin(B, B_bin);
        hex_to_bin(C, C_bin);
        hex_to_bin(F, F_bin);
        hex_to_bin(G, G_bin);

        // 计算TT1
        char F_result[33], D_bin[33], wj_bin[33], w68_bin[33];
        hex_to_bin(D, D_bin);
        hex_to_bin(w[j], wj_bin);
        hex_to_bin(w[j + 68], w68_bin);

        FF(F_result, A_bin, B_bin, C_bin, j);
        uint32_t f_val = strtol(F_result, NULL, 2);
        uint32_t d_val = strtol(D_bin, NULL, 2);
        uint32_t ss2_val = strtol(SS2, NULL, 2);
        uint32_t wj_val = strtol(wj_bin, NULL, 2);
        uint32_t w68_val = strtol(w68_bin, NULL, 2);
        uint32_t tt1_val = (f_val + d_val + ss2_val + wj_val + w68_val) % 0x100000000;

        // 计算TT2
        char G_result[33], H_bin[33];
        hex_to_bin(H, H_bin);
        GG(G_result, E_bin, F_bin, G_bin, j);
        uint32_t g_val = strtol(G_result, NULL, 2);
        uint32_t h_val = strtol(H_bin, NULL, 2);
        uint32_t ss1_val2 = strtol(SS1, NULL, 2);
        uint32_t tt2_val = (g_val + h_val + ss1_val2) % 0x100000000;

        // 更新寄存器
        char new_A[33], new_E[33];
        dec_to_bin(tt1_val, new_A, 32);
        bin_to_hex(new_A, A);

        dec_to_bin((d_val + tt1_val) % 0x100000000, new_E, 32);
        bin_to_hex(new_E, E);

        // 其他寄存器移位更新
        strcpy(H, G);

        char F_rot19[33];
        rotate_left(F_rot19, F_bin, 19, 32);
        bin_to_hex(F_rot19, G);

        strcpy(F, E);
        strcpy(D, C);

        char B_rot9[33];
        rotate_left(B_rot9, B_bin, 9, 32);
        bin_to_hex(B_rot9, C);

        strcpy(B, A);
    }

    // 与初始IV异或，生成输出
    char result[65], iv_bin[513], result_bin[513], xor_result[513];
    sprintf(result, "%s%s%s%s%s%s%s%s", A, B, C, D, E, F, G, H);

    hex_to_bin(iv, iv_bin);
    hex_to_bin(result, result_bin);
    xor_bits(xor_result, result_bin, iv_bin);

    bin_to_hex(xor_result, output);
}

// 迭代函数
void iteration(const char* m, char* output) {
    const char* IV = "7380166f4914b2b9172442d7da8a0600a96f30bc163138aae38dee4db0fb0e4e";
    int len = strlen(m);
    int n = len / 512;

    char current_iv[65];
    strcpy(current_iv, IV);

    for (int i = 0; i < n; i++) {
        const char* block = m + i * 512;
        char w[132][9];
        expand(block, w);

        char new_iv[65];
        compress(w, current_iv, new_iv);
        strcpy(current_iv, new_iv);
    }

    strcpy(output, current_iv);
}

int main() {
    const char* text = "abc";
    char* m = fill(text);
    char result[65];
    iteration(m, result);
    printf("%s\n", result);

    free(m);
    return 0;
}