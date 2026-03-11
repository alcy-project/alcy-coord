// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#include "base/log_prefix.h"

#include <cstddef>
#include <string_view>

#include "base/console.h"
#include "base/style.h"

namespace base {

namespace {

constexpr std::string_view kPlainPrefixes[] = {
    "debug: ",  // Debug
    " info: ",  // Info
    " warn: ",  // Warn
    "error: ",  // Error
    "fatal: ",  // Fatal
};

struct PrefixStorage {
  // 32 bytes is enough for any ansi prefix; verified at compile time below
  static constexpr usize kCapacity = 32;
  char buf[kCapacity]{};
  std::string_view view;
};

// Build into a pre-allocated prefixstorage
constexpr void build_ansi_prefix(PrefixStorage* out,
                                 std::string_view bold,
                                 std::string_view color,
                                 std::string_view reset,
                                 std::string_view label,
                                 std::string_view suffix) {
  char* p = out->buf;
  const auto append = [&p](std::string_view s) {
    for (char c : s) {
      *p++ = c;
    }
  };
  append(bold);
  append(color);
  append(label);
  append(reset);
  append(suffix);
  out->view = std::string_view{out->buf, static_cast<usize>(p - out->buf)};
}

struct PrefixTable {
  PrefixStorage ansi[static_cast<usize>(LogLevel::MaxValue) + 1];
  bool use_ansi{false};

  void init() {
    use_ansi = can_use_ansi_escape_sequence(Stream::Stderr);
    if (!use_ansi) {
      return;
    }

    build_ansi_prefix(&ansi[0], kBold, kBrightCyan, kReset, "debug", ": ");
    build_ansi_prefix(&ansi[1], kBold, kBrightGreen, kReset, " info", ": ");
    build_ansi_prefix(&ansi[2], kBold, kBrightYellow, kReset, " warn", ": ");
    build_ansi_prefix(&ansi[3], kBold, kBrightRed, kReset, "error", ": ");
    build_ansi_prefix(&ansi[4], kBold, kBrightMagenta, kReset, "fatal", ": ");
  }
};

const PrefixTable& table() {
  static PrefixTable table = [] {
    PrefixTable t;
    t.init();
    return t;
  }();
  return table;
}

}  // namespace

std::string_view log_prefix(LogLevel level) {
  const auto idx = static_cast<usize>(level);
  const PrefixTable& t = table();
  return t.use_ansi ? t.ansi[idx].view : kPlainPrefixes[idx];
}

}  // namespace base
