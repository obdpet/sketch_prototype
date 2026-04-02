#!/usr/bin/env python3
from __future__ import annotations

import argparse
from pathlib import Path

from PIL import Image, ImageSequence


def rgb888_to_rgb565(r: int, g: int, b: int) -> int:
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)


def sanitize_symbol(name: str) -> str:
    out = []
    for ch in name:
        if ch.isalnum():
            out.append(ch.lower())
        else:
            out.append("_")
    symbol = "".join(out).strip("_")
    while "__" in symbol:
        symbol = symbol.replace("__", "_")
    return symbol or "anim"


def get_resample_filter(name: str) -> int:
    mapping = {
        "nearest": Image.Resampling.NEAREST,
        "bilinear": Image.Resampling.BILINEAR,
        "bicubic": Image.Resampling.BICUBIC,
        "lanczos": Image.Resampling.LANCZOS,
    }
    return mapping[name]


def build_header(
    input_path: Path, output_path: Path, symbol: str, width: int, height: int, resample_name: str
) -> None:
    gif = Image.open(input_path)
    resample_filter = get_resample_filter(resample_name)
    frame_names: list[str] = []
    lines: list[str] = [
        "#pragma once",
        "#include <pgmspace.h>",
        "#include <stdint.h>",
        "",
        f"static constexpr char {symbol}_name[] = \"{input_path.name}\";",
        f"static constexpr uint16_t {symbol}_width = {width};",
        f"static constexpr uint16_t {symbol}_height = {height};",
        "",
    ]

    for index, gif_frame in enumerate(ImageSequence.Iterator(gif)):
        frame_rgba = gif_frame.convert("RGBA")
        flattened = Image.new("RGBA", frame_rgba.size, (255, 255, 255, 255))
        flattened.alpha_composite(frame_rgba)
        frame = flattened.convert("RGB").resize((width, height), resample_filter)
        frame_name = f"{symbol}_frame_{index:03d}"
        frame_names.append(frame_name)

        pixels: list[str] = []
        for y in range(frame.height):
            for x in range(frame.width):
                pixels.append(f"0x{rgb888_to_rgb565(*frame.getpixel((x, y))):04X}")

        lines.append(f"static const uint16_t {frame_name}[] PROGMEM = {{")
        for offset in range(0, len(pixels), 12):
            chunk = ", ".join(pixels[offset : offset + 12])
            suffix = "," if offset + 12 < len(pixels) else ""
            lines.append(f"  {chunk}{suffix}")
        lines.append("};")
        lines.append("")

    lines.append(f"static const uint16_t* const {symbol}_frames[] PROGMEM = {{")
    for index, frame_name in enumerate(frame_names):
        suffix = "," if index + 1 < len(frame_names) else ""
        lines.append(f"  {frame_name}{suffix}")
    lines.append("};")
    lines.append("")
    lines.append(f"static constexpr uint16_t {symbol}_frame_count = {len(frame_names)};")
    lines.append("")

    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text("\n".join(lines), encoding="ascii")


def main() -> None:
    parser = argparse.ArgumentParser(description="Convert GIF frames to RGB565 PROGMEM header.")
    parser.add_argument("input", type=Path, help="Input GIF path")
    parser.add_argument("output", type=Path, help="Output header path")
    parser.add_argument("--symbol", default="", help="Symbol prefix for generated arrays")
    parser.add_argument("--width", type=int, default=160, help="Output frame width")
    parser.add_argument("--height", type=int, default=140, help="Output frame height")
    parser.add_argument(
        "--resample",
        choices=["nearest", "bilinear", "bicubic", "lanczos"],
        default="lanczos",
        help="Resize filter",
    )
    args = parser.parse_args()

    symbol = args.symbol or sanitize_symbol(args.input.stem)
    build_header(args.input, args.output, symbol, args.width, args.height, args.resample)
    print(f"Generated {args.output} from {args.input.name}")


if __name__ == "__main__":
    main()
