import os
import sys
import datetime
import imageio.v2 as imageio
from glob import glob
import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path

SIZE = 100
max_iter = 10_000
cannon_shot_time = 7_000
cannon_strength = 1.0
cone_limiter = 6.0
max_alpha = np.pi / cone_limiter
speed = 1.5
shockwave_shortness = 3.0
cannon_x_loc = 0.5


def cannon(x, y, iterr):
    if iterr - cannon_shot_time <= 0:
        return 0.0

    time = (iterr - cannon_shot_time) / ((max_iter / speed) - cannon_shot_time)

    if y > time:
        return 0.0

    x_prim = np.abs(cannon_x_loc - x)
    alpha_rad = np.arctan(x_prim / time)

    if alpha_rad >= max_alpha:
        return 0.0

    y_prim = np.sqrt(time * time - x_prim * x_prim)

    if y > y_prim:  # if y is higher than 'y
        return 0.0

    # explosion
    # return max(y_prim - y, 0) * np.cos(alpha_rad * cone_limiter * 0.5) * cannon_strength

    return (
        (1.0 - min(max(y_prim - y, 0) * shockwave_shortness, 1.0))
        * np.cos(alpha_rad * cone_limiter * 0.5)
        * cannon_strength
    )

def fire_wave(x, y, iterr):
    t = iterr / 1000
    a = (t/70)*-10
    b = (t/70)*-5
    c = -0.6
    # wave_strength = 100

    val = (((x * np.cos(c) - y * np.sin(c))**2) / a**2) + (((x*np.cos(c) + y*np.sin(c))**2) / b**2)
    result = val
    if val <= 1:
        return result
    else:
        return 0


def create_heatmap(i, save_dir):
    increm = 1.0 / SIZE
    data = np.array(
        [
            [x * increm, y * increm, fire_wave(x * increm, y * increm, i)]
            for x in range(SIZE)
            for y in range(SIZE)
        ]
    )

    # Separate data into x, y, and value columns
    x = data[:, 0]
    y = data[:, 1]
    value = data[:, 2]

    # Ensure x, y, and value are 1D arrays
    x = np.unique(x)
    y = np.unique(y)

    # Create 2D grids using np.meshgrid
    X, Y = np.meshgrid(x, y)

    # Reshape the value array to match the shape of X and Y
    value_2d = value.reshape(len(y), len(x))

    # Create the heatmap
    fig, ax = plt.subplots()
    norm = plt.Normalize(1,0)
    heatmap = ax.pcolormesh(Y, X, value_2d, cmap="viridis", norm=norm)  # Use a good colormap

    # Add labels and title
    ax.set_xlabel("X")
    ax.set_ylabel("Y")
    ax.set_title(f"test {i}")

    # Add colorbar
    fig.colorbar(heatmap, label="Value")

    # Create the output filename
    output_filename = os.path.join(save_dir, f"test {i}.jpg")

    # Save the heatmap as a PNG with transparency for better visualization
    plt.savefig(output_filename, transparent=False)
    plt.close(fig)  # Close the plot to avoid memory leaks


# Function to create a GIF from images
def create_gif_from_images(images, gif_path):
    l = len(images)

    with imageio.get_writer(gif_path, mode="I", loop=0) as writer:
        for i, image_path in enumerate(images):
            image = imageio.imread(image_path)
            writer.append_data(image)
            print(f"\r{i+1}/{l}", end="")


def get_jpg_files_ordered_by_creation_date(directory):
    jpg_files = list(Path(directory).glob("*.jpg"))
    jpg_files.sort(key=lambda x: x.stat().st_ctime)
    return jpg_files


curr_dir = os.path.curdir
imgs_dir = os.path.join(curr_dir, "imgs_cannon")
gifs_dir = os.path.join(curr_dir, "gifs")

if not os.path.exists(imgs_dir):
    os.makedirs(imgs_dir)

for i in range(0, max_iter, 200):
    print(f"\r{i+100}/{max_iter}", end="")
    create_heatmap(i, imgs_dir)

print("\nHeatmaps created and saved successfully!")

jpg_files = get_jpg_files_ordered_by_creation_date(imgs_dir)
existing_gif_files = len(list(Path(gifs_dir).glob("cannon_*.gif")))
new_gif_number = existing_gif_files + 1
gif_filename = f"cannon_{new_gif_number}.gif"
gif_path = os.path.join(gifs_dir, gif_filename)
create_gif_from_images(jpg_files, gif_path)
print(f"\nCreated gif: {gif_filename}")
