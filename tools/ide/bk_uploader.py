#!/usr/bin/env python3

import argparse
import subprocess
import sys
from pathlib import Path


def main() -> int:
    parser = argparse.ArgumentParser(description="Arduino BK upload wrapper")
    sub = parser.add_subparsers(dest="cmd", required=True)

    upload_p = sub.add_parser("upload")
    upload_p.add_argument("--tool", required=True)
    upload_p.add_argument("--port", required=True)
    upload_p.add_argument("--baud", required=True)
    upload_p.add_argument("--input", required=True)

    args = parser.parse_args()

    if args.cmd != "upload":
        return 1

    try:
        cmd = [
            str(Path(args.tool).resolve()),
            "--log_level",
            "1",
            "download",
            "-p",
            str(args.port),
            "-b",
            str(args.baud),
            "-i",
            str(Path(args.input).resolve()),
            "--reboot",
        ]
        return subprocess.call(cmd)
    except Exception as exc:
        print(f"bk_uploader error: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
