#!/usr/bin/env python3
"""
OBC-STD - compile a sketch outside the Arduino IDE, using the same flags as platform.txt.
Used to test the core in CI / without the IDE.

usage: python3 tools/test_build.py path/to/sketch.ino [build_dir]
Copyright (c) 2026 Damien Fauconnier / AIR ESIEA - MIT License
"""
import os, re, subprocess, sys, pathlib

ROOT = pathlib.Path(__file__).resolve().parent.parent / "samd51obc"
CORE = ROOT / "cores" / "obc"
VARIANT = ROOT / "variants" / "obc_std"
LIBS = ROOT / "libraries"
GCC = os.environ.get("ARM_GCC_PREFIX", "arm-none-eabi-")

MCU = "-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16".split()
DEFS = ["-DF_CPU=120000000L", "-DARDUINO=10819", "-DARDUINO_OBC_STD_MINIFS",
        "-DARDUINO_ARCH_SAMD51OBC", "-D__SAMD51J20A__", "-DUSB_VID=0x1209", "-DUSB_PID=0x0001"]
INC = [CORE, CORE / "cmsis/Core/Include", CORE / "cmsis/Device/samd51/include", CORE / "tinyusb/src", VARIANT]
WARN = ["-Wall", "-Wextra", "-Wno-unused-parameter"]
CFLAGS = MCU + ["-c", "-g", "-Os", "-std=gnu11", "-ffunction-sections", "-fdata-sections",
                "-nostdlib", "--param", "max-inline-insns-single=500"]
CXXFLAGS = MCU + ["-c", "-g", "-Os", "-std=gnu++11", "-ffunction-sections", "-fdata-sections",
                  "-fno-threadsafe-statics", "-nostdlib", "--param", "max-inline-insns-single=500",
                  "-fno-rtti", "-fno-exceptions"]


def run(cmd):
    r = subprocess.run([str(c) for c in cmd], capture_output=True, text=True)
    if r.returncode != 0 or r.stderr.strip():
        print(" ".join(str(c) for c in cmd[-3:]))
        print(r.stderr)
    if r.returncode != 0:
        sys.exit(1)


def sources(folder):
    return sorted(p for p in folder.rglob("*") if p.suffix in (".c", ".cpp", ".S"))


def compile_all(files, out, includes, warn):
    objs = []
    for f in files:
        o = out / (f.as_posix().replace("/", "_").lstrip("_") + ".o")
        flags = CFLAGS if f.suffix in (".c", ".S") else CXXFLAGS
        tool = GCC + ("gcc" if f.suffix in (".c", ".S") else "g++")
        extra = warn if warn else ["-w"]
        run([tool] + flags + extra + DEFS + ["-I" + str(i) for i in includes] + [f, "-o", o])
        objs.append(o)
    return objs


def main():
    sketch = pathlib.Path(sys.argv[1]).resolve()
    out = pathlib.Path(sys.argv[2] if len(sys.argv) > 2 else "/tmp/obc_build") / sketch.stem
    out.mkdir(parents=True, exist_ok=True)

    code = sketch.read_text()
    used_libs = [d / "src" for d in LIBS.iterdir() if re.search(r'#include\s*[<"]%s\.h[>"]' % d.name, code)]
    # extra libraries (Library Manager style folders), e.g. OBC_EXTRA_LIBS=/path/SdFat:/path/Adafruit_BusIO
    for d in filter(None, os.environ.get("OBC_EXTRA_LIBS", "").split(":")):
        d = pathlib.Path(d)
        used_libs.append(d / "src" if (d / "src").is_dir() else d)
    # libraries used by the extra libraries (SPI, Wire) are always added
    for name in ("SPI", "Wire"):
        if LIBS / name / "src" not in used_libs:
            used_libs.append(LIBS / name / "src")
    lib_inc = list(used_libs)

    cpp = out / (sketch.stem + ".ino.cpp")
    cpp.write_text('#include <Arduino.h>\n#line 1 "%s"\n' % sketch.name + code)

    includes = INC + lib_inc
    objs = []
    objs += compile_all([cpp], out, includes, WARN)
    for d in used_libs:
        own_lib = LIBS in d.parents
        files = sources(d) if own_lib or (d.name == "src") else [f for f in d.glob("*") if f.suffix in (".c", ".cpp")]
        objs += compile_all(files, out, includes, WARN if own_lib else None)
    objs += compile_all(sources(VARIANT), out, includes, WARN)
    # core: our own files with warnings, third-party code (tinyusb) without
    own = [f for f in sources(CORE) if "tinyusb" not in f.parts]
    third = [f for f in sources(CORE) if "tinyusb" in f.parts]
    core_objs = compile_all(own, out, includes, WARN) + compile_all(third, out, includes, None)
    ar = out / "core.a"
    if ar.exists():
        ar.unlink()
    run([GCC + "ar", "rcs", ar] + core_objs)

    elf = out / (sketch.stem + ".elf")
    run([GCC + "gcc"] + MCU + ["-Os", "-Wl,--gc-sections", "--specs=nano.specs",
         "-u", "_printf_float", "-L" + str(VARIANT / "linker_scripts/gcc"),
         "-T" + str(VARIANT / "linker_scripts/gcc/flash_with_bootloader.ld"),
         "-Wl,-Map," + str(out / "out.map"), "-o", elf, "-Wl,--start-group"] + objs + [ar, "-lm", "-lc", "-lgcc", "-Wl,--end-group"])
    run([GCC + "objcopy", "-O", "binary", elf, out / (sketch.stem + ".bin")])
    size = subprocess.run([GCC + "size", "-A", str(elf)], capture_output=True, text=True).stdout
    text = sum(int(m) for m in re.findall(r"^\.(?:text|ARM\.exidx|data)\s+(\d+)", size, re.M))
    ram = sum(int(m) for m in re.findall(r"^\.(?:data|bss)\s+(\d+)", size, re.M))
    print("OK %-28s flash %7d bytes   RAM %6d bytes" % (sketch.name, text, ram))


if __name__ == "__main__":
    main()
