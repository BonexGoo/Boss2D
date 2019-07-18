major_version: "local"
minor_version: ""
default_target_cpu: "same_as_host"

default_toolchain {
  cpu: "k8"
  toolchain_identifier: "local_linux"
}
default_toolchain {
  cpu: "piii"
  toolchain_identifier: "local_linux"
}
default_toolchain {
  cpu: "arm"
  toolchain_identifier: "local_linux"
}
default_toolchain {
  cpu: "darwin"
  toolchain_identifier: "local_darwin"
}
default_toolchain {
  cpu: "ppc"
  toolchain_identifier: "local_linux"
}

toolchain {
  abi_version: "local"
  abi_libc_version: "local"
  compiler: "compiler"
  host_system_name: "local"
  needsPic: true
  target_libc: "local"
  target_cpu: "local"
  target_system_name: "local"
  toolchain_identifier: "local_linux"

  feature {
    name: "c++11"
    flag_set {
      action: "c++-compile"
      flag_group {
        flag: "-std=c++11"
      }
    }
  }

  feature {
    name: "stdlib"
    flag_set {
      action: "c++-link-executable"
      action: "c++-link-dynamic-library"
      flag_group {
        flag: "-lstdc++"
      }
    }
  }

  feature {
    name: "determinism"
    flag_set {
      action: "c-compile"
      action: "c++-compile"
      flag_group {
        # Make C++ compilation deterministic. Use linkstamping instead of these
        # compiler symbols.
        flag: "-Wno-builtin-macro-redefined"
        flag: "-D__DATE__=\"redacted\""
        flag: "-D__TIMESTAMP__=\"redacted\""
        flag: "-D__TIME__=\"redacted\""
      }
    }
  }

  feature {
    name: "alwayslink"
    flag_set {
      action: "c++-link-dynamic-library"
      action: "c++-link-executable"
      flag_group {
        flag: "-Wl,-no-as-needed"
      }
    }
  }

  # This feature will be enabled for builds that support pic by bazel.
  feature {
    name: "pic"
    flag_set {
      action: "c-compile"
      action: "c++-compile"
      flag_group {
        expand_if_all_available: "pic"
        flag: "-fPIC"
      }
      flag_group {
        expand_if_none_available: "pic"
        flag: "-fPIE"
      }
    }
  }

  # Security hardening on by default.
  feature {
    name: "hardening"
    flag_set {
      action: "c-compile"
      action: "c++-compile"
      flag_group {
        # Conservative choice; -D_FORTIFY_SOURCE=2 may be unsafe in some cases.
        # We need to undef it before redefining it as some distributions now
        # have it enabled by default.
        flag: "-U_FORTIFY_SOURCE"
        flag: "-D_FORTIFY_SOURCE=1"
        flag: "-fstack-protector"
      }
    }
    flag_set {
      action: "c++-link-dynamic-library"
      flag_group {
        flag: "-Wl,-z,relro,-z,now"
      }
    }
    flag_set {
      action: "c++-link-executable"
      flag_group {
        flag: "-pie"
        flag: "-Wl,-z,relro,-z,now"
      }
    }
  }

  feature {
    name: "warnings"
    flag_set {
      action: "c-compile"
      action: "c++-compile"
      flag_group {
        # All warnings are enabled. Maybe enable -Werror as well?
        flag: "-Wall"
        # Some parts of the codebase set -Werror and hit this warning, so
        # switch it off for now.
        flag: "-Wno-invalid-partial-specialization"
      }
    }
  }

  # Keep stack frames for debugging, even in opt mode.
  feature {
    name: "frame-pointer"
    flag_set {
      action: "c-compile"
      action: "c++-compile"
      flag_group {
        flag: "-fno-omit-frame-pointer"
      }
    }
  }

  feature {
    name: "build-id"
    flag_set {
      action: "c++-link-executable"
      action: "c++-link-dynamic-library"
      flag_group {
        # Stamp the binary with a unique identifier.
        flag: "-Wl,--build-id=md5"
        flag: "-Wl,--hash-style=gnu"
      }
    }
  }

  feature {
    name: "no-canonical-prefixes"
    flag_set {
      action: "c-compile"
      action: "c++-compile"
      action: "c++-link-executable"
      action: "c++-link-dynamic-library"
      flag_group {
        flag:"-no-canonical-prefixes"
      }
    }
  }

  feature {
    name: "disable-assertions"
    flag_set {
      action: "c-compile"
      action: "c++-compile"
      flag_group {
        flag: "-DNDEBUG"
      }
    }
  }

  feature {
    name: "linker-bin-path"

    flag_set {
      action: "c++-link-executable"
      action: "c++-link-dynamic-library"
      flag_group {
        flag: "-B/usr/bin/"
      }
    }
  }

  feature {
    name: "common"
    implies: "stdlib"
    implies: "c++11"
    implies: "determinism"
    implies: "alwayslink"
    implies: "hardening"
    implies: "warnings"
    implies: "frame-pointer"
    implies: "build-id"
    implies: "no-canonical-prefixes"
    implies: "linker-bin-path"
  }

  feature {
    name: "opt"
    implies: "common"
    implies: "disable-assertions"

    flag_set {
      action: "c-compile"
      action: "c++-compile"
      flag_group {
        # No debug symbols.
        # Maybe we should enable https://gcc.gnu.org/wiki/DebugFission for opt
        # or even generally? However, that can't happen here, as it requires
        # special handling in Bazel.
        flag: "-g0"

        # Conservative choice for -O
        # -O3 can increase binary size and even slow down the resulting binaries.
        # Profile first and / or use FDO if you need better performance than this.
        flag: "-O2"

        # Removal of unused code and data at link time (can this increase binary size in some cases?).
        flag: "-ffunction-sections"
        flag: "-fdata-sections"
      }
    }
    flag_set {
      action: "c++-link-dynamic-library"
      action: "c++-link-executable"
      flag_group {
        flag: "-Wl,--gc-sections"
      }
    }
  }

  feature {
    name: "fastbuild"
    implies: "common"
  }

  feature {
    name: "dbg"
    implies: "common"
    flag_set {
      action: "c-compile"
      action: "c++-compile"
      flag_group {
        flag: "-g"
      }
    }
  }

  # Set clang as a C/C++ compiler.
  tool_path { name: "gcc" path: "%{clang_path}" }

  # Use the default system toolchain for everything else.
  tool_path { name: "ar" path: "/usr/bin/ar" }
  tool_path { name: "compat-ld" path: "/usr/bin/ld" }
  tool_path { name: "cpp" path: "/usr/bin/cpp" }
  tool_path { name: "dwp" path: "/usr/bin/dwp" }
  tool_path { name: "gcov" path: "/usr/bin/gcov" }
  tool_path { name: "ld" path: "/usr/bin/ld" }
  tool_path { name: "nm" path: "/usr/bin/nm" }
  tool_path { name: "objcopy" path: "/usr/bin/objcopy" }
  tool_path { name: "objdump" path: "/usr/bin/objdump" }
  tool_path { name: "strip" path: "/usr/bin/strip" }

  # Enabled dynamic linking.
  linking_mode_flags { mode: DYNAMIC }

%{host_compiler_includes}
}
