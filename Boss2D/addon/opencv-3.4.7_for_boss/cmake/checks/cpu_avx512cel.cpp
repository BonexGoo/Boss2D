#if defined __AVX512__ || defined __AVX512F__
#include <immintrin.h>
void test()
{
    __m512i a, b, c;
    a = _mm512_dpwssd_epi32(a, b, c);
}
#else
#error "AVX512-CEL is not supported"
#endif
int main() { return 0; }