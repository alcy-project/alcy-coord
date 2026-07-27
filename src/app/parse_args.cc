// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#include "app/parse_args.h"

#include <string>
#include <utility>
#include <vector>

#include "app/driver_config.h"
#include "base/debug/fatal.h"
#include "base/logger.h"
#include "fpag/arg/macro.h"

namespace app {

namespace {

DriverConfig extract_from_matches(arg::Matches&& matches) {
  DriverConfig c{};
  c.time_trace = matches.get<bool>("time-trace").unwrap_or(c.time_trace);

  return c;
}

}  // namespace

arg::Parser build_parser() {
  arg::CommandBuilder builder(ALCY_PROJECT_NAME, ALCY_PROJECT_VERSION);
  builder.about(ALCY_COMMAND_ABOUT);
  builder.builtin_enabled(true);
  builder.add_arg(arg::ArgBuilder("color")
                      .help("Color mode for logging")
                      .default_value("auto")
                      .choices({"auto", "always", "never"})
                      .build());
  builder.add_arg(arg::ArgBuilder("time-trace")
                      .short_name('t')
                      .help("Enable time profiling and generate the json file.")
                      .is_flag(true)
                      .build());
  return arg::Parser(std::move(builder).build());
}

ParseArgsResult parse_args(arg::Parser&& parser,
                           i32 argc,
                           const char* const* argv,
                           base::ColorStyle style) {
  const std::string_view name = parser.root_command().name();
  arg::ParseResult<arg::Matches> result = parser.try_parse(argc, argv);

  switch (result.status()) {
    case arg::ParseStatus::Success: {
      return extract_from_matches(std::move(result).unwrap());
    }
    case arg::ParseStatus::Error: {
      std::vector<arg::ParseError>&& errors = std::move(result).unwrap_err();
      const arg::DefaultErrorFormatter f;
      base::logger.info("\n{}", f(name, errors, style));
      return ParseInterruptedReason::ParseError;
    }
    case arg::ParseStatus::HelpRequested: {
      std::string&& help = std::move(result).unwrap_help();
      base::logger.info("\n{}", std::move(help));
      return ParseInterruptedReason::HelpRequested;
    }
    case arg::ParseStatus::VersionRequested: {
      std::string&& version = std::move(result).unwrap_version();
      const arg::DefaultVersionFormatter f;
      base::logger.info("\n{}", f(name, std::move(version), style));
      return ParseInterruptedReason::VersionRequested;
    }
    default: UNREACHABLE();
  }
}

}  // namespace app
