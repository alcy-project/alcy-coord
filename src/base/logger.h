#pragma once

#include "fpag/logging/sink/composite_sink.h"
#include "fpag/logging/sink/stdout_sink.h"
#include "fpag/logging/sync/sync_logger.h"

namespace base {

using Logger =
    logging::SyncLogger<logging::StdoutSink, logging::kDefaultLogLevel>;
extern Logger logger;

void init_logger(ColorStyle style);

}  // namespace base
