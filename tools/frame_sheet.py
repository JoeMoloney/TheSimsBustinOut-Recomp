"""Contact sheet of SIMS_FRAME_DUMP_DIR frame_NNNNNN.ppm dumps: python3 tools/frame_sheet.py <dir> [cols] [thumb_w]"""
import sys, glob, os
from PIL import Image, ImageDraw

d = sys.argv[1]
cols = int(sys.argv[2]) if len(sys.argv) > 2 else 8
tw = int(sys.argv[3]) if len(sys.argv) > 3 else 160
files = sorted(glob.glob(os.path.join(d, "frame_*.ppm")))
if not files:
    sys.exit("no frames")
first = Image.open(files[0])
th = tw * first.height // first.width
rows = (len(files) + cols - 1) // cols
sheet = Image.new("RGB", (cols * tw, rows * th), "black")
draw = ImageDraw.Draw(sheet)
for i, f in enumerate(files):
    im = Image.open(f).convert("RGB").resize((tw, th))
    x, y = (i % cols) * tw, (i // cols) * th
    sheet.paste(im, (x, y))
    draw.text((x + 2, y + 1), os.path.basename(f)[6:-4].lstrip("0"), fill="yellow")
sheet.save(os.path.join(d, "sheet.png"))
print(len(files), "frames ->", os.path.join(d, "sheet.png"))
