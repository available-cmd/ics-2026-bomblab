#include "defuser.h"

#include <inttypes.h>
#include <stdio.h>

#define MAX_FIELDS 8

/*
 * 把 values 中的 count 个整数写成一行“空格分隔的无符号十进制整数”，存入 out。
 * 成功返回 0；count 为 0（还没有实现）、snprintf 出错或结果被截断时返回非零。
 */
static int write_answer(char *out, size_t cap, const uint32_t *values, size_t count) {
    size_t used = 0;
    for (size_t i = 0; i < count; ++i) {
        int size = snprintf(out + used, cap - used, "%s%" PRIu32, i ? " " : "", values[i]);
        if (size < 0 || (size_t)size >= cap - used) return 1;
        used += (size_t)size;
    }
    return count == 0;
}
//----------------------------------------------------------
int p1_step2(uint32_t x, uint32_t y) {
    return (x + y * 8) ^ 0xc995;
}
int p1_step0(uint32_t y, uint32_t z){
    return (z ^ 0x7aa0) + y * 2;
}
int p1_step1(uint32_t b, uint32_t a){
    return (b - a) * 5 + 0x85f4;
}
int p1_compute(uint64_t challenge){
    uint32_t x  = (uint32_t)(challenge >> 32);
    uint32_t y  = (uint32_t)challenge;
    uint64_t z = challenge >> 16;
    uint32_t a = p1_step2(x, y);
    uint32_t b = p1_step0(y, z);
    uint32_t c = p1_step1(b, a);
    return c;
}
int solve_phase_1(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[1]={p1_compute(challenge)};
    size_t count = 1;
    return write_answer(out, cap, values, count);
}
//--------------------------------------------------------------------
int solve_phase_2(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[6];
    size_t count = 6;
    values[0] = (uint32_t)(((challenge >> 37) & 0xf) + 12);
    uint32_t prev = values[0];
    uint32_t j = 18;
    for (uint32_t i = 1; i < 6; i++) {
        uint32_t t = (uint32_t)(((challenge >> j) & 0x3f) + i);
        t ^= prev;
        j += 5;
        values[i] = t * 3;
        prev = values[i];
    }
    return write_answer(out, cap, values, count);
}
//------------------------------------------------

uint32_t p3_dispatch(uint32_t i, uint32_t x) {
    switch (i) {
        case 0: return 5u * x + 0x329bu;
        case 1: return 5u * x + 0x70cbu;
        case 2: return (x ^ 0x4c5bu) + (x >> 2);
        case 3: return (x ^ 0x69fu) + (x >> 2);
        case 4: return 3u * x + 0x96fbu;
        case 5: return (2u * x) ^ 0xce02u;
        case 6: return 3u * x + 0x1d07u;
        case 7: return (4u * x) ^ 0xf8deu;
        default: return 0;
    }
}
int solve_phase_3(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[2];
    size_t count = 2;
    values[0] = challenge & 7;
    values[1] = p3_dispatch(values[0] , ((challenge>>32) ^ ((challenge>>3) & 0xffffffffu)) & 0xffffffffu);
    return write_answer(out, cap, values, count);
}
//--------------------------------------------------------------
uint32_t p4_walk(uint32_t x, int lo, int hi, uint32_t key) {
    uint32_t mid =  (hi + lo) / 2;
    if (mid == x)
        return (mid + 0xc7) ^ key;
    uint32_t ebx = mid + key;
    uint32_t r;
    if (x > mid) {
        r = p4_walk(x, mid + 1, hi, key);
        return (5 * r) ^ ebx ^ 0x7;
    } else {
        r = p4_walk(x, lo, mid - 1, key);
        return (5 * r) ^ ebx ^ 0xf;
    }
}
int solve_phase_4(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[1] = {p4_walk(challenge & 0x7f , 0 , 0x7f , (challenge >> 7) ^ (challenge >> 32))};
    size_t count = 1;
    return write_answer(out, cap, values, count);
}
//---------------------------------------------------------------------
struct node {
    uint32_t f0;
    uint32_t f4;
    struct node *next;
};
static struct node p5_nodes[12] = {
{0x98e7, 0x0a5a, &p5_nodes[5]},
{0xd00e, 0x026e, &p5_nodes[0]},
{0x6cc0, 0x0118, &p5_nodes[9]},
{0x6488, 0x0915, &p5_nodes[11]},
{0x19c7, 0x073d, &p5_nodes[3]},
{0x2355, 0x0027, &p5_nodes[2]},
{0xe273, 0x066c, &p5_nodes[1]},
{0xd77c, 0x0434, &p5_nodes[6]},
{0x554e, 0x0b60, NULL},
{0xc5de, 0x0817, &p5_nodes[8]},
{0xe611, 0x0312, &p5_nodes[7]},
{0x3f29, 0x0549, &p5_nodes[10]},
};

static struct node *p5_heads[8] = {
    &p5_nodes[4],
    &p5_nodes[3],
    &p5_nodes[11],
    &p5_nodes[10],
    &p5_nodes[7],
    &p5_nodes[6],
    &p5_nodes[1],
    &p5_nodes[0],
};
uint32_t p5_traverse(uint64_t x, uint32_t *out)
{
    struct node *n = p5_heads[x & 7];
    uint32_t eax = (uint32_t)(x >> 6) ^ (uint32_t)(x >> 32)^ n->f0 ^ 0xf243u;
    eax += n->f4;
    *out = n->f4;
    unsigned count = (unsigned)(x >> 3) & 3;
    unsigned i = 1;
    while (i <= count) {
        n = n->next;
        eax ^= n->f0;
        eax += n->f4;
        *out = n->f4;
        i++;
    }
    return eax;
}
int solve_phase_5(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[2];
    size_t count = 2;
    uint32_t last_f4;
    uint32_t ret = p5_traverse(challenge, &last_f4);
    values[0] = ret;
    values[1] = last_f4;   
    return write_answer(out, cap, values, count);
}
//------------------------------------------------------------------------------
static uint32_t f_490(uint32_t a){return (a ^ 0xc379u) + 4u*a;}
static uint32_t f_4a0(uint32_t a){return ((a<<7)|(a>>25)) ^ 0x8b4bu;}
static uint32_t f_4b0(uint32_t a){return (a + 0x5cc9u) ^ (a >> 5);}
static uint32_t f_4c0(uint32_t a){return ((a >> 7) ^ a) + 0x4a15u;}
static uint32_t f_4e0(uint32_t a){return (4u*a + 0xc948u) ^ (3u*a);}
static uint32_t f_500(uint32_t a){return (a >> 1) | (a << 31);}
static uint32_t (*const p6_ops[8])(uint32_t) = {
    f_490, f_4e0, f_4e0, f_4c0, f_500, f_4a0, f_4c0, f_4b0
};
uint32_t p6_dispatch(uint64_t x)
{
    uint32_t edi = (uint32_t)(x >> 6) ^ (uint32_t)(x >> 32);
    uint32_t eax = p6_ops[x & 7](edi);
    eax ^= 0x625;
    eax = p6_ops[(x & 0x38) >> 3](eax);
    eax += 0x3f57;
    return eax;
}
int solve_phase_6(uint64_t challenge, char *out, size_t cap) {
    size_t count =1;
    uint32_t values[1] = {p6_dispatch(challenge)};
    return write_answer(out, cap, values, count);
}
//-----------------------------------------------------
#include <stdint.h>
#include <string.h>
static uint32_t secret_mix(uint32_t x) {
    uint32_t a = x;
    uint32_t b = x ^ 0xa2dd;
    a >>= 3;
    a ^= 0x48c2;
    return a + b;
}
int solve_secret(uint64_t challenge, uint64_t context, char *out, size_t cap) {
    uint32_t ch_low   = (uint32_t)challenge;
    uint32_t ch_high  = (uint32_t)(challenge >> 32);
    uint32_t ctx_low  = (uint32_t)context;
    uint32_t ctx_high = (uint32_t)(context >> 32);
    uint32_t ecx = ctx_low & 3;
    uint32_t v0  = ecx;
    uint32_t r9d = (ecx * 0xa2dd) ^ ((uint32_t)(context >> 16));
    uint32_t esi = ch_high ^ ch_low;
    uint32_t r8d = ctx_high ^ ctx_low;
    uint32_t edi = esi ^ r8d;
    uint32_t m   = secret_mix(edi);
    uint32_t v1  = m ^ r9d;
    uint32_t esi_shifted = esi >> 3;
    uint32_t edx         = r8d + ecx + 0x48c2;
    edx                 += esi_shifted;
    uint32_t v2          = v1 ^ edx;  
    char buf[200];
    memset(buf, 'A', 32);
    memcpy(buf + 32, "OPEN", 4);
    snprintf(buf + 36, sizeof(buf) - 36, "%u %u %u", v0, v1, v2);
    if (out == NULL || cap < strlen(buf) + 1) return -1;
    snprintf(out, cap, "%s", buf);
    return 0;
}