-- Copyright 2026 pugur
-- This source code is licensed under the Apache License, Version 2.0 with LLVM
-- Exceptions which can be found in the LICENSE file.

set_project("alcy_lang")
local project_version = "0.1.0"
set_version(project_version)

option("coverage")
set_default(false)
set_showmenu(true)
set_description("use llvm-cov for analyzing test coverage")
option_end()

option("xray")
set_default(false)
set_showmenu(true)
set_description("use llvm-xray for determining performance bottleneck")
option_end()

option("optreport")
set_default(false)
set_showmenu(true)
set_description("report optimization result")
option_end()

option("sanitizers")
set_default(false)
set_showmenu(true)
set_description("enable address/undefined behaviour/leak sanitizer")
option_end()

option("timetrace")
set_default(false)
set_showmenu(true)
set_description("generate timetrace json that can be see with perfetto ui")
option_end()

option("native")
set_default(false)
set_showmenu(true)
set_description("native architecture optimization")
option_end()

option("libunwind")
set_default(false)
set_showmenu(true)
set_description("use libunwind for stack tracing (for fpag)")
option_end()

option("fmtlib")
set_default(true)
set_showmenu(true)
set_description(
  "use fmtlib for formatting (use std::format if false) (for fpag)"
)
option_end()

option("unitybuild")
set_default(false)
set_showmenu(true)
set_description("enable unity build to shorten build time")
option_end()

option("lto")
set_default(false)
set_showmenu(true)
set_description("do link time optimization on release builds")
option_end()

option("tests")
set_default(false)
set_showmenu(true)
set_description("build unit tests")
option_end()

option("benchmarks")
set_default(false)
set_showmenu(true)
set_description("build micro benchmarks")
option_end()

option("stdlib")
set_default("libstdc++")
set_showmenu(true)
set_description("specify stl to use")
option_end()

set_policy("build.ccache", true)
set_policy("check.auto_ignore_flags", false)
set_policy("build.optimization.lto", has_config("lto"))
set_policy("build.c++.msvc.runtime", "MD")

add_rules("mode.debug", "mode.release", "mode.releasedbg")
add_rules("plugin.compile_commands.autoupdate", { outputdir = "build/" })

-- Helper functions
local function coverage(target)
  return has_config("coverage")
    and target:name() == "tests"
    and not is_plat("windows")
end

local function optreport()
  return has_config("optreport") and not is_mode("debug")
end

local function sanitizers()
  return has_config("sanitizers")
    and is_mode("debug")
    and not is_plat("windows")
end

local function xray()
  return has_config("xray") and not is_mode("release")
end

local function is_clang()
  return is_config("toolchain", "clang", "llvm")
    or (
      not is_config("toolchain", "gcc")
      and (is_plat("macosx", "iphoneos") or is_host("macosx"))
    )
end

local function is_gcc()
  return is_config("toolchain", "gcc")
    or (not is_config("toolchain", "clang", "llvm") and is_plat("linux"))
end

local function stdlib_config()
  if is_clang() and not is_plat("windows") and has_config("stdlib") then
    local std = get_config("stdlib")
    return { cxxflags = "-stdlib=" .. std, ldflags = "-stdlib=" .. std }
  end
  return {}
end

local subdirs = "src tests benchmarks"

local function source_files()
  local files = os.files("src/**.cc")
  table.join2(files, os.files("src/**.h"))

  if has_config("tests") then
    table.join2(files, os.files("tests/**.cc"))
    table.join2(files, os.files("tests/**.h"))
  end

  if has_config("benchmarks") then
    table.join2(files, os.files("benchmarks/**.cc"))
    table.join2(files, os.files("benchmarks/**.h"))
  end
  return files
end

if has_config("tests") then
  add_requires("catch2 v3.13.0", { system = false, configs = stdlib_config() })
end

if has_config("benchmarks") then
  add_requires("benchmark v1.9.5", {
    system = false,
    configs = table.join(stdlib_config(), {
      exceptions = false,
      cxflags = "-DBENCHMARK_USE_LIBCXX="
        .. (
          stdlib_config()
            and (has_config("stdlib") and get_config("stdlib") == "libc++")
            and "ON"
          or "OFF"
        ),
    }),
  })
end

add_requires(
  "fpag",
  {
    configs = {
      stdlib = get_config("stdlib"),
      fmtlib = get_config("fmtlib"),
      libunwind = get_config("libunwind"),
    },
  }
)

local llvm_configs = {
  shared = false,
  clang = true,
  lld = true,
  libunwind = true,
  libcxx = true,
  libcxxabi = true,
  assertions = is_mode("debug"),
  components = { "core", "irreader", "mc", "support", "native", "all-targets" },
}
table.join2(llvm_configs, stdlib_config())
add_requires("llvm 21.1.0", {
  system = false,
  configs = llvm_configs,
})

local alcy_modules = {
  "alcy.analyzer",
  "alcy.base",
  "alcy.codegen",
  "alcy.core",
  "alcy.ir",
  "alcy.lexer",
  "alcy.parser",
  "alcy.pipeline",
}

-- Tasks
task("format")
set_menu({ usage = "xmake format", description = "format source code" })
on_run(function()
  local files = source_files()
  if #files > 0 then
    os.runv(
      "clang-format",
      table.join({
        "--fail-on-incomplete-format",
        "--ferror-limit=1",
        "--sort-includes",
        "-i",
      }, files)
    )
  end
  os.run("uv sync")
  print(os.iorun("uv run scripts/header_license.py"):trim())
end)
task_end()

task("tidy")
set_menu({ usage = "xmake tidy", description = "Run clang-tidy --fix" })
on_run(function()
  local files = source_files()
  if #files > 0 then
    os.runv(
      "clang-tidy",
      table.join(
        { "--use-color", "--fix", "--config-file=./.clang-tidy" },
        files
      )
    )
  end
end)
task_end()

task("lint")
set_menu({
  usage = "xmake lint",
  description = "lint using cpplint & clang-format",
})
on_run(function()
  os.run("uv sync")
  print(os.iorun("uv run cpplint --recursive " .. subdirs):trim())
  local files = source_files()
  if #files > 0 then
    os.runv(
      "clang-format",
      table.join({ "--dry-run", "--fail-on-incomplete-format", "-i" }, files)
    )
  end
end)
task_end()

-- Events
after_build(function(target)
  if has_config("timetrace") then
    local trace_dir = path.join(os.projectdir(), "build/timetrace")
    os.mkdir(trace_dir)
    for _, objfile in ipairs(target:objectfiles()) do
      local json = objfile .. ".json"
      if os.exists(json) then
        os.cp(json, path.join(trace_dir, path.basename(json)))
      end
    end
  end

  if optreport() then
    local remark_dir = "build/remarks"
    os.mkdir(remark_dir)
    for _, yaml in
      ipairs(os.files(path.join(target:targetdir(), "**.opt.yaml")))
    do
      os.cp(yaml, remark_dir)
    end
  end
end)

after_run(function(target)
  if coverage(target) then
    local profraw = path.join(target:targetdir(), "default.profraw")
    local profdata = path.join(target:targetdir(), "default.profdata")
    local coverage_dir = "build/coverage"

    os.runv("llvm-profdata", { "merge", "-sparse", profraw, "-o", profdata })
    os.runv("llvm-cov", {
      "show",
      target:targetfile(),
      "-instr-profile=" .. profdata,
      "-format=html",
      "-output-dir=" .. coverage_dir,
    })

    cprint(
      "${green}coverage report generated at: "
        .. path.join(coverage_dir, "index.html")
    )
  end
end)

-- Rules
rule("alcy.common_config")
on_load(function(target)
  target:set("languages", "c++23", { public = true })
  target:set(
    "warnings",
    { "all", "extra", "error", "pedantic" },
    { public = true }
  )
  target:set("encodings", "source:utf-8", "utf-8")

  target:add("includedirs", "src", "third_party", { public = true })
  target:add(
    "defines",
    'ALCY_PROJECT_VERSION="' .. project_version .. '"',
    { public = true }
  )
  target:add(
    "defines",
    { "__STDC_CONSTANT_MACROS", "__STDC_FORMAT_MACROS" },
    { public = true }
  )

  target:set("exceptions", "none", { public = true })
  target:add("cxxflags", { "-fno-exceptions", "-fno-rtti" }, { public = true })

  if is_clang() or is_gcc() then
    target:add("cxxflags", {
      "-Wconversion",
      "-Wsign-conversion",
      "-Wnull-dereference",
      "-Wformat=2",
      "-Wundef",
    }, { public = true })
    target:add("cxxflags", "-fstack-protector-strong", { public = true })

    if is_mode("debug") and not is_plat("windows") then
      target:add("cxxflags", "-rdynamic", { public = true })
      target:add("ldflags", "-rdynamic", { public = true })
    end
  end

  if is_plat("linux") then
    if is_mode("debug") then
      target:add("ldflags", "-Wl,--build-id", { public = true })
    end
  end

  if is_mode("debug") then
    target:set("symbols", "debug", { public = true })
    target:set("optimize", "none", { public = true })
    target:add("cxxflags", "-fno-omit-frame-pointer", "-g3", { public = true })
    target:add(
      "defines",
      { "LLVM_ENABLE_STATS", "LLVM_ENABLE_DUMP" },
      { public = true }
    )
  elseif is_mode("release") then
    target:set("symbols", "hidden", { public = true })
    target:set("optimize", "fastest", { public = true })
    target:set("strip", "all", { public = true })
  end

  if stdlib_config() then
    local sl = get_config("stdlib")
    target:add("cxxflags", "-stdlib=" .. sl, { public = true })
    target:add("ldflags", "-stdlib=" .. sl, { public = true })
  end

  if sanitizers() then
    target:set("policy", "build.sanitizer.address", true)
    target:set("policy", "build.sanitizer.undefined", true)
    target:set("policy", "build.sanitizer.leak", true)
  end

  if xray() then
    target:add(
      "cxxflags",
      { "-fxray-instrument", "-fxray-instruction-threshold=200" },
      { public = true }
    )
    target:add("ldflags", "-fxray-instrument", { public = true })
  end

  if coverage(target) then
    target:add(
      "cxxflags",
      { "-fprofile-instr-generate", "-fcoverage-mapping" },
      { public = true }
    )
    target:add(
      "ldflags",
      { "-fprofile-instr-generate", "-fcoverage-mapping" },
      { public = true }
    )
  end

  if optreport() then
    target:add("cxxflags", "-fsave-optimization-record", { public = true })
  end

  if has_config("timetrace") then
    target:add("cxxflags", "-ftime-trace", { public = true })
  end

  if
    has_config("native")
    and not target:is_cross()
    and not is_mode("debug")
  then
    target:add("cxxflags", "-march=native", { public = true })
  end

  if has_config("unitybuild") then
    target:add("rules", "c++.unity_build", { batchsize = 12 })
  end
end)

rule("deps.llvm")
on_config(function(target)
  import("lib.detect.find_tool")
  local llvm_config = find_tool("llvm-config")
  if llvm_config then
    local includedir = os.iorunv(llvm_config.program, { "--includedir" }):trim()
    target:add("includedirs", includedir)
    local ldflags =
      os.iorunv(llvm_config.program, { "--libs", "core", "support" }):trim()
    target:add("ldflags", ldflags)
  end
end)

if is_plat("windows") then
  local llvm_root = os.getenv("LLVM_PATH")
    or find_path("include/llvm/Config/llvm-config.h", {
      "C:/Program Files/LLVM",
      "D:/LLVM",
      "$(env PATH)",
    })

  if llvm_root then
    target:add("includedirs", path.join(llvm_root, "include"))
    target:add("linkdirs", path.join(llvm_root, "lib"))
    target:add(
      "ldflags",
      "-lLLVMCore -lLLVMRemarks -lLLVMBitstreamReader -lLLVMBinaryFormat -lLLVMTargetParser -lLLVMSupport -lLLVMDemangle"
    )
  else
    raise("LLVM not found. Please set LLVM_PATH environment variable.")
  end
end
rule_end()

target("alcy.analyzer")
add_rules("alcy.common_config")
set_kind("object")
add_files("src/analyzer/**.cc")
set_default(false)
target_end()

target("alcy.base")
add_rules("alcy.common_config")
set_kind("object")
add_files("src/base/**.cc")
set_default(false)
target_end()

target("alcy.codegen")
add_rules("alcy.common_config")
set_kind("object")
add_files("src/codegen/**.cc")
add_rules("deps.llvm")
-- add_packages("llvm")
set_default(false)
target_end()

target("alcy.core")
add_rules("alcy.common_config")
set_kind("object")
add_files("src/core/**.cc")
set_default(false)
target_end()

target("alcy.ir")
add_rules("alcy.common_config")
set_kind("object")
add_files("src/ir/**.cc")
set_default(false)
target_end()

target("alcy.lexer")
add_rules("alcy.common_config")
set_kind("object")
add_files("src/lexer/**.cc")
set_default(false)
target_end()

target("alcy.parser")
add_rules("alcy.common_config")
set_kind("object")
add_files("src/parser/**.cc")
set_default(false)
target_end()

target("alcy.pipeline")
add_rules("alcy.common_config")
set_kind("object")
add_files("src/pipeline/**.cc")
set_default(false)
target_end()

target("alcy")
add_rules("alcy.common_config")
set_kind("binary")
add_files("src/app/**.cc")
add_deps(alcy_modules)
add_packages("fpag")
set_default(true)
target_end()

target("tests")
set_enabled(has_config("tests"))
add_rules("alcy.common_config", { public = false })
add_deps(alcy_modules)
set_kind("binary")
add_files("tests/**.cc")
add_packages("catch2")
add_includedirs("tests", { public = true })
-- catch2 uses c2y extension in their macro
if is_clang() then
  add_cxxflags("-Wno-c2y-extensions")
end
set_default(false)
target_end()

target("benchmarks")
set_enabled(has_config("benchmarks"))
add_rules("alcy.common_config", { public = false })
add_deps(alcy_modules)
set_kind("binary")
add_files("benchmarks/**.cc")
add_packages("benchmark")
add_includedirs("benchmarks", { public = true })
set_default(false)
target_end()
