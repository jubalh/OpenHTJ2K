// Copyright (c) 2019 - 2021, Osamu Watanabe
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

#pragma once
#include <cstdint>

#if __GNUC__ || __has_attribute(always_inline)
  #define FORCE_INLINE inline __attribute__((always_inline))
#elif defined(_MSC_VER)
  #define FORCE_INLINE __forceinline
#else
  #define FORCE_INLINE inline
#endif

#define MAX_Lcup 16834
#define MAX_Scup 4079
#define MAX_Lref 2046

/********************************************************************************
 * state_MS_enc: state class for MagSgn encoding
 *******************************************************************************/
class state_MS_enc {
 private:
  uint64_t Creg;       // temporal buffer to store up to 4 codewords
  uint32_t ctreg;      // number of used bits in Creg
  int32_t pos;         // current position in the buffer
  uint8_t last;        // last byte in the buffer
  uint8_t *const buf;  // buffer for MagSgn

  FORCE_INLINE void emit_dword() {  // internal function to emit 4 code words
    uint32_t bits_local = 0;
    uint32_t val        = Creg & 0xFFFFFFFF;
    uint32_t stuff      = (last == 0xFF);
    uint32_t tmp;

    uint32_t t = 0;
    tmp        = val & ((1 << (8 - stuff)) - 1);
    t |= tmp;
    bits_local += 8 - stuff;
    stuff = (tmp == 0xFF);

    tmp = (val >> (bits_local)) & ((1 << (8 - stuff)) - 1);
    t |= tmp << 8;
    bits_local += 8 - stuff;
    stuff = (tmp == 0xFF);

    tmp = (val >> (bits_local)) & ((1 << (8 - stuff)) - 1);
    t |= tmp << 16;
    bits_local += 8 - stuff;
    stuff = (tmp == 0xFF);

    tmp = (val >> (bits_local)) & ((1 << (8 - stuff)) - 1);
    t |= tmp << 24;
    bits_local += 8 - stuff;
    last = tmp & 0xFF;

    //  for (int i = 0; i < 4; ++i) {
    //    if (last == 0xFF) {
    //      last = static_cast<uint8_t>(Creg & 0x7F);
    //      Creg >>= 7;
    //      ctreg -= 7;
    //    } else {
    //      last = static_cast<uint8_t>(Creg & 0xFF);
    //      Creg >>= 8;
    //      ctreg -= 8;
    //    }
    //    t |= static_cast<uint32_t>(last) << bits_local;
    //    bits_local += 8;
    //  }

    Creg >>= bits_local;
    ctreg -= bits_local;
    *reinterpret_cast<uint32_t *>(buf + pos) = t;
    pos += 4;
  }

 public:
  explicit state_MS_enc(uint8_t *p) : Creg(0), ctreg(0), pos(0), last(0), buf(p) {}
  //  FORCE_INLINE void emitMagSgnBits(uint32_t cwd, uint8_t len, uint8_t emb_1) {
  //    int32_t temp = emb_1 << len;
  //    cwd -= static_cast<uint32_t>(temp);
  //    Creg |= static_cast<uint64_t>(cwd) << ctreg;
  //    ctreg += len;
  //    while (ctreg >= 32) {
  //      emit_dword();
  //    }
  //  }
  FORCE_INLINE void emitBits(int32x4_t &v, int32x4_t &m, int32x4_t &emb1) {
    int32x4_t tmp = vshlq_s32(emb1, m);
    v             = vsubq_s32(v, tmp);
    for (int i = 0; i < 4; ++i) {
      Creg |= static_cast<uint64_t>(v[i]) << ctreg;
      ctreg += static_cast<unsigned int>(m[i]);
      while (ctreg >= 32) {
        emit_dword();
      }
    }
  }
  FORCE_INLINE int32_t termMS() {
    while (true) {
      if (last == 0xFF) {
        if (ctreg < 7) break;
        last = static_cast<uint8_t>(Creg & 0x7F);
        Creg >>= 7;
        ctreg -= 7;
      } else {
        if (ctreg < 8) break;
        last = static_cast<uint8_t>(Creg & 0xFF);
        Creg >>= 8;
        ctreg -= 8;
      }
      buf[pos++] = last;
    }
    bool last_was_FF = (last == 0xFF);
    uint8_t fill_mask, cwd;
    if (ctreg > 0) {
      fill_mask = static_cast<uint8_t>(0xFF << ctreg);
      if (last_was_FF) {
        fill_mask &= 0x7F;
      }
      cwd = static_cast<uint8_t>(Creg |= fill_mask);
      if (cwd != 0xFF) {
        buf[pos++] = cwd;
      }
    } else if (last_was_FF) {
      pos--;
      buf[pos] = 0x00;  // may be not necessary
    }
    return pos;  // return current position as Pcup
  }
};

class state_MEL_enc;  // forward declaration for friend function "termMELandVLC()"
/********************************************************************************
 * state_VLC_enc: state class for VLC encoding
 *******************************************************************************/
class state_VLC_enc {
 private:
  uint8_t *const buf;
  uint8_t tmp;
  uint8_t last;
  uint8_t bits;
  int32_t pos;

  friend int32_t termMELandVLC(state_VLC_enc &, state_MEL_enc &);

 public:
  explicit state_VLC_enc(uint8_t *p) : buf(p), tmp(0xF), last(0xFF), bits(4), pos(MAX_Scup - 2) {
    buf[pos + 1] = 0xFF;
  }

  FORCE_INLINE void emitVLCBits(uint32_t cwd, uint32_t len) {
    for (; len > 0;) {
      int32_t available_bits = 8 - (last > 0x8F) - bits;
      int32_t t              = std::min(available_bits, (int32_t)len);
      tmp |= static_cast<uint8_t>((cwd & ((1 << t) - 1)) << bits);
      bits = static_cast<uint8_t>(bits + t);
      available_bits -= t;
      len -= t;
      cwd >>= t;
      if (available_bits == 0) {
        if ((last > 0x8f) && tmp != 0x7F) {
          last = 0x00;
          continue;
        }
        buf[pos] = tmp;
        pos--;  // reverse order
        last = tmp;
        tmp  = 0;
        bits = 0;
      }
    }
  }
};

/********************************************************************************
 * state_MEL_enc: state class for MEL encoding
 *******************************************************************************/
class state_MEL_enc {
 private:
  int8_t MEL_k;
  uint8_t MEL_run;
  const uint8_t MEL_E[13];
  uint8_t MEL_t;
  int32_t pos;
  uint8_t rem;
  uint8_t tmp;
  uint8_t *const buf;
  void emitMELbit(uint8_t bit);

  friend int32_t termMELandVLC(state_VLC_enc &, state_MEL_enc &);

 public:
  explicit state_MEL_enc(uint8_t *p)
      : MEL_k(0),
        MEL_run(0),
        MEL_E{0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 4, 5},
        MEL_t(static_cast<uint8_t>(1 << MEL_E[MEL_k])),
        pos(0),
        rem(8),
        tmp(0),
        buf(p) {}
  void encodeMEL(uint8_t smel);
  void termMEL();
};

class MR_enc;  // forward declaration for friend function termSPandMR()
/********************************************************************************
 * SP_enc: state class for HT SigProp encoding
 *******************************************************************************/
class SP_enc {
 private:
  uint32_t pos;
  uint8_t bits;
  uint8_t max;
  uint8_t tmp;
  uint8_t *const buf;
  friend int32_t termSPandMR(SP_enc &, MR_enc &);

 public:
  explicit SP_enc(uint8_t *Dref) : pos(0), bits(0), max(8), tmp(0), buf(Dref) {}
  void emitSPBit(uint8_t bit) {
    tmp |= static_cast<uint8_t>(bit << bits);
    bits++;
    if (bits == max) {
      buf[pos] = tmp;
      pos++;
      max  = (tmp == 0xFF) ? 7 : 8;
      tmp  = 0;
      bits = 0;
    }
  }
  void termSP() {
    if (tmp != 0) {
      buf[pos] = tmp;
      pos++;
      max = (tmp == 0xFF) ? 7 : 8;
    }
    if (max == 7) {
      buf[pos] = 0x00;
      pos++;  // this prevents the appearance of a terminal 0xFF
    }
  }
  [[nodiscard]] uint32_t get_length() const { return pos; }
};
/********************************************************************************
 * MR_enc: state class for HT MagRef encoding
 *******************************************************************************/
class MR_enc {
 private:
  uint32_t pos;
  uint8_t bits;
  uint8_t tmp;
  uint8_t last;
  uint8_t *const buf;
  friend int32_t termSPandMR(SP_enc &, MR_enc &);

 public:
  explicit MR_enc(uint8_t *Dref) : pos(MAX_Lref), bits(0), tmp(0), last(255), buf(Dref) {}
  void emitMRBit(uint8_t bit) {
    tmp |= static_cast<uint8_t>(bit << bits);
    bits++;
    if ((last > 0x8F) && (tmp == 0x7F)) {
      bits++;  // this must leave MR_bits equal to 8
    }
    if (bits == 8) {
      buf[pos] = tmp;
      pos--;  // MR buf gorws reverse order
      last = tmp;
      tmp  = 0;
      bits = 0;
    }
  }
  [[nodiscard]] uint32_t get_length() const { return MAX_Lref - pos; }
};
