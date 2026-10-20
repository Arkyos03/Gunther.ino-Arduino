#!/usr/bin/env python3
"""
OBC-STD - build the Boards Manager archive and the package index.

usage:
  python3 tools/make_release.py <base_url>

  <base_url> = public folder where you upload the files, for example
               https://github.com/<user>/obc-arduino/releases/download/v0.1.0

Output (in dist/):
  obc-samd51obc-<version>.tar.bz2
  package_obc_index.json   -> its URL goes in Arduino IDE > Preferences >
                              "Additional boards manager URLs"
Copyright (c) 2026 Damien Fauconnier / AIR ESIEA - MIT License
"""
import hashlib, json, pathlib, re, sys, tarfile

ROOT = pathlib.Path(__file__).resolve().parent.parent
PLATFORM = ROOT / "samd51obc"
DIST = ROOT / "dist"


def main():
    if len(sys.argv) < 2:
        print(__doc__)
        sys.exit(1)
    base_url = sys.argv[1].rstrip("/")
    version = re.search(r"^version=(.+)$", (PLATFORM / "platform.txt").read_text(), re.M).group(1).strip()

    DIST.mkdir(exist_ok=True)
    name = "obc-samd51obc-%s" % version
    archive = DIST / (name + ".tar.bz2")
    with tarfile.open(archive, "w:bz2") as tar:
        for f in sorted(PLATFORM.rglob("*")):
            if f.is_file() and "__pycache__" not in f.parts:
                tar.add(f, arcname=name + "/" + f.relative_to(PLATFORM).as_posix())
        for extra in ("LICENSE", "THIRD_PARTY_NOTICES.md"):
            tar.add(ROOT / extra, arcname=name + "/" + extra)

    data = archive.read_bytes()
    boards = re.findall(r"^\w+\.name=(.+)$", (PLATFORM / "boards.txt").read_text(), re.M)

    index = {
        "packages": [{
            "name": "obc",
            "maintainer": "AIR ESIEA",
            "websiteURL": base_url,
            "email": "",
            "help": {"online": base_url},
            "platforms": [{
                "name": "OBC-STD Minifs (AIR ESIEA)",
                "architecture": "samd51obc",
                "version": version,
                "category": "Contributed",
                "url": base_url + "/" + archive.name,
                "archiveFileName": archive.name,
                "checksum": "SHA-256:" + hashlib.sha256(data).hexdigest(),
                "size": str(len(data)),
                "help": {"online": base_url},
                "boards": [{"name": b.strip()} for b in boards],
                "toolsDependencies": [
                    {"packager": "arduino", "name": "arm-none-eabi-gcc", "version": "7-2017q4"},
                    {"packager": "arduino", "name": "bossac", "version": "1.8.0-48-gb176eee"},
                    {"packager": "arduino", "name": "openocd", "version": "0.11.0-arduino2"},
                ],
            }],
            "tools": [],
        }]
    }
    (DIST / "package_obc_index.json").write_text(json.dumps(index, indent=2) + "\n")
    print("archive :", archive, "(%d bytes)" % len(data))
    print("index   :", DIST / "package_obc_index.json")


if __name__ == "__main__":
    main()
