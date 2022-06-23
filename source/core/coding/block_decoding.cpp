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

#include "coding_units.hpp"
#include "mq_decoder.hpp"
#include "coding_local.hpp"
#include "EBCOTtables.hpp"

void j2k_codeblock::update_sample(const uint8_t &symbol, const uint8_t &p, const int16_t &j1,
                                  const int16_t &j2) const {
  sample_buf[static_cast<size_t>(j2) + static_cast<size_t>(j1) * blksampl_stride] |=
      static_cast<int32_t>(symbol) << p;
}

void j2k_codeblock::update_sign(const int8_t &val, const int16_t &j1, const int16_t &j2) const {
  sample_buf[static_cast<size_t>(j2) + static_cast<size_t>(j1) * blksampl_stride] |= val << 31;
}

uint8_t j2k_codeblock::get_sign(const int16_t &j1, const int16_t &j2) const {
  return static_cast<uint8_t>(
      ((uint32_t)(sample_buf[static_cast<size_t>(j2) + static_cast<size_t>(j1) * blksampl_stride] | 0x8000))
      >> 31);
}

uint8_t j2k_codeblock::get_context_label_sig(const int16_t &j1, const int16_t &j2) const {
  int32_t idx        = 0;
  const int16_t j1m1 = static_cast<int16_t>(j1 - 1);
  const int16_t j1p1 = static_cast<int16_t>(j1 + 1);
  const int16_t j2m1 = static_cast<int16_t>(j2 - 1);
  const int16_t j2p1 = static_cast<int16_t>(j2 + 1);
  // one line above left, center, right
  idx = get_state(Sigma, j1m1, j2m1);
  idx += get_state(Sigma, j1m1, j2) << 4;
  idx += get_state(Sigma, j1m1, j2p1) << 1;
  // current line left, right
  idx += get_state(Sigma, j1, j2m1) << 6;
  idx += get_state(Sigma, j1, j2p1) << 7;
  // one line below left, center, right
  idx += get_state(Sigma, j1p1, j2m1) << 2;
  idx += get_state(Sigma, j1p1, j2) << 5;
  idx += get_state(Sigma, j1p1, j2p1) << 3;

  if ((this->Cmodes & CAUSAL) && j1 % 4 == 3) {
    idx &= 0xD3;
  }
  return sig_LUT[this->band][idx];
}
uint8_t j2k_codeblock::get_signLUT_index(const int16_t &j1, const int16_t &j2) const {
  int32_t idx        = 0;
  const int16_t j1m1 = static_cast<int16_t>(j1 - 1);
  const int16_t j1p1 = static_cast<int16_t>(j1 + 1);
  const int16_t j2m1 = static_cast<int16_t>(j2 - 1);
  const int16_t j2p1 = static_cast<int16_t>(j2 + 1);
  idx += get_state(Sigma, j1m1, j2);                                             // top
  idx += (j1 > 0) ? get_sign(j1m1, j2) << 4 : 0;                                 // top
  idx += get_state(Sigma, j1, j2m1) << 2;                                        // left
  idx += (j2 > 0) ? get_sign(j1, j2m1) << 6 : 0;                                 // left
  idx += get_state(Sigma, j1, j2p1) << 3;                                        // right
  idx += (j2 < static_cast<int16_t>(size.x) - 1) ? get_sign(j1, j2p1) << 7 : 0;  // right
  idx += get_state(Sigma, j1p1, j2) << 1;                                        // bottom
  idx += (j1 < static_cast<int16_t>(size.y) - 1) ? get_sign(j1p1, j2) << 5 : 0;  // bottom
  return static_cast<uint8_t>(idx);
}

void decode_j2k_sign_raw(j2k_codeblock *block, mq_decoder &mq_dec, const int16_t &j1, const int16_t &j2) {
  uint8_t symbol = mq_dec.get_raw_symbol();
  block->update_sign(static_cast<int8_t>(symbol), j1, j2);
}
void decode_j2k_sign(j2k_codeblock *block, mq_decoder &mq_dec, const int16_t &j1, const int16_t &j2) {
  uint8_t idx = block->get_signLUT_index(j1, j2);
  if ((block->Cmodes & CAUSAL) && j1 % 4 == 3) {
    idx &= 0xDD;
  }
  const uint8_t x      = mq_dec.decode(sign_LUT[0][idx]);
  const uint8_t XORbit = sign_LUT[1][idx];
  block->update_sign((x ^ XORbit) & 1, j1, j2);
}

void decode_sigprop_pass_raw(j2k_codeblock *block, const uint8_t &p, mq_decoder &mq_dec) {
  uint16_t num_v_stripe = static_cast<uint16_t>(block->size.y / 4);
  int16_t j1, j2, j1_start = 0;
  uint8_t label_sig;
  uint8_t symbol;
  for (uint16_t n = 0; n < num_v_stripe; n++) {
    for (j2 = 0; j2 < static_cast<int16_t>(block->size.x); j2++) {
      for (j1 = j1_start; j1 < j1_start + 4; j1++) {
        label_sig = block->get_context_label_sig(j1, j2);
        if (block->get_state(Sigma, j1, j2) == 0 && label_sig > 0) {
          block->modify_state(decoded_bitplane_index, p, j1, j2);
          symbol = mq_dec.get_raw_symbol();
          block->update_sample(symbol, p, j1, j2);
          if (symbol) {
            block->modify_state(sigma, symbol, j1, j2);  // symbol shall be 1
            decode_j2k_sign_raw(block, mq_dec, j1, j2);
          }
          block->modify_state(pi_, 1, j1, j2);
        } else {
          block->modify_state(pi_, 0, j1, j2);
        }
      }
    }
    j1_start = static_cast<int16_t>(j1_start + 4);
  }

  if (block->size.y % 4) {
    for (j2 = 0; j2 < static_cast<int16_t>(block->size.x); j2++) {
      for (j1 = j1_start; j1 < j1_start + static_cast<int16_t>(block->size.y % 4); j1++) {
        label_sig = block->get_context_label_sig(j1, j2);
        if (block->get_state(Sigma, j1, j2) == 0 && label_sig > 0) {
          block->modify_state(decoded_bitplane_index, p, j1, j2);
          symbol = mq_dec.get_raw_symbol();
          block->update_sample(symbol, p, j1, j2);
          if (symbol) {
            block->modify_state(sigma, symbol, j1, j2);  // symbol shall be 1
            decode_j2k_sign_raw(block, mq_dec, j1, j2);
          }
          block->modify_state(pi_, 1, j1, j2);
        } else {
          block->modify_state(pi_, 0, j1, j2);
        }
      }
    }
  }
}

void decode_sigprop_pass(j2k_codeblock *block, const uint8_t &p, mq_decoder &mq_dec) {
  uint16_t num_v_stripe = static_cast<uint16_t>(block->size.y / 4);
  int16_t j1, j2, j1_start = 0;
  uint8_t label_sig;
  uint8_t symbol;
  for (uint16_t n = 0; n < num_v_stripe; n++) {
    for (j2 = 0; j2 < static_cast<int16_t>(block->size.x); j2++) {
      for (j1 = j1_start; j1 < j1_start + 4; j1++) {
        label_sig = block->get_context_label_sig(j1, j2);
        if (block->get_state(Sigma, j1, j2) == 0 && label_sig > 0) {
          block->modify_state(decoded_bitplane_index, p, j1, j2);
          symbol = mq_dec.decode(label_sig);
          block->update_sample(symbol, p, j1, j2);
          if (symbol) {
            block->modify_state(sigma, symbol, j1, j2);  // symbol shall be 1
            decode_j2k_sign(block, mq_dec, j1, j2);
          }
          block->modify_state(pi_, 1, j1, j2);
        } else {
          block->modify_state(pi_, 0, j1, j2);
        }
      }
    }
    j1_start = static_cast<int16_t>(j1_start + 4);
  }

  if (block->size.y % 4) {
    for (j2 = 0; j2 < static_cast<int16_t>(block->size.x); j2++) {
      for (j1 = j1_start; j1 < j1_start + static_cast<int16_t>(block->size.y % 4); j1++) {
        label_sig = block->get_context_label_sig(j1, j2);
        if (block->get_state(Sigma, j1, j2) == 0 && label_sig > 0) {
          block->modify_state(decoded_bitplane_index, p, j1, j2);
          symbol = mq_dec.decode(label_sig);
          block->update_sample(symbol, p, j1, j2);
          if (symbol) {
            block->modify_state(sigma, symbol, j1, j2);  // symbol shall be 1
            decode_j2k_sign(block, mq_dec, j1, j2);
          }
          block->modify_state(pi_, 1, j1, j2);
        } else {
          block->modify_state(pi_, 0, j1, j2);
        }
      }
    }
  }
}

void decode_magref_pass_raw(j2k_codeblock *block, const uint8_t &p, mq_decoder &mq_dec) {
  uint16_t num_v_stripe = static_cast<uint16_t>(block->size.y / 4);
  int16_t j1, j2, j1_start = 0;
  uint8_t symbol;
  for (uint16_t n = 0; n < num_v_stripe; n++) {
    for (j2 = 0; j2 < static_cast<int16_t>(block->size.x); j2++) {
      for (j1 = j1_start; j1 < j1_start + 4; j1++) {
        if (block->get_state(Sigma, j1, j2) == 1 && block->get_state(Pi_, j1, j2) == 0) {
          block->modify_state(decoded_bitplane_index, p, j1, j2);
          symbol = mq_dec.get_raw_symbol();
          block->update_sample(symbol, p, j1, j2);
          block->modify_state(sigma_, 1, j1, j2);
        }
      }
    }
    j1_start = static_cast<int16_t>(j1_start + 4);
  }

  if (block->size.y % 4 != 0) {
    for (j2 = 0; j2 < static_cast<int16_t>(block->size.x); j2++) {
      for (j1 = j1_start; j1 < j1_start + static_cast<int16_t>(block->size.y % 4); j1++) {
        if (block->get_state(Sigma, j1, j2) == 1 && block->get_state(Pi_, j1, j2) == 0) {
          block->modify_state(decoded_bitplane_index, p, j1, j2);
          symbol = mq_dec.get_raw_symbol();
          block->update_sample(symbol, p, j1, j2);
          block->modify_state(sigma_, 1, j1, j2);
        }
      }
    }
  }
}

void decode_magref_pass(j2k_codeblock *block, const uint8_t &p, mq_decoder &mq_dec) {
  uint16_t num_v_stripe = static_cast<uint16_t>(block->size.y / 4);
  int16_t j1, j2, j1_start = 0;
  uint8_t label_sig, label_mag = 0;
  uint8_t symbol;
  for (uint16_t n = 0; n < num_v_stripe; n++) {
    for (j2 = 0; j2 < static_cast<int16_t>(block->size.x); j2++) {
      for (j1 = j1_start; j1 < j1_start + 4; j1++) {
        if (block->get_state(Sigma, j1, j2) == 1 && block->get_state(Pi_, j1, j2) == 0) {
          block->modify_state(decoded_bitplane_index, p, j1, j2);
          label_sig = block->get_context_label_sig(j1, j2);
          if (block->get_state(Sigma_, j1, j2) == 0 && label_sig == 0) {
            label_mag = 14;
          } else if (block->get_state(Sigma_, j1, j2) == 0 && label_sig > 0) {
            label_mag = 15;
          } else if (block->get_state(Sigma_, j1, j2) == 1) {
            label_mag = 16;
          }
          symbol = mq_dec.decode(label_mag);
          block->update_sample(symbol, p, j1, j2);
          block->modify_state(sigma_, 1, j1, j2);
        }
      }
    }
    j1_start = static_cast<int16_t>(j1_start + 4);
  }

  if (block->size.y % 4 != 0) {
    for (j2 = 0; j2 < static_cast<int16_t>(block->size.x); j2++) {
      for (j1 = j1_start; j1 < j1_start + static_cast<int16_t>(block->size.y % 4); j1++) {
        if (block->get_state(Sigma, j1, j2) == 1 && block->get_state(Pi_, j1, j2) == 0) {
          block->modify_state(decoded_bitplane_index, p, j1, j2);
          label_sig = block->get_context_label_sig(j1, j2);
          if (block->get_state(Sigma_, j1, j2) == 0 && label_sig == 0) {
            label_mag = 14;
          } else if (block->get_state(Sigma_, j1, j2) == 0 && label_sig > 0) {
            label_mag = 15;
          } else if (block->get_state(Sigma_, j1, j2) == 1) {
            label_mag = 16;
          }
          symbol = mq_dec.decode(label_mag);
          block->update_sample(symbol, p, j1, j2);
          block->modify_state(sigma_, 1, j1, j2);
        }
      }
    }
  }
}

void decode_cleanup_pass(j2k_codeblock *block, const uint8_t &p, mq_decoder &mq_dec) {
  uint16_t num_v_stripe = static_cast<uint16_t>(block->size.y / 4);
  int16_t j1, j2, j1_start = 0;
  uint8_t label_sig;
  const uint8_t label_run = 17;
  const uint8_t label_uni = 18;
  uint8_t symbol          = 0;
  int32_t k;
  int32_t r = 0;
  for (uint16_t n = 0; n < num_v_stripe; n++) {
    for (j2 = 0; j2 < static_cast<int16_t>(block->size.x); j2++) {
      k = 4;
      while (k > 0) {
        j1 = static_cast<int16_t>(j1_start + 4 - k);
        r  = -1;
        if (j1 % 4 == 0 && j1 <= static_cast<int16_t>(block->size.y) - 4) {
          label_sig = 0;
          for (int i = 0; i < 4; i++) {
            label_sig =
                label_sig
                | static_cast<uint8_t>(block->get_context_label_sig(static_cast<int16_t>(j1 + i), j2));
          }
          if (label_sig == 0) {
            symbol = mq_dec.decode(label_run);
            if (symbol == 0) {
              r = 4;
            } else {
              r = mq_dec.decode(label_uni);
              r <<= 1;
              r += mq_dec.decode(label_uni);
              block->update_sample(1, p, static_cast<int16_t>(j1 + r), j2);
            }
            k -= r;
          }
          if (k != 0) {
            j1 = static_cast<int16_t>(j1_start + 4 - k);
          }
        }
        if (block->get_state(Sigma, j1, j2) == 0 && block->get_state(Pi_, j1, j2) == 0) {
          block->modify_state(decoded_bitplane_index, p, j1, j2);
          if (r >= 0) {
            r = r - 1;
          } else {
            label_sig = block->get_context_label_sig(j1, j2);
            symbol    = mq_dec.decode(label_sig);
            block->update_sample(symbol, p, j1, j2);
          }
          if (block->sample_buf[static_cast<size_t>(j2) + static_cast<size_t>(j1) * block->blksampl_stride]
              == static_cast<int32_t>(1) << p) {
            block->modify_state(sigma, 1, j1, j2);
            decode_j2k_sign(block, mq_dec, j1, j2);
          }
        }
        k--;
      }
    }
    j1_start = static_cast<int16_t>(j1_start + 4);
  }

  if (block->size.y % 4 != 0) {
    for (j2 = 0; j2 < static_cast<int16_t>(block->size.x); j2++) {
      for (j1 = j1_start; j1 < j1_start + static_cast<int16_t>(block->size.y % 4); j1++) {
        if (block->get_state(Sigma, j1, j2) == 0 && block->get_state(Pi_, j1, j2) == 0) {
          block->modify_state(decoded_bitplane_index, p, j1, j2);
          label_sig = block->get_context_label_sig(j1, j2);
          symbol    = mq_dec.decode(label_sig);
          block->update_sample(symbol, p, j1, j2);
          if (symbol) {
            block->modify_state(sigma, 1, j1, j2);
            decode_j2k_sign(block, mq_dec, j1, j2);
          }
        }
      }
    }
  }
}

void j2k_decode(j2k_codeblock *block, const uint8_t ROIshift) {
  constexpr uint8_t label_uni = 18;

  uint8_t num_decode_pass = 0;
  for (uint16_t i = 0; i < block->num_layers; i++) {
    num_decode_pass = static_cast<uint8_t>(num_decode_pass + block->layer_passes[i]);
  }
  mq_decoder mq_dec(block->get_compressed_data());
  // mq_dec.set_dynamic_table();

  const auto M_b           = static_cast<int32_t>(block->get_Mb());
  const int32_t K          = M_b + static_cast<int32_t>(ROIshift) - static_cast<int32_t>(block->num_ZBP);
  const int32_t max_passes = 3 * (K)-2;

  uint8_t z                    = 0;  // pass index
  uint8_t k                    = 2;  // pass category (0 = sig, 1 = mag, 2 = cleanup)
  uint8_t pmsb                 = static_cast<uint8_t>(30 - block->num_ZBP);  // index of the MSB
  uint8_t p                    = pmsb;                                       // index of current bitplane
  uint8_t current_segment_pass = 0;  // number of passes in a current codeword segment
  uint32_t segment_bytes       = 0;
  uint32_t segment_pos         = 0;  // position of a current codeword segment
  uint8_t bypass_threshold     = 0;  // threshold of pass index in BYPASS mode

  if (block->Cmodes & BYPASS) {
    bypass_threshold = 10;
  }
  bool is_bypass = false;  // flag for BYPASS mode
  while (z < num_decode_pass) {
    if (k == 3) {
      k = 0;
      p--;  // move down to the next bitplane
    }
    if (current_segment_pass == 0) {
      // segment_start = z;
      current_segment_pass = (uint8_t)max_passes;

      // BYPASS mode
      if (bypass_threshold > 0) {
        if (z < bypass_threshold) {
          current_segment_pass = static_cast<uint8_t>(bypass_threshold - z);
        } else if (k == 2) {  // Cleanup pass
          current_segment_pass = 1;
          is_bypass            = false;
        } else {  // Sigprop or Magref pass
          current_segment_pass = 2;
          is_bypass            = true;
        }
      }

      // RESTART mode
      if (block->Cmodes & RESTART) {
        current_segment_pass = 1;
      }

      if ((z + current_segment_pass) > num_decode_pass) {
        current_segment_pass = static_cast<uint8_t>(num_decode_pass - z);
        if (num_decode_pass < max_passes) {
          // TODO: ?truncated?
        }
      }

      segment_bytes = 0;
      for (uint8_t n = 0; n < current_segment_pass; n++) {
        segment_bytes += block->pass_length[static_cast<size_t>(z + n)];
      }
      mq_dec.init(segment_pos, segment_bytes, is_bypass);
      segment_pos += segment_bytes;
    }

    if (z == 0 || block->Cmodes & RESET) {
      mq_dec.init_states_for_all_contexts();
    }

    if (k == 0) {
      if (is_bypass) {
        decode_sigprop_pass_raw(block, p, mq_dec);
      } else {
        decode_sigprop_pass(block, p, mq_dec);
      }
    } else if (k == 1) {
      if (is_bypass) {
        decode_magref_pass_raw(block, p, mq_dec);
      } else {
        decode_magref_pass(block, p, mq_dec);
      }
    } else {
      decode_cleanup_pass(block, p, mq_dec);
      if (block->Cmodes & SEGMARK) {
        int32_t r = 0;
        for (uint8_t i = 0; i < 4; i++) {
          r <<= 1;
          r += mq_dec.decode(label_uni);
        }
        if (r != 10) {
          printf("ERROR: SEGMARK test failed.\n");
          throw std::exception();
        }
      }
    }
    current_segment_pass--;
    if (current_segment_pass == 0) {
      mq_dec.finish();
    }
    z++;
    k++;
  }  // end of while

  // number of decoded magnitude bits, see D.2.1 in the spec
  int32_t N_b;
  // indicates binary point
  const int32_t pLSB = 31 - M_b;
#ifdef TEMP
  for (uint16_t y = 0; y < block->size.y; y++) {
    for (uint16_t x = 0; x < block->size.x; x++) {
      if (ROIshift) {
        N_b = 30 - pLSB + 1;
      } else {
        N_b = 30 - block->get_state(Decoded_bitplane_index, y, x) + 1;
      }
      block->dequantize(y, x, N_b, pLSB, ROIshift);
    }
  }
#else
  // bit mask for ROI detection
  const uint32_t mask = UINT32_MAX >> (M_b + 1);
  // reconstruction parameter defined in E.1.1.2 of the spec
  int32_t r_val;
  int32_t offset = 0;

  int32_t *val = nullptr;
  sprec_t *dst = nullptr;
  int32_t sign;
  int16_t QF15;
  float fscale = block->stepsize;
  fscale *= (1 << FRACBITS);
  if (M_b <= 31) {
    fscale /= (static_cast<float>(1 << (31 - M_b)));
  } else {
    fscale *= (static_cast<float>(1 << (M_b - 31)));
  }
  constexpr int32_t downshift = 15;
  fscale *= (float)(1 << 16) * (float)(1 << downshift);
  const auto scale = (int32_t)(fscale + 0.5);

  if (block->transformation) {
    // reversible path
    for (int16_t y = 0; y < static_cast<int16_t>(block->size.y); y++) {
      for (int16_t x = 0; x < static_cast<int16_t>(block->size.x); x++) {
        const uint32_t n = static_cast<uint32_t>(x) + static_cast<uint32_t>(y) * block->band_stride;
        val  = &block->sample_buf[static_cast<size_t>(x) + static_cast<size_t>(y) * block->blksampl_stride];
        dst  = block->i_samples + n;
        sign = *val & INT32_MIN;
        *val &= INT32_MAX;
        // detect background region and upshift it
        if (ROIshift && (((uint32_t)*val & ~mask) == 0)) {
          *val <<= ROIshift;
        }
        // do adjustment of the position indicating 0.5
        if (ROIshift) {
          N_b = 30 - pLSB + 1;
        } else {
          N_b = 30 - block->get_state(Decoded_bitplane_index, y, x) + 1;
        }
        // construct reconstruction value (=0.5)
        offset = (M_b > N_b) ? M_b - N_b : 0;
        r_val  = 1 << (pLSB - 1 + offset);
        // add 0.5 if necessary
        if (*val != 0 && N_b < M_b) {
          *val |= r_val;
        }
        // bring sign back
        *val |= sign;
        // convert sign-magnitude to two's complement form
        if (*val < 0) {
          *val = -(*val & INT32_MAX);
        }

        assert(pLSB >= 0);  // assure downshift is not negative
        QF15 = static_cast<int16_t>(*val >> pLSB);
        *dst = QF15;
      }
    }
  } else {
    // irreversible path
    for (int16_t y = 0; y < static_cast<int16_t>(block->size.y); y++) {
      for (int16_t x = 0; x < static_cast<int16_t>(block->size.x); x++) {
        const uint32_t n = static_cast<uint32_t>(x) + static_cast<uint32_t>(y) * block->band_stride;
        val  = &block->sample_buf[static_cast<size_t>(x) + static_cast<size_t>(y) * block->blksampl_stride];
        dst  = block->i_samples + n;
        sign = *val & INT32_MIN;  // extract sign bit
        *val &= INT32_MAX;        // delete sign bit temporally
        // detect background region and upshift it
        if (ROIshift && (((uint32_t)*val & ~mask) == 0)) {
          *val <<= ROIshift;
        }
        // do adjustment of the position indicating 0.5
        if (ROIshift) {
          N_b = 30 - pLSB + 1;
        } else {
          N_b = 30 - block->get_state(Decoded_bitplane_index, y, x) + 1;
        }
        // construct reconstruction value (=0.5)
        offset = (M_b > N_b) ? M_b - N_b : 0;
        r_val  = 1 << (pLSB - 1 + offset);
        // add 0.5, if necessary
        if (*val != 0) {
          *val |= r_val;
        }
        // to prevent overflow, truncate to int16_t
        *val = (*val + (1 << 15)) >> 16;
        // dequantization
        *val *= scale;
        // downshift
        QF15 = (int16_t)((*val + (1 << (downshift - 1))) >> downshift);
        // convert sign-magnitude to two's complement form
        if (sign) {
          QF15 = static_cast<int16_t>(-QF15);
        }
        *dst = QF15;
      }
    }
  }
#endif
  // TODO: if k !=0
}
