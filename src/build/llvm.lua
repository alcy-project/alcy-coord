package("alcy_llvm")
add_configs("cache_llvm", {
  description = "use preinstalled LLVM in third_party/llvm-project/.alcy/install",
  default = true,
  type = "boolean",
})
add_configs("download_llvm", {
  description = "download prebuilt LLVM from GitHub releases if available",
  default = true,
  type = "boolean",
})

-- from `llvm-config --libs --ignore-libllvm codegen`
local llvm_components = {
  "LLVMCodeGen",
  "LLVMTarget",
  "LLVMScalarOpts",
  "LLVMInstCombine",
  "LLVMAggressiveInstCombine",
  "LLVMObjCARCOpts",
  "LLVMTransformUtils",
  "LLVMCodeGenTypes",
  "LLVMCGData",
  "LLVMBitWriter",
  "LLVMAnalysis",
  "LLVMProfileData",
  "LLVMSymbolize",
  "LLVMDebugInfoBTF",
  "LLVMDebugInfoPDB",
  "LLVMDebugInfoMSF",
  "LLVMDebugInfoCodeView",
  "LLVMDebugInfoGSYM",
  "LLVMDebugInfoDWARF",
  "LLVMObject",
  "LLVMTextAPI",
  "LLVMMCParser",
  "LLVMIRReader",
  "LLVMAsmParser",
  "LLVMMC",
  "LLVMDebugInfoDWARFLowLevel",
  "LLVMBitReader",
  "LLVMFrontendHLSL",
  "LLVMCore",
  "LLVMRemarks",
  "LLVMBitstreamReader",
  "LLVMBinaryFormat",
  "LLVMTargetParser",
  "LLVMSupport",
  "LLVMDemangle",
}

on_load(function(package)
  local install_dir = path.join(
    os.projectdir(),
    "third_party",
    "llvm-project",
    ".alcy",
    "install"
  )
  package:set("installdir", install_dir)

  os.exec("uv sync", { cwd = os.projectdir() })
  local llvm_build_type = "Debug"
  if is_mode("release") then
    llvm_build_type = "Release"
  elseif is_mode("releasedbg") then
    llvm_build_type = "RelWithDebInfo"
  end

  local llvm_setup_script =
    path.join(os.projectdir(), "scripts", "setup_llvm.py")
  local args = {
    "run",
    llvm_setup_script,
    "--type=" .. llvm_build_type,
  }
  if not package:config("download_llvm") then
    table.join2(args, "--disable-download-llvm")
  end
  if not package:config("cache_llvm") then
    table.join2(args, "--disable-cache-llvm")
  end

  os.execv("uv", args, { cwd = os.projectdir() })
end)

on_fetch(function(package)
  local result = {}
  result.includedirs = package:installdir("include")
  result.linkdirs = package:installdir("lib")
  result.links = llvm_components
  return result
end)
