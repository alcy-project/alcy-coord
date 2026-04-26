package("fpag")
set_urls("https://github.com/pug523/fpag.git")

add_versions("v0.0.1", "834d1857d64f4b322ad0e34a1ff620c36d44807b")
add_versions("v0.0.2", "9634712514a729897b378173e83d9e96007507ed")
add_versions("v0.0.3", "b55831d03b2c91d148206b07cb4447335a18153e")
add_versions("v0.0.4", "657e3867eb6fc35a652f1ddd8252e7021321a148")
add_versions("v0.0.5", "f11496616709a2e75c50d6ca2ad6f73b09ee6c13")
add_versions("v0.0.6", "7c0b59c77d396529ea15c7fcd114ee2f56944ca4")
add_versions("v0.0.7", "779139e798e3ee567dec7db31c755771662c505f")
add_versions("v0.0.8", "d6644009fe518f171c55a920ced92dfda7a47f9f")
add_versions("v0.0.9", "8d23657b1149db5d8f6059a57d054817877b23ad")
add_versions("v0.0.10", "6fcc98dc6cdb442dd8bc5d68524555b51a2b4326")

add_configs("fmtlib", {
  description = 'Use fmtlib for formatting (You need to manually execute add_requires("fmt <version>") with correct version and add_packages("fmt").)',
  default = true,
  type = "boolean",
})
add_configs("libunwind", {
  description = "Use libunwind for stack tracing",
  default = false,
  type = "boolean",
})
add_configs(
  "stdlib",
  { description = "stl to use", default = "libstdc++", type = "string" }
)

add_deps("xxhash v0.8.3")

on_load(function(package)
  if package:config("fmtlib") then
    package:add("deps", "fmt")
    package:add("defines", "FPAG_BUILD_FLAG_INTERNAL_USE_FMTLIB()=1")
  else
    package:add("defines", "FPAG_BUILD_FLAG_INTERNAL_USE_FMTLIB()=0")
  end
  if package:config("libunwind") and package:is_plat("linux") then
    package:add("deps", "libunwind")
  end
end)

on_install("linux", "macosx", "windows", function(package)
  local configs = {}
  configs.tests = false
  configs.benchmarks = false
  configs.fmtlib = package:config("fmtlib")
  configs.libunwind = package:config("libunwind")
  configs.stdlib = package:config("stdlib")
  configs.kind = package:config("shared") and "shared" or "static"

  import("package.tools.xmake").install(package, configs)
end)

on_test(function(package)
  assert(
    package:check_cxxsnippets(
      {
        test = [[
                #include <fpag/base/math_util.h>
                #include <assert.h>
                void test() {
                    assert(base::next_power_of_two(31) == 32);
                    assert(base::next_power_of_two(4096) == 4096);
                }
            ]],
      },
      { configs = { languages = "c++23" }, includes = "fpag/base/math_util.h" }
    )
  )
end)
