#include "base/logger.h"

#include "fpag/mem/page_allocator.h"

namespace base {

Logger logger;

void init_logger(ColorStyle style) {
  logging::StdoutSink sink(nullptr, 0, style, false);
  logger.init(std::move(sink));
}

}  // namespace base
