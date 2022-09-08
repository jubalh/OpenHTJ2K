// Copyright (c) 2019 - 2022, Osamu Watanabe
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
//    modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
//    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
//    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#if defined(OPENHTJ2K_TRY_AVX2) && defined(__AVX2__)
  #include "coding_units.hpp"
  #include "dec_CxtVLC_tables.hpp"
  #include "ht_block_decoding.hpp"
  #include "coding_local.hpp"
  #include "utils.hpp"

  #if defined(_MSC_VER) || defined(__MINGW64__)
    #include <intrin.h>
  #else
    #include <x86intrin.h>
  #endif

// // https://stackoverflow.com/a/58827596
// FORCE_INLINE __m256i avx2_lzcnt_epi32(__m256i v) {
//   // prevent value from being rounded up to the next power of two
//   v = _mm256_andnot_si256(_mm256_srli_epi32(v, 8), v);  // keep 8 MSB

//   v = _mm256_castps_si256(_mm256_cvtepi32_ps(v));    // convert an integer to float
//   v = _mm256_srli_epi32(v, 23);                      // shift down the exponent
//   v = _mm256_subs_epu16(_mm256_set1_epi32(158), v);  // undo bias
//   v = _mm256_min_epi16(v, _mm256_set1_epi32(32));    // clamp at 32

//   return v;
// }

// // Credit: YumiYumiYumi
// // https://old.reddit.com/r/simd/comments/b3k1oa/looking_for_sseavx_bitscan_discussions/
// FORCE_INLINE __m256i avx2_lzcnt2_epi32(__m256i v) {
//   const __m256i lut_lo = _mm256_set_epi8(4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 7, 32, 4, 4, 4, 4, 4,
//   4,
//                                          4, 4, 5, 5, 5, 5, 6, 6, 7, 32);
//   const __m256i lut_hi = _mm256_set_epi8(0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 3, 32, 0, 0, 0, 0, 0,
//   0,
//                                          0, 0, 1, 1, 1, 1, 2, 2, 3, 32);
//   const __m256i nibble_mask = _mm256_set1_epi8(0x0F);
//   const __m256i byte_offset = _mm256_set1_epi32(0x00081018);
//   __m256i t;

//   /* find lzcnt for each byte */
//   t = _mm256_and_si256(nibble_mask, v);
//   v = _mm256_and_si256(_mm256_srli_epi16(v, 4), nibble_mask);
//   t = _mm256_shuffle_epi8(lut_lo, t);
//   v = _mm256_shuffle_epi8(lut_hi, v);
//   v = _mm256_min_epu8(v, t);

//   /* find lzcnt for each dword */
//   v = _mm256_or_si256(v, byte_offset);
//   v = _mm256_min_epu8(v, _mm256_srli_epi16(v, 8));
//   v = _mm256_min_epu8(v, _mm256_srli_epi32(v, 16));

//   return v;
// }

// // Credit: YumiYumiYumi
// // https://old.reddit.com/r/simd/comments/b3k1oa/looking_for_sseavx_bitscan_discussions/
// FORCE_INLINE __m256i avx2_tzcnt_epi32(__m256i v) {
//   const __m256i lut_lo = _mm256_set_epi8(0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 32, 0, 1, 0, 2, 0,
//   1,
//                                          0, 3, 0, 1, 0, 2, 0, 1, 0, 32);
//   const __m256i lut_hi = _mm256_set_epi8(4, 5, 4, 6, 4, 5, 4, 7, 4, 5, 4, 6, 4, 5, 4, 32, 4, 5, 4, 6, 4,
//   5,
//                                          4, 7, 4, 5, 4, 6, 4, 5, 4, 32);
//   const __m256i nibble_mask = _mm256_set1_epi8(0x0F);
//   const __m256i byte_offset = _mm256_set1_epi32(0x18100800);
//   __m256i t;

//   /* find tzcnt for each byte */
//   t = _mm256_and_si256(nibble_mask, v);
//   v = _mm256_and_si256(_mm256_srli_epi16(v, 4), nibble_mask);
//   t = _mm256_shuffle_epi8(lut_lo, t);
//   v = _mm256_shuffle_epi8(lut_hi, v);
//   v = _mm256_min_epu8(v, t);

//   /* find tzcnt for each dword */
//   v = _mm256_or_si256(v, byte_offset);
//   v = _mm256_min_epu8(v, _mm256_srli_epi16(v, 8));
//   v = _mm256_min_epu8(v, _mm256_srli_epi32(v, 16));

//   return v;
// }

// // from my earlier answer, with tuning for non-AVX CPUs removed
// // static inline
// FORCE_INLINE uint32_t hsum_epi32_avx(__m128i x) {
//   __m128i hi64 = _mm_unpackhi_epi64(
//       x, x);  // 3-operand non-destructive AVX lets us save a byte without needing a movdqa
//   __m128i sum64 = _mm_add_epi32(hi64, x);
//   __m128i hi32  = _mm_shuffle_epi32(sum64, _MM_SHUFFLE(2, 3, 0, 1));  // Swap the low two elements
//   __m128i sum32 = _mm_add_epi32(sum64, hi32);
//   return static_cast<uint32_t>(_mm_cvtsi128_si32(sum32));  // movd
// }

// // only needs AVX2
// uint32_t hsum_8x32(__m256i v) {
//   __m128i sum128 =
//       _mm_add_epi32(_mm256_castsi256_si128(v),
//                     _mm256_extracti128_si256(
//                         v, 1));  // silly GCC uses a longer AXV512VL instruction if AVX512 is enabled :/
//   return hsum_epi32_avx(sum128);
// }

uint8_t j2k_codeblock::calc_mbr(const int16_t i, const int16_t j, const uint8_t causal_cond) const {
  const int16_t im1 = static_cast<int16_t>(i - 1);
  const int16_t jm1 = static_cast<int16_t>(j - 1);
  const int16_t ip1 = static_cast<int16_t>(i + 1);
  const int16_t jp1 = static_cast<int16_t>(j + 1);
  uint8_t mbr       = get_state(Sigma, im1, jm1);
  mbr               = mbr | get_state(Sigma, im1, j);
  mbr               = mbr | get_state(Sigma, im1, jp1);
  mbr               = mbr | get_state(Sigma, i, jm1);
  mbr               = mbr | get_state(Sigma, i, jp1);
  mbr               = mbr | static_cast<uint8_t>(get_state(Sigma, ip1, jm1) * causal_cond);
  mbr               = mbr | static_cast<uint8_t>(get_state(Sigma, ip1, j) * causal_cond);
  mbr               = mbr | static_cast<uint8_t>(get_state(Sigma, ip1, jp1) * causal_cond);

  mbr = mbr | static_cast<uint8_t>(get_state(Refinement_value, im1, jm1) * get_state(Scan, im1, jm1));
  mbr = mbr | static_cast<uint8_t>(get_state(Refinement_value, im1, j) * get_state(Scan, im1, j));
  mbr = mbr | static_cast<uint8_t>(get_state(Refinement_value, im1, jp1) * get_state(Scan, im1, jp1));
  mbr = mbr | static_cast<uint8_t>(get_state(Refinement_value, i, jm1) * get_state(Scan, i, jm1));
  mbr = mbr | static_cast<uint8_t>(get_state(Refinement_value, i, jp1) * get_state(Scan, i, jp1));
  mbr = mbr
        | static_cast<uint8_t>(get_state(Refinement_value, ip1, jm1) * get_state(Scan, ip1, jm1)
                               * causal_cond);
  mbr = mbr
        | static_cast<uint8_t>(get_state(Refinement_value, ip1, j) * get_state(Scan, ip1, j) * causal_cond);
  mbr = mbr
        | static_cast<uint8_t>(get_state(Refinement_value, ip1, jp1) * get_state(Scan, ip1, jp1)
                               * causal_cond);
  return mbr;
}

// https://stackoverflow.com/a/58827596
inline __m128i sse_lzcnt_epi32(__m128i v) {
  // prevent value from being rounded up to the next power of two
  v = _mm_andnot_si128(_mm_srli_epi32(v, 8), v);  // keep 8 MSB

  v = _mm_castps_si128(_mm_cvtepi32_ps(v));    // convert an integer to float
  v = _mm_srli_epi32(v, 23);                   // shift down the exponent
  v = _mm_subs_epu16(_mm_set1_epi32(158), v);  // undo bias
  v = _mm_min_epi16(v, _mm_set1_epi32(32));    // clamp at 32

  // __m128i t;
  // const __m128i lut_lo        = _mm_set_epi8(4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 7, 31);
  // const __m128i lut_hi        = _mm_set_epi8(0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 3, 31);
  // const __m128i nibble_mask   = _mm_set1_epi8(0x0F);
  // const __m128i byte_offset8  = _mm_set1_epi16(8);
  // const __m128i byte_offset16 = _mm_set1_epi16(16);
  // t                           = _mm_and_si128(nibble_mask, v);
  // v                           = _mm_and_si128(_mm_srli_epi16(v, 4), nibble_mask);
  // t                           = _mm_shuffle_epi8(lut_lo, t);
  // v                           = _mm_shuffle_epi8(lut_hi, v);
  // v                           = _mm_min_epu8(v, t);

  // t = _mm_srli_epi16(v, 8);
  // v = _mm_or_si128(v, byte_offset8);
  // v = _mm_min_epu8(v, t);

  // t = _mm_srli_epi32(v, 16);
  // v = _mm_or_si128(v, byte_offset16);
  // v = _mm_min_epu8(v, t);
  return v;
}

template <int N>
FORCE_INLINE __m128i decode_onq_quad(__m128i qinf, __m128i U_q, uint8_t pLSB, fwd_buf<0xFF> &MagSgn,
                                     __m128i &v_n) {
  const __m128i vone = _mm_set1_epi32(1);
  __m128i w0         = _mm_shuffle_epi32(qinf, _MM_SHUFFLE(N, N, N, N));
  __m128i flags      = _mm_and_si128(w0, _mm_set_epi32(0x8880, 0x4440, 0x2220, 0x1110));
  __m128i insig      = _mm_cmpeq_epi32(flags, _mm_setzero_si128());
  flags              = _mm_mullo_epi16(flags, _mm_set_epi16(1, 1, 2, 2, 4, 4, 8, 8));
  w0                 = _mm_srli_epi32(flags, 15);  // e_k
  U_q                = _mm_shuffle_epi32(U_q, _MM_SHUFFLE(N, N, N, N));
  __m128i m_n        = _mm_sub_epi32(U_q, w0);
  m_n                = _mm_andnot_si128(insig, m_n);
  w0                 = _mm_and_si128(_mm_srli_epi32(flags, 11), vone);  // e_1
  __m128i vmask      = _mm_sub_epi32(_mm_sllv_epi32(vone, m_n), vone);
  __m128i ms_vec     = MagSgn.fetch(m_n);
  ms_vec             = _mm_and_si128(ms_vec, vmask);
  ms_vec             = _mm_or_si128(ms_vec, _mm_sllv_epi32(w0, m_n));  // v = 2(mu-1) + sign (0 or 1)
  __m128i v_mu0      = _mm_add_epi32(ms_vec, _mm_set1_epi32(2));       // 2(mu-1) + sign + 2 = 2mu + sign
  // Add center bin (would be used for lossy and truncated lossless codestreams)
  v_mu0 = _mm_or_si128(v_mu0, vone);  // This cancels the effect of a sign bit in LSB
  v_mu0 = _mm_slli_epi32(v_mu0, pLSB - 1);
  v_mu0 = _mm_or_si128(v_mu0, _mm_slli_epi32(ms_vec, 31));
  v_mu0 = _mm_andnot_si128(insig, v_mu0);

  w0 = ms_vec;
  if (N == 0) {
    w0 = _mm_shuffle_epi8(w0, _mm_set_epi32(-1, -1, 0x0F0E0D0C, 0x07060504));
  } else if (N == 1) {
    w0 = _mm_shuffle_epi8(w0, _mm_set_epi32(0x0F0E0D0C, 0x07060504, -1, -1));
  }
  v_n = _mm_or_si128(v_n, w0);
  return v_mu0;
}

void ht_cleanup_decode(j2k_codeblock *block, const uint8_t &pLSB, const int32_t Lcup, const int32_t Pcup,
                       const int32_t Scup) {
  fwd_buf<0xFF> MagSgn(block->get_compressed_data(), Pcup);
  MEL_dec MEL(block->get_compressed_data(), Lcup, Scup);
  rev_buf VLC_dec(block->get_compressed_data(), Lcup, Scup);
  const uint16_t QW = static_cast<uint16_t>(ceil_int(static_cast<int16_t>(block->size.x), 2));
  const uint16_t QH = static_cast<uint16_t>(ceil_int(static_cast<int16_t>(block->size.y), 2));

  __m128i vExp;
  auto mp0 = block->sample_buf.get();
  auto mp1 = block->sample_buf.get() + block->blksampl_stride;
  auto sp0 = block->block_states.get() + 1 + block->blkstate_stride;
  auto sp1 = block->block_states.get() + 1 + 2 * block->blkstate_stride;

  uint32_t rho0, rho1;
  uint32_t u_off0, u_off1;
  // uint32_t emb_k_0, emb_k_1;
  // uint32_t emb_1_0, emb_1_1;
  uint32_t u0, u1;
  uint32_t U0, U1;
  uint8_t gamma0, gamma1;
  uint32_t kappa0 = 1, kappa1 = 1;  // kappa is always 1 for initial line-pair

  const uint16_t *dec_table0, *dec_table1;
  dec_table0 = dec_CxtVLC_table0_fast_16;
  dec_table1 = dec_CxtVLC_table1_fast_16;

  alignas(32) auto rholine = MAKE_UNIQUE<uint32_t[]>(QW + 3U);
  rholine[0]               = 0;
  auto rho_p               = rholine.get() + 1;
  alignas(32) auto Eline   = MAKE_UNIQUE<int32_t[]>(2U * QW + 6U);
  Eline[0]                 = 0;
  auto E_p                 = Eline.get() + 1;
  uint16_t scratch[1024];
  uint16_t *sp;

  uint32_t context = 0;
  uint32_t vlcval;
  int32_t mel_run = MEL.get_run();

  int32_t qx;
  // Initial line-pair
  sp = scratch;
  for (qx = QW; qx >= 2; qx -= 2, sp += 4) {
    // Decoding of significance and EMB patterns and unsigned residual offsets
    vlcval       = VLC_dec.fetch();
    uint16_t tv0 = dec_table0[(vlcval & 0x7F) + (static_cast<unsigned int>(context << 7))];
    if (context == 0) {
      mel_run -= 2;
      tv0 = (mel_run == -1) ? tv0 : 0;
      if (mel_run < 0) {
        mel_run = MEL.get_run();
      }
    }
    sp[0] = tv0;
    rho0  = _pext_u32(tv0, 0x00F0U);
    // emb_k_0 = _pext_u32(tv0, 0x0F00U);
    // emb_1_0 = _pext_u32(tv0, 0xF000U);

    // calculate context for the next quad
    context = (rho0 >> 1) | (rho0 & 1);

    // Decoding of significance and EMB patterns and unsigned residual offsets
    vlcval       = VLC_dec.advance((tv0 & 0x000F) >> 1);
    uint16_t tv1 = dec_table0[(vlcval & 0x7F) + (context << 7)];
    if (context == 0) {
      mel_run -= 2;
      tv1 = (mel_run == -1) ? tv1 : 0;
      if (mel_run < 0) {
        mel_run = MEL.get_run();
      }
    }
    sp[2] = tv1;
    rho1  = _pext_u32(tv1, 0x00F0U);
    // emb_k_1 = _pext_u32(tv1, 0x0F00U);
    // emb_1_1 = _pext_u32(tv1, 0xF000U);

    // store sigma
    *sp0++   = (rho0 >> 0) & 1;
    *sp0++   = (rho0 >> 2) & 1;
    *sp0++   = (rho1 >> 0) & 1;
    *sp0++   = (rho1 >> 2) & 1;
    *sp1++   = (rho0 >> 1) & 1;
    *sp1++   = (rho0 >> 3) & 1;
    *sp1++   = (rho1 >> 1) & 1;
    *sp1++   = (rho1 >> 3) & 1;
    *rho_p++ = rho0;
    *rho_p++ = rho1;

    // calculate context for the next quad
    context = (rho1 >> 1) | (rho1 & 1);

    vlcval = VLC_dec.advance((tv1 & 0x000F) >> 1);
    u_off0 = tv0 & 1;
    u_off1 = tv1 & 1;

    uint32_t mel_offset = 0;
    if (u_off0 == 1 && u_off1 == 1) {
      mel_run -= 2;
      mel_offset = (mel_run == -1) ? 0x40 : 0;
      if (mel_run < 0) {
        mel_run = MEL.get_run();
      }
    }

    // UVLC decoding
    uint32_t idx         = (vlcval & 0x3F) + (u_off0 << 6U) + (u_off1 << 7U) + mel_offset;
    uint32_t uvlc_result = uvlc_dec_0[idx];
    // remove total prefix length
    vlcval = VLC_dec.advance(uvlc_result & 0x7);
    uvlc_result >>= 3;
    // extract suffixes for quad 0 and 1
    uint32_t len = uvlc_result & 0xF;                    // suffix length for 2 quads (up to 10 = 5 + 5)
    uint32_t tmp = vlcval & _bzhi_u32(UINT32_MAX, len);  //  = ((1U << len) - 1U) suffix value for 2 quads
    vlcval       = VLC_dec.advance(len);
    uvlc_result >>= 4;
    // quad 0 length
    len = uvlc_result & 0x7;  // quad 0 suffix length
    uvlc_result >>= 3;
    // U = 1+ u
    u0 = 1 + (uvlc_result & 7) + (tmp & ~(0xFFU << len));  // always kappa = 1 in initial line pair
    u1 = 1 + (uvlc_result >> 3) + (tmp >> len);            // always kappa = 1 in initial line pair

    sp[1] = static_cast<uint16_t>(u0);
    sp[3] = static_cast<uint16_t>(u1);
  }
  sp = scratch;
  for (qx = QW; qx >= 2; qx -= 2, sp += 4) {
    __m128i v_n   = _mm_setzero_si128();
    __m128i qinf  = _mm_loadu_si128((__m128i *)sp);
    __m128i U_q   = _mm_srli_epi32(qinf, 16);
    __m128i v_mu0 = decode_onq_quad<0>(qinf, U_q, pLSB, MagSgn, v_n);
    __m128i v_mu1 = decode_onq_quad<1>(qinf, U_q, pLSB, MagSgn, v_n);

    // store mu
    // 0, 2, 4, 6, 1, 3, 5, 7
    auto t0 = _mm_unpacklo_epi32(v_mu0, v_mu1);
    auto t1 = _mm_unpackhi_epi32(v_mu0, v_mu1);
    v_mu0   = _mm_unpacklo_epi32(t0, t1);
    v_mu1   = _mm_unpackhi_epi32(t0, t1);
    _mm_storeu_si128((__m128i *)mp0, v_mu0);
    _mm_storeu_si128((__m128i *)mp1, v_mu1);
    mp0 += 4;
    mp1 += 4;

    // Update Exponent
    // t0   = _mm_unpacklo_epi32(v_v_quads0, v_v_quads1);
    // t1   = _mm_unpackhi_epi32(v_v_quads0, v_v_quads1);
    // vExp = _mm_unpackhi_epi32(t0, t1);
    vExp = sse_lzcnt_epi32(v_n);
    vExp = _mm_sub_epi32(_mm_set1_epi32(32), vExp);
    _mm_storeu_si128((__m128i *)E_p, vExp);
    E_p += 4;
  }

  // process the last block (left over)
  if (qx) {
    // Decoding of significance and EMB patterns and unsigned residual offsets
    vlcval       = VLC_dec.fetch();
    uint16_t tv0 = dec_table0[(vlcval & 0x7F) + (context << 7)];
    if (context == 0) {
      mel_run -= 2;
      tv0 = (mel_run == -1) ? tv0 : 0;
      if (mel_run < 0) {
        mel_run = MEL.get_run();
      }
    }
    sp[0] = tv0;
    rho0  = (tv0 & 0x00F0) >> 4;
    // emb_k_0 = (tv0 & 0x0F00) >> 8;
    // emb_1_0 = (tv0 & 0xF000) >> 12;

    // store sigma
    *sp0++   = (rho0 >> 0) & 1;
    *sp0++   = (rho0 >> 2) & 1;
    *sp1++   = (rho0 >> 1) & 1;
    *sp1++   = (rho0 >> 3) & 1;
    *rho_p++ = rho0;

    // UVLC decoding
    vlcval = VLC_dec.advance((tv0 & 0x000F) >> 1);

    u_off0 = tv0 & 1;

    uint32_t idx         = (vlcval & 0x3F) + (u_off0 << 6U);
    uint32_t uvlc_result = uvlc_dec_0[idx];
    // remove total prefix length
    vlcval = VLC_dec.advance(uvlc_result & 0x7);
    uvlc_result >>= 3;
    // extract suffixes for quad 0 and 1
    uint32_t len = uvlc_result & 0xF;                    // suffix length for 2 quads (up to 10 = 5 + 5)
    uint32_t tmp = vlcval & _bzhi_u32(UINT32_MAX, len);  // suffix value for 2 quads
    vlcval       = VLC_dec.advance(len);
    uvlc_result >>= 4;
    // quad 0 length
    len = uvlc_result & 0x7;  // quad 0 suffix length
    uvlc_result >>= 3;
    u0 = 1 + (uvlc_result & 7) + (tmp & ~(0xFFU << len));  // kappa = 1
    // U0    = kappa0 + u0;
    sp[1] = u0;

    __m128i v_n   = _mm_setzero_si128();
    __m128i qinf  = _mm_loadu_si128((__m128i *)sp);
    __m128i U_q   = _mm_srli_epi32(qinf, 16);
    __m128i v_mu0 = decode_onq_quad<0>(qinf, U_q, pLSB, MagSgn, v_n);

    // store mu
    *mp0++ = _mm_extract_epi32(v_mu0, 0);
    *mp0++ = _mm_extract_epi32(v_mu0, 2);
    *mp1++ = _mm_extract_epi32(v_mu0, 1);
    *mp1++ = _mm_extract_epi32(v_mu0, 3);

    // Update Exponent
    *E_p++ =
        static_cast<int32_t>(32 - count_leading_zeros(static_cast<uint32_t>(_mm_extract_epi32(v_n, 0))));
    *E_p++ =
        static_cast<int32_t>(32 - count_leading_zeros(static_cast<uint32_t>(_mm_extract_epi32(v_n, 1))));
  }  // Initial line-pair end

  /*******************************************************************************************************************/
  // Non-initial line-pair
  /*******************************************************************************************************************/
  for (uint16_t row = 1; row < QH; row++) {
    rho_p = rholine.get() + 1;
    E_p   = Eline.get() + 1;
    mp0   = block->sample_buf.get() + (row * 2U) * block->blksampl_stride;
    mp1   = mp0 + block->blksampl_stride;
    sp0   = block->block_states.get() + (row * 2U + 1U) * block->blkstate_stride + 1U;
    sp1   = sp0 + block->blkstate_stride;
    rho1  = 0;

    // calculate context for the next quad
    context = ((rho1 & 0x4) << 6) | ((rho1 & 0x8) << 5);            // (w | sw) << 8
    context |= ((rho_p[-1] & 0x8) << 4) | ((rho_p[0] & 0xa) << 6);  // ((nw | n) << 7) | (ne << 9)
    context |= ((rho_p[1] & 0x2) << 8);                             // (nf) << 9

    sp = scratch;
    for (qx = QW; qx >= 2; qx -= 2, sp += 4) {
      // Decoding of significance and EMB patterns and unsigned residual offsets
      vlcval       = VLC_dec.fetch();
      uint16_t tv0 = dec_table1[(vlcval & 0x7F) + context];
      if (context == 0) {
        mel_run -= 2;
        tv0 = (mel_run == -1) ? tv0 : 0;
        if (mel_run < 0) {
          mel_run = MEL.get_run();
        }
      }
      sp[0] = tv0;
      rho0  = _pext_u32(tv0, 0x00F0U);
      // emb_k_0 = _pext_u32(tv0, 0x0F00U);
      // emb_1_0 = _pext_u32(tv0, 0xF000U);

      vlcval = VLC_dec.advance((tv0 & 0x000F) >> 1);

      // calculate context for the next quad
      context = ((rho0 & 0x4) << 6) | ((rho0 & 0x8) << 5);           // (w | sw) << 8
      context |= ((rho_p[0] & 0x8) << 4) | ((rho_p[1] & 0xa) << 6);  // ((nw | n) << 7) | (ne << 9)
      context |= ((rho_p[2] & 0x2) << 8);                            // ( nf) << 9

      // Decoding of significance and EMB patterns and unsigned residual offsets
      uint16_t tv1 = dec_table1[(vlcval & 0x7F) + context];
      if (context == 0) {
        mel_run -= 2;
        tv1 = (mel_run == -1) ? tv1 : 0;
        if (mel_run < 0) {
          mel_run = MEL.get_run();
        }
      }
      sp[2] = tv1;
      rho1  = _pext_u32(tv1, 0x00F0U);
      // emb_k_1 = _pext_u32(tv1, 0x0F00U);
      // emb_1_1 = _pext_u32(tv1, 0xF000U);

      // calculate context for the next quad
      context = ((rho1 & 0x4) << 6) | ((rho1 & 0x8) << 5);           // (w | sw) << 8
      context |= ((rho_p[1] & 0x8) << 4) | ((rho_p[2] & 0xa) << 6);  // ((nw | n) << 7) | (ne << 9)
      context |= ((rho_p[3] & 0x2) << 8);                            // (nf) << 9

      // store sigma
      *sp0++ = (rho0 >> 0) & 1;
      *sp0++ = (rho0 >> 2) & 1;
      *sp0++ = (rho1 >> 0) & 1;
      *sp0++ = (rho1 >> 2) & 1;
      *sp1++ = (rho0 >> 1) & 1;
      *sp1++ = (rho0 >> 3) & 1;
      *sp1++ = (rho1 >> 1) & 1;
      *sp1++ = (rho1 >> 3) & 1;

      *rho_p++ = rho0;
      *rho_p++ = rho1;

      vlcval = VLC_dec.advance((tv1 & 0x000F) >> 1);

      // UVLC decoding
      u_off0       = tv0 & 1;
      u_off1       = tv1 & 1;
      uint32_t idx = (vlcval & 0x3F) + (u_off0 << 6U) + (u_off1 << 7U);

      uint32_t uvlc_result = uvlc_dec_1[idx];
      // remove total prefix length
      vlcval = VLC_dec.advance(uvlc_result & 0x7);
      uvlc_result >>= 3;
      // extract suffixes for quad 0 and 1
      uint32_t len = uvlc_result & 0xF;                    // suffix length for 2 quads (up to 10 = 5 + 5)
      uint32_t tmp = vlcval & _bzhi_u32(UINT32_MAX, len);  // suffix value for 2 quads
      vlcval       = VLC_dec.advance(len);
      uvlc_result >>= 4;
      // quad 0 length
      len = uvlc_result & 0x7;  // quad 0 suffix length
      uvlc_result >>= 3;
      u0 = (uvlc_result & 7) + (tmp & ~(0xFFU << len));
      u1 = (uvlc_result >> 3) + (tmp >> len);

      sp[1] = static_cast<uint16_t>(u0);
      sp[3] = static_cast<uint16_t>(u1);
    }

    // Calculate Emax for the next two quads
    int32_t Emax0, Emax1;
    Emax0 = find_max(E_p[-1], E_p[0], E_p[1], E_p[2]);
    Emax1 = find_max(E_p[1], E_p[2], E_p[3], E_p[4]);
    sp    = scratch;
    for (qx = QW; qx >= 2; qx -= 2, sp += 4) {
      __m128i v_n = _mm_setzero_si128();
      __m128i U_q;
      __m128i qinf = _mm_loadu_si128((__m128i *)sp);
      {
        // Compute gamma, kappa, U_q with Emax
        // The SIMD code below does the following
        //
        // gamma0 = (popcount32(rho0) < 2) ? 0 : 1;
        // gamma1 = (popcount32(rho1) < 2) ? 0 : 1;
        // kappa0 = (1 > gamma0 * (Emax0 - 1)) ? 1U : static_cast<uint8_t>(gamma0 * (Emax0 - 1));
        // kappa1 = (1 > gamma1 * (Emax1 - 1)) ? 1U : static_cast<uint8_t>(gamma1 * (Emax1 - 1));
        // U0     = kappa0 + u0;
        // U1     = kappa1 + u1;

        __m128i gamma, emax, kappa, u_q, w0;  // needed locally
        gamma = _mm_and_si128(qinf, _mm_set1_epi32(0xF0));
        w0    = _mm_sub_epi32(gamma, _mm_set1_epi32(1));
        gamma = _mm_and_si128(gamma, w0);
        gamma = _mm_cmpeq_epi32(gamma, _mm_setzero_si128());

        emax  = _mm_set_epi32(0, 0, Emax1 - 1, Emax0 - 1);
        emax  = _mm_andnot_si128(gamma, emax);
        kappa = _mm_set1_epi32(1);
        kappa = _mm_max_epi16(emax, kappa);  // no max_epi32 in ssse3

        u_q = _mm_srli_epi32(qinf, 16);
        U_q = _mm_add_epi32(u_q, kappa);
      }
      __m128i v_mu0 = decode_onq_quad<0>(qinf, U_q, pLSB, MagSgn, v_n);
      __m128i v_mu1 = decode_onq_quad<1>(qinf, U_q, pLSB, MagSgn, v_n);

      // store mu
      // 0, 2, 4, 6, 1, 3, 5, 7
      auto t0 = _mm_unpacklo_epi32(v_mu0, v_mu1);
      auto t1 = _mm_unpackhi_epi32(v_mu0, v_mu1);
      v_mu0   = _mm_unpacklo_epi32(t0, t1);
      v_mu1   = _mm_unpackhi_epi32(t0, t1);
      _mm_storeu_si128((__m128i *)mp0, v_mu0);
      _mm_storeu_si128((__m128i *)mp1, v_mu1);
      mp0 += 4;
      mp1 += 4;

      // Update Exponent
      Emax0 = find_max(E_p[3], E_p[4], E_p[5], E_p[6]);
      Emax1 = find_max(E_p[5], E_p[6], E_p[7], E_p[8]);
      vExp  = sse_lzcnt_epi32(v_n);
      vExp  = _mm_sub_epi32(_mm_set1_epi32(32), vExp);
      _mm_storeu_si128((__m128i *)E_p, vExp);
      E_p += 4;
    }

    // process the last block (left over)
    if (qx) {
      // Decoding of significance and EMB patterns and unsigned residual offsets
      vlcval       = VLC_dec.fetch();
      uint16_t tv0 = dec_table1[(vlcval & 0x7F) + context];
      if (context == 0) {
        mel_run -= 2;
        tv0 = (mel_run == -1) ? tv0 : 0;
        if (mel_run < 0) {
          mel_run = MEL.get_run();
        }
      }
      sp[0] = tv0;
      rho0  = (tv0 & 0x00F0) >> 4;
      // emb_k_0 = (tv0 & 0x0F00) >> 8;
      // emb_1_0 = (tv0 & 0xF000) >> 12;

      // store sigma
      *sp0++   = (rho0 >> 0) & 1;
      *sp0++   = (rho0 >> 2) & 1;
      *sp1++   = (rho0 >> 1) & 1;
      *sp1++   = (rho0 >> 3) & 1;
      *rho_p++ = rho0;

      // UVLC decoding
      vlcval = VLC_dec.advance((tv0 & 0x000F) >> 1);
      u_off0 = tv0 & 1;

      uint32_t idx         = (vlcval & 0x3F) + (u_off0 << 6U);
      uint32_t uvlc_result = uvlc_dec_0[idx];
      // remove total prefix length
      vlcval = VLC_dec.advance(uvlc_result & 0x7);
      uvlc_result >>= 3;
      // extract suffixes for quad 0 and 1
      uint32_t len = uvlc_result & 0xF;                    // suffix length for 2 quads (up to 10 = 5 + 5)
      uint32_t tmp = vlcval & _bzhi_u32(UINT32_MAX, len);  // suffix value for 2 quads
      vlcval       = VLC_dec.advance(len);
      uvlc_result >>= 4;
      // quad 0 length
      len = uvlc_result & 0x7;  // quad 0 suffix length
      uvlc_result >>= 3;
      u0 = (uvlc_result & 7) + (tmp & ~(0xFFU << len));

      gamma0 = (popcount32(rho0) < 2) ? 0 : 1;
      kappa0 = (1 > gamma0 * (Emax0 - 1)) ? 1U : static_cast<uint8_t>(gamma0 * (Emax0 - 1));
      U0     = kappa0 + u0;
      sp[1]  = U0;

      __m128i v_n   = _mm_setzero_si128();
      __m128i qinf  = _mm_loadu_si128((__m128i *)sp);
      __m128i U_q   = _mm_srli_epi32(qinf, 16);
      __m128i v_mu0 = decode_onq_quad<0>(qinf, U_q, pLSB, MagSgn, v_n);

      // store mu
      *mp0++ = _mm_extract_epi32(v_mu0, 0);
      *mp0++ = _mm_extract_epi32(v_mu0, 2);
      *mp1++ = _mm_extract_epi32(v_mu0, 1);
      *mp1++ = _mm_extract_epi32(v_mu0, 3);

      *E_p++ =
          static_cast<int32_t>(32 - count_leading_zeros(static_cast<uint32_t>(_mm_extract_epi32(v_n, 0))));
      *E_p++ =
          static_cast<int32_t>(32 - count_leading_zeros(static_cast<uint32_t>(_mm_extract_epi32(v_n, 1))));
    }
  }  // Non-Initial line-pair end
}

auto process_stripes_block_dec = [](SP_dec &SigProp, j2k_codeblock *block, const int32_t i_start,
                                    const int32_t j_start, const uint16_t width, const uint16_t height,
                                    const uint8_t &pLSB) {
  int32_t *sp;
  uint8_t causal_cond = 0;
  uint8_t bit;
  uint8_t mbr;
  const auto block_width  = static_cast<uint16_t>(j_start + width);
  const auto block_height = static_cast<uint16_t>(i_start + height);

  // Decode magnitude
  for (int16_t j = (int16_t)j_start; j < block_width; j++) {
    for (int16_t i = (int16_t)i_start; i < block_height; i++) {
      sp = &block->sample_buf[static_cast<size_t>(j) + static_cast<size_t>(i) * block->blksampl_stride];
      causal_cond = (((block->Cmodes & CAUSAL) == 0) || (i != block_height - 1));
      mbr         = 0;
      if (block->get_state(Sigma, i, j) == 0) {
        mbr = block->calc_mbr(i, j, causal_cond);
      }
      if (mbr != 0) {
        block->modify_state(refinement_indicator, 1, i, j);
        bit = SigProp.importSigPropBit();
        block->modify_state(refinement_value, bit, i, j);
        *sp |= bit << pLSB;
        *sp |= bit << (pLSB - 1);  // new bin center ( = 0.5)
      }
      block->modify_state(scan, 1, i, j);
    }
  }
  // Decode sign
  for (int16_t j = (int16_t)j_start; j < block_width; j++) {
    for (int16_t i = (int16_t)i_start; i < block_height; i++) {
      sp = &block->sample_buf[static_cast<size_t>(j) + static_cast<size_t>(i) * block->blksampl_stride];
      //      if ((*sp & (1 << pLSB)) != 0) {
      if (block->get_state(Refinement_value, i, j)) {
        *sp |= static_cast<int32_t>(SigProp.importSigPropBit()) << 31;
      }
    }
  }
};

void ht_sigprop_decode(j2k_codeblock *block, uint8_t *HT_magref_segment, uint32_t magref_length,
                       const uint8_t &pLSB) {
  SP_dec SigProp(HT_magref_segment, magref_length);
  const uint16_t num_v_stripe = static_cast<uint16_t>(block->size.y / 4);
  const uint16_t num_h_stripe = static_cast<uint16_t>(block->size.x / 4);
  int32_t i_start             = 0, j_start;
  uint16_t width              = 4;
  uint16_t width_last;
  uint16_t height = 4;

  // decode full-height (=4) stripes
  for (uint16_t n1 = 0; n1 < num_v_stripe; n1++) {
    j_start = 0;
    for (uint16_t n2 = 0; n2 < num_h_stripe; n2++) {
      process_stripes_block_dec(SigProp, block, i_start, j_start, width, height, pLSB);
      j_start += 4;
    }
    width_last = block->size.x % 4;
    if (width_last) {
      process_stripes_block_dec(SigProp, block, i_start, j_start, width_last, height, pLSB);
    }
    i_start += 4;
  }
  // decode remaining height stripes
  height  = block->size.y % 4;
  j_start = 0;
  for (uint16_t n2 = 0; n2 < num_h_stripe; n2++) {
    process_stripes_block_dec(SigProp, block, i_start, j_start, width, height, pLSB);
    j_start += 4;
  }
  width_last = block->size.x % 4;
  if (width_last) {
    process_stripes_block_dec(SigProp, block, i_start, j_start, width_last, height, pLSB);
  }
}

void ht_magref_decode(j2k_codeblock *block, uint8_t *HT_magref_segment, uint32_t magref_length,
                      const uint8_t &pLSB) {
  MR_dec MagRef(HT_magref_segment, magref_length);
  const uint16_t blk_height   = static_cast<uint16_t>(block->size.y);
  const uint16_t blk_width    = static_cast<uint16_t>(block->size.x);
  const uint16_t num_v_stripe = static_cast<uint16_t>(block->size.y / 4);
  int16_t i_start             = 0;
  int16_t height              = 4;
  int32_t *sp;
  int32_t bit;
  int32_t tmp;
  for (int16_t n1 = 0; n1 < num_v_stripe; n1++) {
    for (int16_t j = 0; j < blk_width; j++) {
      for (int16_t i = i_start; i < i_start + height; i++) {
        sp = &block->sample_buf[static_cast<size_t>(j) + static_cast<size_t>(i) * block->blksampl_stride];
        if (block->get_state(Sigma, i, j) != 0) {
          block->modify_state(refinement_indicator, 1, i, j);
          bit = MagRef.importMagRefBit();
          tmp = static_cast<int32_t>(0xFFFFFFFE | static_cast<unsigned int>(bit));
          tmp <<= pLSB;
          sp[0] &= tmp;
          sp[0] |= 1 << (pLSB - 1);  // new bin center ( = 0.5)
        }
      }
    }
    i_start = static_cast<int16_t>(i_start + 4);
  }
  height = static_cast<int16_t>(blk_height % 4);
  for (int16_t j = 0; j < blk_width; j++) {
    for (int16_t i = i_start; i < i_start + height; i++) {
      sp = &block->sample_buf[static_cast<size_t>(j) + static_cast<size_t>(i) * block->blksampl_stride];
      if (block->get_state(Sigma, i, j) != 0) {
        block->modify_state(refinement_indicator, 1, i, j);
        bit = MagRef.importMagRefBit();
        tmp = static_cast<int32_t>(0xFFFFFFFE | static_cast<unsigned int>(bit));
        tmp <<= pLSB;
        sp[0] &= tmp;
        sp[0] |= 1 << (pLSB - 1);  // new bin center ( = 0.5)
      }
    }
  }
}

void j2k_codeblock::dequantize(uint8_t ROIshift) const {
  // number of decoded magnitude bit‐planes
  const int32_t pLSB = 31 - M_b;  // indicates binary point;
  // bit mask for ROI detection
  const uint32_t mask = UINT32_MAX >> (M_b + 1);

  const __m256i magmask = _mm256_set1_epi32(0x7FFFFFFF);
  const __m256i vmask   = _mm256_set1_epi32(static_cast<int32_t>(~mask));
  const __m256i zero    = _mm256_setzero_si256();
  const __m256i shift   = _mm256_set1_epi32(ROIshift);
  __m256i v0, v1, s0, s1, vdst0, vdst1, vROImask;
  if (this->transformation) {
    // lossless path
    for (size_t i = 0; i < static_cast<size_t>(this->size.y); i++) {
      int32_t *val = this->sample_buf.get() + i * this->blksampl_stride;
      sprec_t *dst = this->i_samples + i * this->band_stride;
      size_t len   = this->size.x;
      for (; len >= 16; len -= 16) {
        v0 = _mm256_loadu_si256((__m256i *)val);
        v1 = _mm256_loadu_si256((__m256i *)(val + 8));
        s0 = v0;  //_mm256_or_si256(_mm256_and_si256(v0, signmask), one);
        s1 = v1;  //_mm256_or_si256(_mm256_and_si256(v1, signmask), one);
        v0 = _mm256_and_si256(v0, magmask);
        v1 = _mm256_and_si256(v1, magmask);
        // upshift background region, if necessary
        vROImask = _mm256_and_si256(v0, vmask);
        vROImask = _mm256_cmpeq_epi32(vROImask, zero);
        vROImask = _mm256_and_si256(vROImask, shift);
        v0       = _mm256_sllv_epi32(v0, vROImask);
        vROImask = _mm256_and_si256(v1, vmask);
        vROImask = _mm256_cmpeq_epi32(vROImask, zero);
        vROImask = _mm256_and_si256(vROImask, shift);
        v1       = _mm256_sllv_epi32(v1, vROImask);

        // convert values from sign-magnitude form to two's complement one
        vdst0 = _mm256_sign_epi32(_mm256_srai_epi32(v0, pLSB), s0);
        vdst1 = _mm256_sign_epi32(_mm256_srai_epi32(v1, pLSB), s1);
        v0    = _mm256_permute4x64_epi64(_mm256_packs_epi32(vdst0, vdst1), 0xD8);
        _mm256_storeu_si256((__m256i *)dst, v0);
        val += 16;
        dst += 16;
      }
      for (; len > 0; --len) {
        int32_t sign = *val & INT32_MIN;
        *val &= INT32_MAX;
        // detect background region and upshift it
        if (ROIshift && (((uint32_t)*val & ~mask) == 0)) {
          *val <<= ROIshift;
        }
        *val >>= pLSB;
        // convert sign-magnitude to two's complement form
        if (sign) {
          *val = -(*val & INT32_MAX);
        }

        assert(pLSB >= 0);  // assure downshift is not negative
        *dst = static_cast<int16_t>(*val);
        val++;
        dst++;
      }
    }
  } else {
    // lossy path
    float fscale = this->stepsize;
    fscale *= (1 << FRACBITS);
    if (M_b <= 31) {
      fscale /= (static_cast<float>(1 << (31 - M_b)));
    } else {
      fscale *= (static_cast<float>(1 << (M_b - 31)));
    }
    constexpr int32_t downshift = 15;
    fscale *= (float)(1 << 16) * (float)(1 << downshift);
    const auto scale = (int32_t)(fscale + 0.5);
    for (size_t i = 0; i < static_cast<size_t>(this->size.y); i++) {
      int32_t *val = this->sample_buf.get() + i * this->blksampl_stride;
      sprec_t *dst = this->i_samples + i * this->band_stride;
      size_t len   = this->size.x;
      for (; len >= 16; len -= 16) {
        v0 = _mm256_loadu_si256((__m256i *)val);
        v1 = _mm256_loadu_si256((__m256i *)(val + 8));
        s0 = v0;  //_mm256_or_si256(_mm256_and_si256(v0, signmask), one);
        s1 = v1;  //_mm256_or_si256(_mm256_and_si256(v1, signmask), one);
        v0 = _mm256_and_si256(v0, _mm256_set1_epi32(0x7FFFFFFF));
        v1 = _mm256_and_si256(v1, _mm256_set1_epi32(0x7FFFFFFF));
        // upshift background region, if necessary
        vROImask = _mm256_and_si256(v0, vmask);
        vROImask = _mm256_cmpeq_epi32(vROImask, zero);
        vROImask = _mm256_and_si256(vROImask, shift);
        v0       = _mm256_sllv_epi32(v0, vROImask);
        vROImask = _mm256_and_si256(v1, vmask);
        vROImask = _mm256_cmpeq_epi32(vROImask, zero);
        vROImask = _mm256_and_si256(vROImask, shift);
        v1       = _mm256_sllv_epi32(v1, vROImask);

        // to prevent overflow, truncate to int16_t range
        v0 = _mm256_srai_epi32(_mm256_add_epi32(v0, _mm256_set1_epi32(1 << 15)), 16);
        v1 = _mm256_srai_epi32(_mm256_add_epi32(v1, _mm256_set1_epi32(1 << 15)), 16);

        // dequantization
        v0 = _mm256_mullo_epi32(v0, _mm256_set1_epi32(scale));
        v1 = _mm256_mullo_epi32(v1, _mm256_set1_epi32(scale));

        // downshift and convert values from sign-magnitude form to two's complement one
        v0 = _mm256_srai_epi32(_mm256_add_epi32(v0, _mm256_set1_epi32(1 << (downshift - 1))), downshift);
        v1 = _mm256_srai_epi32(_mm256_add_epi32(v1, _mm256_set1_epi32(1 << (downshift - 1))), downshift);

        v0 = _mm256_sign_epi32(v0, s0);
        v1 = _mm256_sign_epi32(v1, s1);

        _mm256_storeu_si256((__m256i *)dst, _mm256_permute4x64_epi64(_mm256_packs_epi32(v0, v1), 0xD8));

        val += 16;
        dst += 16;
      }
      for (; len > 0; --len) {
        int32_t sign = *val & INT32_MIN;
        *val &= INT32_MAX;
        // detect background region and upshift it
        if (ROIshift && (((uint32_t)*val & ~mask) == 0)) {
          *val <<= ROIshift;
        }
        // to prevent overflow, truncate to int16_t
        *val = (*val + (1 << 15)) >> 16;
        //  dequantization
        *val *= scale;
        // downshift
        *val = (int16_t)((*val + (1 << (downshift - 1))) >> downshift);
        // convert sign-magnitude to two's complement form
        if (sign) {
          *val = -(*val & INT32_MAX);
        }
        *dst = static_cast<int16_t>(*val);
        val++;
        dst++;
      }
    }
  }
}

bool htj2k_decode(j2k_codeblock *block, const uint8_t ROIshift) {
  // number of placeholder pass
  uint8_t P0 = 0;
  // length of HT Cleanup segment
  int32_t Lcup = 0;
  // length of HT Refinement segment
  uint32_t Lref = 0;
  // number of HT Sets preceding the given(this) HT Set
  const uint8_t S_skip = 0;

  if (block->num_passes > 3) {
    for (uint32_t i = 0; i < block->pass_length.size(); i++) {
      if (block->pass_length[i] != 0) {
        break;
      }
      P0++;
    }
    P0 /= 3;
  } else if (block->length == 0 && block->num_passes != 0) {
    P0 = 1;
  } else {
    P0 = 0;
  }
  const uint8_t empty_passes = static_cast<uint8_t>(P0 * 3);
  if (block->num_passes < empty_passes) {
    printf("WARNING: number of passes %d exceeds number of empty passes %d", block->num_passes,
           empty_passes);
    return false;
  }
  // number of ht coding pass (Z_blk in the spec)
  const uint8_t num_ht_passes = static_cast<uint8_t>(block->num_passes - empty_passes);
  // pointer to buffer for HT Cleanup segment
  uint8_t *Dcup;
  // pointer to buffer for HT Refinement segment
  uint8_t *Dref;

  if (num_ht_passes > 0) {
    std::vector<uint8_t> all_segments;
    all_segments.reserve(3);
    for (uint32_t i = 0; i < block->pass_length.size(); i++) {
      if (block->pass_length[i] != 0) {
        all_segments.push_back(static_cast<uint8_t>(i));
      }
    }
    Lcup += static_cast<int32_t>(block->pass_length[all_segments[0]]);
    if (Lcup < 2) {
      printf("WARNING: Cleanup pass length must be at least 2 bytes in length.\n");
      return false;
    }
    for (uint32_t i = 1; i < all_segments.size(); i++) {
      Lref += block->pass_length[all_segments[i]];
    }
    Dcup = block->get_compressed_data();

    if (block->num_passes > 1 && all_segments.size() > 1) {
      Dref = block->get_compressed_data() + Lcup;
    } else {
      Dref = nullptr;
    }
    // number of (skipped) magnitude bitplanes
    const uint8_t S_blk = static_cast<uint8_t>(P0 + block->num_ZBP + S_skip);
    if (S_blk >= 30) {
      printf("WARNING: Number of skipped mag bitplanes %d is too large.\n", S_blk);
      return false;
    }
    // Suffix length (=MEL + VLC) of HT Cleanup pass
    const int32_t Scup = static_cast<int32_t>((Dcup[Lcup - 1] << 4) + (Dcup[Lcup - 2] & 0x0F));
    if (Scup < 2 || Scup > Lcup || Scup > 4079) {
      printf("WARNING: cleanup pass suffix length %d is invalid.\n", Scup);
      return false;
    }
    // modDcup (shall be done before the creation of state_VLC instance)
    Dcup[Lcup - 1] = 0xFF;
    Dcup[Lcup - 2] |= 0x0F;
    const int32_t Pcup = static_cast<int32_t>(Lcup - Scup);
    //    state_MS_dec MS     = state_MS_dec(Dcup, Pcup);
    //    state_MEL_unPacker MEL_unPacker = state_MEL_unPacker(Dcup, Lcup, Pcup);
    //    state_MEL_decoder MEL_decoder   = state_MEL_decoder(MEL_unPacker);
    //    state_VLC_dec VLC               = state_VLC_dec(Dcup, Lcup, Pcup);
    ht_cleanup_decode(block, static_cast<uint8_t>(30 - S_blk), Lcup, Pcup, Scup);
    if (num_ht_passes > 1) {
      ht_sigprop_decode(block, Dref, Lref, static_cast<uint8_t>(30 - (S_blk + 1)));
    }
    if (num_ht_passes > 2) {
      ht_magref_decode(block, Dref, Lref, static_cast<uint8_t>(30 - (S_blk + 1)));
    }

    // dequantization
    block->dequantize(ROIshift);

  }  // end

  return true;
}
#endif