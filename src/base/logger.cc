#include "base/logger.h"

#include "fpag/mem/page_allocator.h"

namespace base {

Logger logger;

void init_logger(ColorStyle style) {
  logging::StdoutSink sink(
      static_cast<char*>(mem::allocate_pages(mem::kPageSize)), mem::kPageSize,
      style, true);
  logger.init(std::move(sink));
}

}  // namespace base
