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

#include <cstdio>
#include <cstdlib>

#include <algorithm>
#include <iterator>
#include <print>
#include <string>

#include <cs/Core/ByteArray.h>
#include <cs/IO/BufferedReader.h>
#include <cs/IO/File.h>
#include <cs/Logging/Logger.h>
#include <cs/Math/Numeric.h>
#include <cs/System/FileSystem.h>
#include <cs/System/PathFormatter.h>
#include <cs/System/Time.h>
#include <cs/Text/StringUtil.h>
#include <cs/Text/StringValue.h>
#include <cs/Text/TextIO.h>

#include "PCAP.h"
#include "SocketCAN.h"

namespace chr = std::chrono;
namespace  fs = std::filesystem;

using LineData = cs::ByteArray<CANFD_MAX_DLEN>;

struct LineInfo {
  LineInfo()
  {
    data.fill(0);
  }

  bool isValid() const
  {
    return !device.empty()  &&  time.isValid();
  }

  bool isLen8Dlc() const
  {
    return !is_canfd  &&  len == 8  &&  len8_dlc > 8;
  }

  LineData    data;
  std::string device;
  uint8_t     fdflags{0};
  canid_t     id{0};
  bool        is_canfd{false};
  bool        is_ext{false};
  bool        is_rtr{false};
  uint8_t     len{0};
  uint8_t     len8_dlc{0};
  cs::TimeVal time{-1};
};

using LineInfos = std::list<LineInfo>;

namespace parser {

  using ConstStringIter = std::string::const_iterator;

  constexpr auto INVALID_HEXCHAR = std::numeric_limits<cs::byte_t>::max();

  constexpr auto lambda_is_space()
  {
    return [](const char ch) -> bool {
      return ch == ' ';
    };
  }

  bool parseData(LineInfo& result, ConstStringIter& first, const ConstStringIter& last,
                 const cs::LoggerPtr& logger, const std::size_t lineno)
  {
    constexpr std::size_t TWO = 2;

    result.data.fill(0);
    result.len = 0;

    std::size_t count = 0;
    for(; first != last; ++count, ++first) {
      const bool is_even = cs::isEven(count);

      if( !cs::isHexDigit(*first) ) {
        if( is_even ) {
          return true;
        } else {
          logger->logError(lineno, u8"Incomplete data!");
          return false;
        }

      } else {
        const std::size_t idxData = count/TWO;
        if( idxData >= result.data.size() ) {
          logger->logError(lineno, u8"Data buffer exceeded!");
          return false;
        }

        result.data[idxData] |= cs::fromHexChar(*first);
        if( is_even ) {
          result.data[idxData] <<= 4;
        } else {
          result.len++;
        }
      }
    } // For each character

    if( cs::isOdd(count) ) {
      logger->logError(lineno, u8"Incomplete data!");
      return false;
    }

    return true;
  }

  bool parseDevice(std::string& result, ConstStringIter& first, const ConstStringIter& last,
                   const cs::LoggerPtr& logger, const std::size_t lineno)
  {
    result.clear();

    const ConstStringIter begDev = std::find_if_not(first, last, lambda_is_space());
    if( begDev == last ) {
      logger->logError(lineno, u8"Missing device declaration!");
      return false;
    }

    const ConstStringIter endDev = std::find(begDev, last, ' ');
    if( endDev == last ) {
      logger->logError(lineno, u8"Invalid device separator!");
      return false;
    }

    first = endDev;

    result.assign(begDev, endDev);

    return true;
  }

  bool parseId(LineInfo& result, ConstStringIter& first, const ConstStringIter& last,
               const cs::LoggerPtr& logger, const std::size_t lineno)
  {
    constexpr ConstStringIter::difference_type THREE = 3;

    result.id = 0;
    result.is_ext = false;

    const ConstStringIter begId = std::find_if_not(first, last, lambda_is_space());
    if( begId == last ) {
      logger->logError(lineno, u8"Missing message ID!");
      return false;
    }

    const ConstStringIter endId = std::find(begId, last, '#');
    if( endId == last ) {
      logger->logError(lineno, u8"Invalid ID separator!");
      return false;
    }

    const std::string_view idStr(begId, endId);
    const auto expVal = cs::toValue<canid_t>(idStr, 16);
    result.id = expVal.value_or(0);
    if( !expVal.has_value() ) {
      logger->logError(lineno, u8"Invalid ID string \"{}\"!", idStr);
      return false;
    }

    result.is_ext = std::distance(begId, endId) > THREE  ||  result.id > CAN_SFF_MASK;

    first = endId;
    ++first; // NOTE: consider '#' part of the ID

    return true;
  }

  bool parseRawDLC(uint8_t& result, ConstStringIter& first, const ConstStringIter& last,
                   const cs::LoggerPtr& logger, const std::size_t lineno)
  {
    result = 0;

    if( first == last  ||  *first != '_' ) {
      return true;
    }

    ++first; // Skip '_'

    result = cs::fromHexChar(*first);
    if( result == INVALID_HEXCHAR ) {
      logger->logError(lineno, u8"Invalid raw DLC \"{}\"!", *first);
      return false;
    }

    ++first;

    return true;
  }

  std::expected<cs::TimeVal,std::errc> parseTime(const std::string_view& str)
  {
    namespace chr = std::chrono;

    using      seconds_t = chr::seconds::rep;
    using microseconds_t = chr::microseconds::rep;

    using size_type = std::string_view::size_type;

    constexpr size_type NPOS = std::string_view::npos;
    constexpr size_type  ONE = 1;

    const size_type idxDot = str.find('.');
    if( idxDot == NPOS ) {
      return std::unexpected(std::errc::invalid_argument);
    }

    const auto expSecs = cs::toValue<seconds_t>(str.substr(0, idxDot));
    if( !expSecs.has_value() ) {
      return std::unexpected(expSecs.error());
    }

    const auto expUSecs = cs::toValue<microseconds_t>(str.substr(idxDot + ONE));
    if( !expUSecs.has_value() ) {
      return std::unexpected(expUSecs.error());
    }

    return cs::TimeVal(chr::seconds{expSecs.value()},
                       chr::microseconds{expUSecs.value()});
  }

  bool parseTime(cs::TimeVal& result, ConstStringIter& first, const ConstStringIter& last,
                 const cs::LoggerPtr& logger, const std::size_t lineno)
  {
    result = cs::TimeVal{-1};

    if( *first != '(' ) {
      logger->logError(lineno, u8"Missing time stamp!");
      return false;
    }

    ++first; // parse '('

    const ConstStringIter endTim = std::find(first, last, ')');
    if( endTim == last ) {
      logger->logError(lineno, u8"Incomplete time stamp!");
      return false;
    }

    const std::string_view timeStr(first, endTim);
    result = parseTime(timeStr).value_or(cs::TimeVal(-1));
    if( !result.isValid() ) {
      logger->logError(lineno, u8"Invalid time stamp \"{}\"!", timeStr);
      return false;
    }

    first = endTim;
    ++first; // parse ')'

    return true;
  }

  bool parseType(LineInfo& result, ConstStringIter& first, const ConstStringIter& last,
                 const cs::LoggerPtr& logger, const std::size_t lineno)
  {
    result.fdflags = 0;
    result.is_canfd = false;
    result.is_rtr = false;
    result.len = 0;

    // (1) Message Type //////////////////////////////////////////////////////

    // NOTE: No message type for empty CAN 2.0 messages!
    if( first == last ) {
      return true;
    }

    if(        *first == '#' ) {
      result.is_canfd = true;
      ++first;

    } else if( *first == 'R' ) {
      result.is_rtr = true;
      ++first;

    } else if( cs::isHexDigit(*first) ) {
      return true;

    } else {
      logger->logError(lineno, u8"Invalid message type \"{}\"!", *first);
      return false;

    }

    // (2) Message Extra /////////////////////////////////////////////////////

    if( first == last ) {
      if( result.is_rtr ) {
        return true;
      } else {
        logger->logError(lineno, u8"Missing message extra!");
        return false;
      }
    }

    const uint8_t extra = cs::fromHexChar(*first);
    if( extra == INVALID_HEXCHAR ) {
      logger->logError(lineno, u8"Invalid message extra \"{}\"!", *first);
      return false;
    }

    if(        result.is_canfd ) {
      result.fdflags = extra;
    } else if( result.is_rtr ) {
      result.len = extra;
    }

    ++first;

    return true;
  }

  LineInfo parseLine(ConstStringIter first, const ConstStringIter& last,
                     const cs::LoggerPtr& logger, const std::size_t lineno)
  {
    LineInfo info;

    // (0) Sanity Check ////////////////////////////////////////////////////////

    if( first == last ) {
      logger->logWarning(lineno, u8"Ignoring empty line!");
      return LineInfo();
    }

    if( *first != '(' ) {
      logger->logWarning(lineno, u8"Ignoring line with invalid start sequence \"{}\"!", *first);
      return LineInfo();
    }

    // (1) Time Stamp //////////////////////////////////////////////////////////

    if( !parseTime(info.time, first, last, logger, lineno) ) {
      return LineInfo();
    }

    // (2) Device //////////////////////////////////////////////////////////////

    if( !parseDevice(info.device, first, last, logger, lineno) ) {
      return LineInfo();
    }

    // (3) Message ID ////////////////////////////////////////////////////////

    if( !parseId(info, first, last, logger, lineno) ) {
      return LineInfo();
    }

    // (4) Message Type: CAN 2.0, RTR, CAN FD ////////////////////////////////

    if( !parseType(info, first, last, logger, lineno) ) {
      return LineInfo();
    }

    // (5) Parse Data ////////////////////////////////////////////////////////

    if( !info.is_rtr  &&  !parseData(info, first, last, logger, lineno) ) {
      return LineInfo();
    }

    // (6) Parse Raw DLC /////////////////////////////////////////////////////

    if( !parseRawDLC(info.len8_dlc, first, last, logger, lineno) ) {
      return LineInfo();
    }

    return info;
  }

  LineInfo parseLine(const std::string& line,
                     const cs::LoggerPtr& logger, const std::size_t lineno)
  {
    return parseLine(line.begin(), line.end(), logger, lineno);
  }

} // namespace parser

namespace writer {

  bool writeHeader(const cs::File& file)
  {
    constexpr auto SIZE_HEADER = sizeof(pcap_hdr);

    pcap_hdr header;
    memset(&header, 0, SIZE_HEADER);

    /*
     * NOTE: cf. to the following link for the value of '.snaplen':
     *
     * https://www.wireshark.org/docs/wsug_html_chunked/AppToolstcpdump.html
     */

    header.magic_number  = MAGIC_NUMBER;
    header.version_major = VERSION_MAJOR;
    header.version_minor = VERSION_MINOR;
    header.snaplen       = 65535;
    header.network       = LINKTYPE_CAN_SOCKETCAN;

    return file.write(&header, SIZE_HEADER) == SIZE_HEADER;
  }

  bool write(const cs::File& file, const LineInfo& info)
  {
    constexpr auto SIZE_HEADER = sizeof(pcaprec_hdr);

    pcaprec_hdr header;
    memset(&header, 0, SIZE_HEADER);

    header.ts_sec   = info.time.secs().count();
    header.ts_usec  = info.time.usecs().count();
    header.incl_len = CAN_MTU;
    header.orig_len = CAN_MTU;

    if( file.write(&header, SIZE_HEADER) != SIZE_HEADER ) {
      return false;
    }

    can_frame frame;
    memset(&frame, 0, CAN_MTU);

    frame.can_id = info.id;
    frame.len    = info.len;

    if( info.is_ext ) {
      frame.can_id |= CAN_EFF_FLAG;
    }

    if( info.is_rtr ) {
      frame.can_id |= CAN_RTR_FLAG;
    }

    if( info.isLen8Dlc() ) {
      frame.len8_dlc = info.len8_dlc;
    }

    if( !info.is_rtr ) {
      for(uint8_t i = 0; i < frame.len; i++) {
        frame.data[i] = info.data[i];
      }
    }

    return file.write(&frame, CAN_MTU) == CAN_MTU;
  }

  bool writeFD(const cs::File& file, const LineInfo& info)
  {
    constexpr auto SIZE_HEADER = sizeof(pcaprec_hdr);

    pcaprec_hdr header;
    memset(&header, 0, SIZE_HEADER);

    header.ts_sec   = info.time.secs().count();
    header.ts_usec  = info.time.usecs().count();
    header.incl_len = CANFD_MTU;
    header.orig_len = CANFD_MTU;

    if( file.write(&header, SIZE_HEADER) != SIZE_HEADER ) {
      return false;
    }

    canfd_frame frame;
    memset(&frame, 0, CANFD_MTU);

    frame.can_id = info.id;
    frame.len    = info.len;
    frame.flags  = info.fdflags;

    if( info.is_ext ) {
      frame.can_id |= CAN_EFF_FLAG;
    }

    for(uint8_t i = 0; i < frame.len; i++) {
      frame.data[i] = info.data[i];
    }

    return file.write(&frame, CANFD_MTU) == CANFD_MTU;
  }

  void write(const fs::path& output, const LineInfos& infos, const std::string& device,
             const cs::LoggerPtr& logger)
  {
    const cs::File::OpenFlags flags = cs::FileOpenFlag::Write | cs::FileOpenFlag::Truncate;
    cs::File file;
    if( !file.open(output, flags) ) {
      logger->logError(u8"Unable to open file \"{}\"!", output);
      return;
    }

    writeHeader(file); // TODO

    for(const LineInfo& info : infos) {
      if( info.device != device ) {
        continue;
      }

      if( info.is_canfd ) {
        writeFD(file, info); // TODO
      } else {
        write(file, info); // TODO
      }
    }

    file.close();
  }

} // namespace writer

inline fs::path replaceExtension(fs::path p, const fs::path& ext)
{
  p.replace_extension(ext);
  return p;
}

void print(const LineInfo& info)
{  
  std::print("({}.{}) ", info.time.secs().count(), info.time.usecs().count());

  std::print("{} ", info.device);

  if( info.is_ext ) {
    std::print("{:08X}#", info.id);
  } else {
    std::print("{:X}#", info.id);
  }

  if( info.is_canfd ) {
    std::print("#{:X}", info.fdflags);
  }

  if( info.is_rtr ) {
    std::print("R{:X}", info.len);
  } else {
    for(uint8_t i = 0; i < info.len; i++) {
      std::print("{:02X}", info.data[i]);
    }
  }

  if( info.isLen8Dlc() ) {
    std::print("_{:X}", info.len8_dlc);
  }

  std::println("");
}

void printno(const std::size_t lineno, const LineInfo& info)
{
  std::print("{}: ", lineno);
  print(info);
}

int main(int /*argc*/, char **argv)
{
  cs::LoggerPtr logger = cs::Logger::make();

  const fs::path input = "./testdata/candump-2024-09-01_173152.log";
  if( !cs::isFile(input) ) {
    logger->logError(u8"Input \"{}\" not found!", input);
    return EXIT_FAILURE;
  }

  const chr::system_clock::time_point inputTime = cs::lastWriteTime(input);
  const cs::TimeVal inputTimeVal(inputTime);
  std::println("{}.{} {}", inputTimeVal.secs(), inputTimeVal.usecs(), inputTimeVal.value());

  const fs::path output = replaceExtension(input, "pcap");
  std::println("{} -> {}", input, output);

  const std::list<std::string> lines = cs::readLines(input);
  if( lines.empty() ) {
    logger->logError(u8"Unable to read input \"{}\"!", input);
    return EXIT_FAILURE;
  }

  LineInfos infos;

  std::size_t lineno = 0;
  for(const std::string& line : lines) {
    lineno += 1;

    const LineInfo info = parser::parseLine(line, logger, lineno);
    if( !info.isValid() ) {
      continue;
    }

    infos.push_back(info);

#if 0
    if( lineno <= 40 ) {
      printno(lineno, info);
    }
#endif
  } // For Each Line

#if 1
  std::for_each(infos.cbegin(), infos.cend(), print);
#else
  std::for_each(infos.cbegin(), std::next(infos.cbegin(), 40), print);
#endif

  writer::write(output, infos, "vcan0", logger);

  return EXIT_SUCCESS;
}
