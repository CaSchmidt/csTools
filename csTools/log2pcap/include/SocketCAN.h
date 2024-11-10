/****************************************************************************
** Copyright (c) 2024, Carsten Schmidt. All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
**
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************/

#pragma once

#include <cstddef>
#include <cstdint>

/*
 * NOTE: cf. https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/include/uapi/linux/can.h?h=v6.10.5
 */

using canid_t = uint32_t;

inline constexpr canid_t CAN_EFF_FLAG = 0x80000000;
inline constexpr canid_t CAN_RTR_FLAG = 0x40000000;
inline constexpr canid_t CAN_ERR_FLAG = 0x20000000;

inline constexpr canid_t CAN_SFF_MASK = 0x000007FF;
inline constexpr canid_t CAN_EFF_MASK = 0x1FFFFFFF;
inline constexpr canid_t CAN_ERR_MASK = 0x1FFFFFFF;

////// CAN 2.0 Frame /////////////////////////////////////////////////////////

inline constexpr std::size_t CAN_MAX_DLEN = 8;

inline constexpr std::size_t CAN_MTU = 16;

#pragma pack(push,1)

struct can_frame {
  canid_t can_id;
  uint8_t len;
  uint8_t __pad;
  uint8_t __res0;
  uint8_t len8_dlc;
  uint8_t data[CAN_MAX_DLEN];
};

#pragma pack(pop)

static_assert( sizeof(can_frame) == CAN_MTU );

////// CAN FD Frame //////////////////////////////////////////////////////////

inline constexpr uint8_t CANFD_BRS = 0x01;
inline constexpr uint8_t CANFD_ESI = 0x02;
inline constexpr uint8_t CANFD_FDF = 0x04;

inline constexpr std::size_t CANFD_MAX_DLEN = 64;

inline constexpr std::size_t CANFD_MTU = 72;

#pragma pack(push,1)

struct canfd_frame {
  canid_t can_id;
  uint8_t len;
  uint8_t flags;
  uint8_t __res0;
  uint8_t __res1;
  uint8_t data[CANFD_MAX_DLEN];
};

#pragma pack(pop)

static_assert( sizeof(canfd_frame) == CANFD_MTU );
