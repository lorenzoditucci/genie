#ifndef SPRING_GENERATE_READ_STREAMS_ERU_H_
#define SPRING_GENERATE_READ_STREAMS_ERU_H_

#include <string>
#include <vector>
#include <map>
#include "spring/util.h"

namespace spring {


std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>> generate_read_streams(const std::string &temp_dir,
                                                                  const compression_params &cp);
}  // namespace spring

#endif  // SPRING_GENERATE_READ_STREAMS_ERU_H_