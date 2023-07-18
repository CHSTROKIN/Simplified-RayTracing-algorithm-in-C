from PIL import Image
import numpy as np

read_from = "brick_wall512.jpg"
write_to  = "brick_wall.c3et"

img = Image.open(read_from)
height, width = img.size

contents = f"{height} {width}\n"
print(contents)

for pixel in img.getdata():
    if len(pixel) == 4:
        r, g, b, _ = pixel
    else:
        r, g, b = pixel
    if g > r and g > b:
        print(pixel)
    contents += f"{r} {g} {b}\n"

with open(write_to, "w") as f:
    f.write(contents)
