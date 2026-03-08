-- Copyright 2026 pugur
-- This source code is licensed under the Apache License, Version 2.0 with LLVM
-- Exceptions which can be found in the LICENSE file.

set_project("alcy_lang")
local project_version = "0.1.0"
set_version(project_version)

set_policy("build.ccache", true)
set_policy("check.auto_ignore_flags", false)

option("coverage", {default = false, description = "use llvm-cov for analyzing test coverage"})
option("xray", {default = false, description = "use llvm-xray for determining performance bottleneck"})
option("optreport", {default = false, description = "report optimization result"})
option("sanitizers", {default = false, description = "enable address/undefined behaviour/leak sanitizer"})
option("timetrace", {default = false, description = "generate timetrace json that can be see with perfetto ui"})
option("native", {default = false, description = "native architecture optimization"})
option("unitybuild", {default = false, description = "enalbe unity build to shorten build time"})
option("stdlib", {default = "libstdc++", description = "stl to use"})

add_rules("mode.debug", "mode.release", "mode.releasedbg")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "out"})

set_languages("c++23")
set_targetdir("out/$(plat)-$(arch)-$(mode)")
set_encodings("source:utf-8")
set_encodings("utf-8") -- target
local alcy_modules = {
  "alcy.analyzer",
  "alcy.base",
  "alcy.codegen",
  "alcy.core",
  "alcy.ir",
  "alcy.lexer",
  "alcy.parser",
  "alcy.pipeline"
}

local is_libcxx = has_config("stdlib") and get_config("stdlib") == "libc++" and is_config("cxx", "clang", "clang++") and is_plat("linux", "macosx")
-- add_requires("zlib")
local catch2_configs = {}
-- local llvm_configs = {
--   shared = false,
--   clang = true,
--   lld = true,
--   libunwind = true,
--   libcxx = true,
--   libcxxabi = true,
--   assertions = is_mode("debug"),
--   components = { "core", "irreader", "mc", "support", "native", "all-targets" },
-- }
if is_libcxx then
  table.join2(catch2_configs, {
    cxxflags = "-stdlib=" .. stdlib,
    ldflags = {"-stdlib=" .. stdlib, "-lc++", "-lc++abi"}
  })
  -- table.join2(llvm_configs, {
  --   cxxflags = "-stdlib=libc++",
  --   ldflags = {"-stdlib=libc++", "-lc++", "-lc++abi"}
  -- })
end
add_requires("catch2 v3.12.0", {
  system = false,
  configs = catch2_configs,
})
-- add_requires("llvm 21.1.0", {
--   system = false,
--   configs = llvm_configs,
-- })


-- tasks
task("format")
  set_category(task_category)
  set_menu({
    usage = "xmake format",
    description = "format source code using clang-format"
  })
  on_run(function()
    local files = os.files("src/**/*.cc")
    table.join2(files, os.files("src/**/*.h"))
    table.join2(files, os.files("tests/**/*.cc"))
    table.join2(files, os.files("tests/**/*.h"))

    if #files > 0 then
      os.runv("clang-format", table.join({
        "--fail-on-incomplete-format",
        "--ferror-limit=1",
        "--sort-includes",
        "-i"
      }, files))
    end
  end)
task_end()

task("lint")
  set_category(task_category)
  set_menu({
    usage = "xmake lint",
    description = "lint source code using cpplint"
  })
  on_run(function()
    os.run("uv sync")
    local result = os.iorun("uv run cpplint --recursive src tests")
    print(result)

    local files = os.files("src/**/*.cc")
    table.join2(files, os.files("src/**/*.h"))
    table.join2(files, os.files("tests/**/*.cc"))
    table.join2(files, os.files("tests/**/*.h"))

    if #files > 0 then
      result = os.iorunv("clang-format", table.join({
        "--dry-run",
        "--fail-on-incomplete-format",
        "--ferror-limit=1",
        "--sort-includes",
        "-i"
      }, files))
      print(result)
    end
  end)
task_end()

task("analyze")
  set_category(task_category)
  set_menu({
    usage = "xmake analyze",
    description = "analyze source code using scan-build"
  })
  on_run(function()
    local result = os.iorunv("scan-build", { "xmake", "build" })
    print(result)
  end)
task_end()

task("checks")
  set_category(task_category)
  set_menu({
    usage = "xmake checks",
    description = "run format, lint, analyze tasks"
  })
  on_run(function()
    local result = os.iorun("xmake lint")
    print(result)
    result = os.iorun("xmake analyze")
    print(result)
  end)
task_end()

-- events
after_build(function(target)
  if has_config("timetrace") and get_config("timetrace") then
    local trace_dir = path.join(os.projectdir(), "out/timetrace")
    os.mkdir(trace_dir)
    for _, objfile in ipairs(target:objectfiles()) do
      local base = path.directory(objfile) .. "/" .. path.basename(objfile)
      local json = base .. ".json"
      if os.exists(json) then
        os.cp(json, path.join(trace_dir, path.basename(json) .. ".json"))
      end
    end
  end

  if has_config("optreport") and get_config("optreport") and is_mode("release") then
    local remark_dir = "out/remarks"
    os.mkdir(remark_dir)
    for _, yaml in ipairs(os.files(path.join(target:targetdir(), "**.opt.yaml"))) do
      os.cp(yaml, remark_dir)
    end
  end
end)

before_run(function(target)
  if has_config("coverage") and get_config("coverage") and target:name() == "tests" and not is_plat("windows") then
    os.setenv("LLVM_PROFILE_FILE", "default.profraw")
  end
end)

after_run(function(target)
  if has_config("coverage") and get_config("coverage") and target:name() == "tests" and not is_plat("windows") then
    local profraw = path.join(target:targetdir(), "default.profraw")
    local profdata = path.join(target:targetdir(), "default.profdata")
    local coverage_dir = "out/coverage"

    os.runv("llvm-profdata", { "merge", "-sparse", profraw, "-o", profdata })
    os.runv(
      "llvm-cov", {
        "show",
        target:targetfile(),
        "-instr-profile=" .. profdata,
        "-format=html",
        "-output-dir=" .. coverage_dir,
      }
    )

    cprint("${green}coverage report generated at: " .. path.join(coverage_dir, "index.html"))
  end
end)

-- targets
target("alcy.root_config")
  set_kind("phony", {public = true})
  set_warnings("all", "extra", "error", "pedantic", {public = true})
  add_cxxflags("-Wshadow", "-Wconversion", "-Wsign-conversion", "-Wnull-dereference", "-Wformat=2", {public = true})
  set_exceptions("none", {public = true})
  add_cxxflags("-fno-exceptions", "-fno-rtti", {public = true})
  add_cxxflags("-fstack-protector-strong", {public = true})
  add_defines("__STDC_CONSTANT_MACROS", "__STDC_FORMAT_MACROS", {public = true})
  add_defines("PROJECT_VERSION=\"" .. project_version .. "\"",  {public = true})
  add_includedirs("src", "third_party", {public = true})

  if is_plat("linux") then
    add_cxxflags("-fcf-protection=full", "-fPIE", {public = true})
    add_ldflags("-pie", {public = true})
    add_rpathdirs("$LD_LIBRARY_PATH", {public = true})
    add_defines("IS_PLAT_LINUX", {public = true})
  elseif is_plat("macosx") then
    add_cxxflags("-fPIE", {public = true})
    add_defines("IS_PLAT_MACOS", {public = true})
  elseif is_plat("windows") then
    add_defines("IS_PLAT_WINDOWS", {public = true})
  end

  if has_config("xray") and get_config("xray") and is_mode("debug") then
    add_cxxflags("-fxray-instrument", "-fxray-instruction-threshold=200", {public = true})
    add_ldflags("-fxray-instrument", {public = true})
  end
  if has_config("coverage") and get_config("coverage") and not is_plat("windows") then
    add_cxxflags("-fprofile-instr-generate", "-fcoverage-mapping", {public = true})
    add_ldflags("-fprofile-instr-generate", "-fcoverage-mapping", {public = true})
  end
  if has_config("optreport") and get_config("optreport") and is_mode("release") then
    add_cxxflags("-fsave-optimization-record", {public = true})
  end
  if has_config("timetrace") and get_config("timetrace") then
    add_cxxflags("-ftime-trace", {public = true})
  end
  if has_config("unitybuild") and get_config("unitybuild") then
    add_rules("c++.unity_build", { batchsize = 12 })
  end

  if is_mode("debug") then
    set_symbols("debug", {public = true})
    set_optimize("none", {public = true})
    add_defines("DEBUG", "LLVM_ENABLE_STATS", "LLVM_ENABLE_DUMP", {public = true})
    if has_config("sanitizers") and get_config("sanitizers") and not is_plat("windows") then
      set_policy("build.sanitizer.address", true)
      set_policy("build.sanitizer.undefined", true)
      set_policy("build.sanitizer.leak", true)
      add_cxxflags("-fsanitize=address,undefined,leak", "-fno-omit-frame-pointer", "-fno-sanitize-recover=all", {public = true})
      add_ldflags("-fsanitize=address,undefined,leak", {public = true})
    end
  elseif is_mode("release") then
    set_symbols("hidden", {public = true})
    set_optimize("fastest", {public = true})
    set_strip("all", {public = true})
    add_defines("NDEBUG", {public = true})
    if has_config("native") and get_config("native") and not is_cross() then
      add_cxxflags("-march=native", {public = true})
    end
  end

  if is_libcxx then
    add_cxxflags("-stdlib=libc++", {public = true})
    add_ldflags("-stdlib=libc++", "-lc++", "-lc++abi", {public = true})
  end
target_end()

target("alcy.analyzer")
  add_deps("alcy.root_config")
  set_kind("object")
  add_files("src/analyzer/**.cc")
  set_default(false)
target_end()

target("alcy.base")
  add_deps("alcy.root_config")
  set_kind("object")
  add_files("src/base/**.cc")
  set_default(false)
target_end()

target("alcy.codegen")
  add_deps("alcy.root_config")
  set_kind("object")
  set_pcxxheader("src/codegen/llvm/pch.h")
  add_files("src/codegen/**.cc")
  -- add_packages("llvm")
  set_default(false)
target_end()

target("alcy.core")
  add_deps("alcy.root_config")
  set_kind("object")
  add_files("src/core/**.cc")
  set_default(false)
target_end()

target("alcy.ir")
  add_deps("alcy.root_config")
  set_kind("object")
  add_files("src/ir/**.cc")
  set_default(false)
target_end()

target("alcy.lexer")
  add_deps("alcy.root_config")
  set_kind("object")
  add_files("src/lexer/**.cc")
  set_default(false)
target_end()

target("alcy.parser")
  add_deps("alcy.root_config")
  set_kind("object")
  add_files("src/parser/**.cc")
  set_default(false)
target_end()

target("alcy.pipeline")
  add_deps("alcy.root_config")
  set_kind("object")
  add_files("src/pipeline/**.cc")
  set_default(false)
target_end()

target("alcy")
  add_deps("alcy.root_config")
  set_kind("binary")
  add_files("src/app/**.cc")
  add_deps(alcy_modules)
  set_default(true)
target_end()

target("tests")
  add_deps("alcy.root_config")
  set_kind("binary")
  add_files("tests/**.cc")
  add_deps(alcy_modules)
  add_packages("catch2")
  set_group("test")
  set_default(false)
target_end()
