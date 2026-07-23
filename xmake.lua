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

option("llvm")
set_default(true)
set_showmenu(true)
set_description("use LLVM for backend codegen")
option_end()

option("cache-llvm")
set_default(true)
set_showmenu(true)
set_description(
  "use preinstalled LLVM in third_party/llvm-project/.alcy/install"
)
option_end()

option("download-llvm")
set_default(true)
set_showmenu(true)
set_description("download prebuilt LLVM from GitHub releases if available")
option_end()

option("build-llvm")
set_default(true)
set_showmenu(true)
set_description("build LLVM if no caches or downloads available")
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

includes("src/build/xmake/fpag.lua")
includes("src/build/xmake/llvm.lua")

set_policy("build.ccache", true)
set_policy("check.auto_ignore_flags", false)
set_policy("build.optimization.lto", has_config("lto"))
set_policy("build.c++.msvc.runtime", "MD")
set_policy("package.include_external_headers", true)
set_policy("package.inherit_external_configs", true)
set_policy("package.install_only", true)
-- set_policy("diagnosis.check_build_deps", true)

-- add_rules("mode.debug", "mode.release", "mode.releasedbg")
add_rules("plugin.compile_commands.autoupdate")

local function coverage(target)
  return has_config("coverage")
    and target:name() == "tests"
    and not is_plat("windows")
end

local function optreport()
  return has_config("optreport") and not is_mode("debug")
end

local function sanitizers()
  return has_config("sanitizers") and is_mode("debug") and is_plat("linux")
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

local function package_common_config()
  if is_clang() and not is_plat("windows") and has_config("stdlib") then
    local std = get_config("stdlib")
    return {
      cxxflags = "-stdlib=" .. std,
      ldflags = "-stdlib=" .. std,
      -- for releasedbg
      debug = not is_mode("release"),
    }
  end
  return {}
end

local function is_libcxx()
  return is_clang()
    and not is_plat("windows")
    and has_config("stdlib")
    and get_config("stdlib") == "libc++"
end

local function source_files()
  local files = {}
  table.join2(files, os.files("src/**.cc"))
  table.join2(files, os.files("src/**.h"))
  table.join2(files, os.files("tests/**.cc"))
  table.join2(files, os.files("tests/**.h"))
  table.join2(files, os.files("benchmarks/**.cc"))
  table.join2(files, os.files("benchmarks/**.h"))

  -- table.join2(files, os.files("src/**.cc|codegen_llvm/**"))
  -- table.join2(files, os.files("src/**.h|codegen_llvm/**"))
  -- if has_config("llvm") then
  --   table.join2(files, os.files("src/codegen_llvm/**.cc"))
  --   table.join2(files, os.files("src/codegen_llvm/**.h"))
  -- end

  -- if has_config("tests") then
  --   table.join2(files, os.files("tests/**.cc|codegen_llvm/**"))
  --   table.join2(files, os.files("tests/**.h|codegen_llvm/**"))
  --   if has_config("llvm") then
  --     table.join2(files, os.files("tests/codegen_llvm/**.cc"))
  --     table.join2(files, os.files("tests/codegen_llvm/**.h"))
  --   end
  -- end

  -- if has_config("benchmarks") then
  --   table.join2(files, os.files("benchmarks/**.cc|codegen_llvm/**"))
  --   table.join2(files, os.files("benchmarks/**.h|codegen_llvm/**"))
  --   if has_config("llvm") then
  --     table.join2(files, os.files("benchmarks/codegen_llvm/**.cc"))
  --     table.join2(files, os.files("benchmarks/codegen_llvm/**.h"))
  --   end
  -- end
  return files
end

local alcy_component_kind = "static" -- object, static, or shared

-- Dependencies
add_requires("fpag v0.1.0", {
  system = false,
  external = true,
  configs = {
    stdlib = get_config("stdlib"),
    libunwind = get_config("libunwind"),
  },
})

if has_config("llvm") then
  add_requires("alcy_llvm", {
    system = false,
    external = true,
    configs = table.join(package_common_config(), {
      download_llvm = has_config("download-llvm"),
      cache_llvm = has_config("cache-llvm"),
      build_llvm = has_config("build-llvm"),
    }),
  })
end

if has_config("tests") then
  add_requires("catch2 v3.13.0", {
    system = false,
    external = true,
    configs = package_common_config(),
  })
end

add_requires("fmt 12.1.0", {
  system = false,
  external = false,
  configs = package_common_config(),
})

if has_config("benchmarks") then
  add_requires("benchmark v1.9.5", {
    system = false,
    external = true,
    configs = table.join(package_common_config(), {
      exceptions = false,
      cxflags = "-DBENCHMARK_USE_LIBCXX=" .. (is_libcxx() and "ON" or "OFF"),
    }),
  })
end

local alcy_modules = {
  ["alcy_analyzer"] = true,
  ["alcy_base"] = true,
  ["alcy_codegen"] = true,
  ["alcy_codegen_llvm"] = has_config("llvm"),
  ["alcy_core"] = true,
  ["alcy_ir"] = true,
  ["alcy_lexer"] = true,
  ["alcy_parser"] = true,
  ["alcy_pipeline"] = true,
}

local subdirs = { "src", "tests", "benchmarks" }

-- Tasks
task("format")
set_menu({ usage = "xmake format", description = "format source code" })
on_run(function()
  local files = source_files()
  if #files > 0 then
    os.execv(
      "clang-format",
      table.join({
        "--fail-on-incomplete-format",
        "--ferror-limit=1",
        "--sort-includes",
        "-i",
      }, files)
    )
  end
  os.exec("uv sync")
  os.exec("uv run scripts/header_license.py")
end)
task_end()

task("tidy")
set_menu({ usage = "xmake tidy", description = "Run clang-tidy --fix" })
on_run(function()
  local files = source_files()
  if #files > 0 then
    os.execv(
      "clang-tidy",
      table.join(
        { "--use-color", "--fix", "--config-file=./.clang-tidy", "-p=." },
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
  os.exec("uv sync")
  os.execv("uv", table.join({ "run", "cpplint", "--recursive" }, subdirs))
  local files = source_files()
  if #files > 0 then
    os.execv(
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
    local coverage_dir = path.join("build", "coverage", target:name())

    local profraw = path.join(target:targetdir(), "default.profraw")
    local profdata = path.join(target:targetdir(), "default.profdata")

    os.execv("llvm-profdata", { "merge", "-sparse", profraw, "-o", profdata })
    os.execv(
      "llvm-cov",
      table.join({
        "show",
        target:targetfile(),
        "-instr-profile=" .. profdata,
        "-format=html",
        "-output-dir=" .. coverage_dir,
      }, subdirs)
    )

    cprint(
      "${green}coverage report generated at: "
        .. path.join(coverage_dir, "index.html")
    )
  end
end)

-- Rules
rule("alcy_common_config")
on_load(function(target)
  target:set("languages", "c++20")
  target:set("warnings", { "all", "extra", "error", "pedantic" })
  target:set("encodings", "source:utf-8", "utf-8")

  target:add("includedirs", "src", "third_party")
  target:add("defines", 'ALCY_PROJECT_VERSION="' .. project_version .. '"')
  target:add(
    "defines",
    { "__STDC_CONSTANT_MACROS", "__STDC_FORMAT_MACROS", "__STDC_LIMIT_MACROS" }
  )
  target:add("defines", {
    "LLVM_ENABLE_ABI_BREAKING_CHECKS=" .. (is_mode("debug") and "1" or "0"),
    "LLVM_ENABLE_ASSERTIONS=" .. (is_mode("debug") and "1" or "0"),
  }, { force = true })

  target:add("packages", "fpag")
  -- target:add("packages", "fmt")

  target:set("exceptions", "none")
  target:add("cxxflags", { "-fno-exceptions", "-fno-rtti" })

  if is_clang() or is_gcc() then
    target:add("cxxflags", {
      "-Wconversion",
      "-Wsign-conversion",
      "-Wnull-dereference",
      "-Wformat=2",
      "-Wundef",
      "-Wnon-virtual-dtor",
      "-Woverloaded-virtual",
    })
    target:add("cxxflags", "-fstack-protector-strong")

    if is_mode("debug") and not is_plat("windows") then
      target:add("cxxflags", "-rdynamic")
      target:add("ldflags", "-rdynamic")
    end
  end

  -- some libraries use c2y extension in their macro
  if is_clang() then
    target:add("cxxflags", "-Wno-c2y-extensions")
  end

  if is_plat("linux") then
    if is_mode("debug") then
      target:add("ldflags", "-Wl,--build-id")
    end
  elseif is_plat("windows") then
    target:add("syslinks", { "ntdll", "advapi32", "user32", "shell32" })
  end

  if not is_mode("release") then
    target:set("symbols", "debug")
    target:set("optimize", "none")
    target:add("cxxflags", { "-fno-omit-frame-pointer", "-g3" })
    target:add("defines", {
      "LLVM_ENABLE_STATS",
      "LLVM_ENABLE_DUMP",
    })
  else
    target:set("symbols", "hidden")
    target:set("optimize", "fastest")
    target:set("strip", "all")
  end

  if is_clang() and has_config("stdlib") then
    local sl = get_config("stdlib")
    target:add("cxxflags", "-stdlib=" .. sl)
    target:add("ldflags", "-stdlib=" .. sl)
  end

  if sanitizers() then
    target:set("policy", "build.sanitizer.address", true)
    target:set("policy", "build.sanitizer.undefined", true)
    target:set("policy", "build.sanitizer.leak", true)
  end

  if xray() then
    target:add(
      "cxxflags",
      { "-fxray-instrument", "-fxray-instruction-threshold=200" }
    )
    target:add("ldflags", "-fxray-instrument")
  end

  if coverage(target) then
    target:add("cxxflags", { "-fprofile-instr-generate", "-fcoverage-mapping" })
    target:add("ldflags", { "-fprofile-instr-generate", "-fcoverage-mapping" })
  end

  if optreport() then
    target:add("cxxflags", "-fsave-optimization-record")
  end

  if has_config("timetrace") then
    target:add("cxxflags", "-ftime-trace")
  end

  if
    has_config("native")
    and not target:is_cross()
    and not is_mode("debug")
  then
    target:add("cxxflags", "-march=native")
  end

  if has_config("unitybuild") then
    target:add("rules", "c++.unity_build", { batchsize = 12 })
  end
end)
rule_end()

target("alcy_analyzer")
set_enabled(alcy_modules["alcy_analyzer"])
add_rules("alcy_common_config")
set_kind(alcy_component_kind)
add_files("src/analyzer/**.cc")
set_default(false)
target_end()

target("alcy_base")
set_enabled(alcy_modules["alcy_base"])
add_rules("alcy_common_config")
set_kind(alcy_component_kind)
add_files("src/base/**.cc")
set_default(false)
target_end()

target("alcy_codegen")
set_enabled(alcy_modules["alcy_codegen"])
add_rules("alcy_common_config")
set_kind(alcy_component_kind)
add_files("src/codegen/**.cc")
set_default(false)
target_end()

target("alcy_codegen_llvm")
set_enabled(alcy_modules["alcy_codegen_llvm"])
add_rules("alcy_common_config")
add_packages("alcy_llvm")
set_kind(alcy_component_kind)
add_files("src/codegen_llvm/**.cc")
set_default(false)
target_end()

target("alcy_core")
set_enabled(alcy_modules["alcy_core"])
add_rules("alcy_common_config")
set_kind(alcy_component_kind)
add_files("src/core/**.cc")
set_default(false)
target_end()

target("alcy_ir")
set_enabled(alcy_modules["alcy_ir"])
add_rules("alcy_common_config")
set_kind(alcy_component_kind)
add_files("src/ir/**.cc")
set_default(false)
target_end()

target("alcy_lexer")
set_enabled(alcy_modules["alcy_lexer"])
add_rules("alcy_common_config")
set_kind(alcy_component_kind)
add_files("src/lexer/**.cc")
set_default(false)
target_end()

target("alcy_parser")
set_enabled(alcy_modules["alcy_parser"])
add_rules("alcy_common_config")
set_kind(alcy_component_kind)
add_files("src/parser/**.cc")
set_default(false)
target_end()

target("alcy_pipeline")
set_enabled(alcy_modules["alcy_pipeline"])
add_rules("alcy_common_config")
set_kind(alcy_component_kind)
add_files("src/pipeline/**.cc")
set_default(false)
target_end()

target("alcy")
add_rules("alcy_common_config")
set_kind("binary")
add_files("src/app/**.cc")
if has_config("llvm") then
  add_packages("alcy_llvm")
end
for m, e in pairs(alcy_modules) do
  if e then
    add_deps(m)
  end
end
set_default(true)
target_end()

target("tests")
set_enabled(has_config("tests"))
add_rules("alcy_common_config")
add_files("tests/**.cc|codegen_llvm/**.cc")
if has_config("llvm") then
  add_packages("alcy_llvm")
  add_files("tests/codegen_llvm/**.cc")
end
for m, e in pairs(alcy_modules) do
  if e then
    add_deps(m)
  end
end
set_kind("binary")
add_packages("catch2")
add_includedirs("tests", { public = true })
set_default(false)
target_end()

target("benchmarks")
set_enabled(has_config("benchmarks"))
add_rules("alcy_common_config")
if has_config("llvm") then
  add_packages("alcy_llvm")
  add_files("benchmarks/codegen_llvm/**.cc")
end
for m, e in pairs(alcy_modules) do
  if e then
    add_deps(m)
  end
end
set_kind("binary")
add_files("benchmarks/**.cc")
add_packages("benchmark")
add_includedirs("benchmarks", { public = true })
set_default(false)
target_end()

target("all")
set_kind("phony")
add_deps("alcy", "tests", "benchmarks")
set_default(false)
