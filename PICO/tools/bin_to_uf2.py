#!/usr/bin/env python3

import argparse
import math
import struct
from pathlib import Path

UF2_MAGIC_START0 = 0x0A324655
UF2_MAGIC_START1 = 0x9E5D5157
UF2_MAGIC_END = 0x0AB16F30
UF2_FLAG_FAMILY_ID_PRESENT = 0x00002000
PAYLOAD_SIZE = 256
BLOCK_SIZE = 512
DATA_SIZE = 476


def parse_int(value: str) -> int:
    return int(value, 0)


def build_block(payload: bytes, block_no: int, total_blocks: int, target_addr: int, family_id: int) -> bytes:
    payload = payload.ljust(PAYLOAD_SIZE, b"\x00")
    flags = UF2_FLAG_FAMILY_ID_PRESENT
    header = struct.pack(
        "<IIIIIIII",
        UF2_MAGIC_START0,
        UF2_MAGIC_START1,
        flags,
        target_addr,
        PAYLOAD_SIZE,
        block_no,
        total_blocks,
        family_id,
    )
    body = payload.ljust(DATA_SIZE, b"\x00")
    trailer = struct.pack("<I", UF2_MAGIC_END)
    return header + body + trailer


def main() -> int:
    parser = argparse.ArgumentParser(description="Convert a raw .bin file to UF2.")
    parser.add_argument("input", type=Path, help="Input .bin file")
    parser.add_argument("output", type=Path, help="Output .uf2 file")
    parser.add_argument("--base", type=parse_int, default=0x10000000, help="Flash base address")
    parser.add_argument(
        "--family-id",
        type=parse_int,
        default=0xE48BFF59,
        help="UF2 family ID",
    )
    args = parser.parse_args()

    data = args.input.read_bytes()
    total_blocks = max(1, math.ceil(len(data) / PAYLOAD_SIZE))
    args.output.parent.mkdir(parents=True, exist_ok=True)

    with args.output.open("wb") as out:
        for block_no in range(total_blocks):
            start = block_no * PAYLOAD_SIZE
            chunk = data[start:start + PAYLOAD_SIZE]
            target_addr = args.base + start
            out.write(build_block(chunk, block_no, total_blocks, target_addr, args.family_id))

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
