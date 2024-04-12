import os
import sys
import datetime
import imageio.v2 as imageio
from glob import glob
import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path


def create_heatmap(data_file, save_dir):
    """
    Creates a heatmap from data in a file and saves it as a PNG.

    Args:
        data_file (str): Path to the data file.
        save_dir (str): Path to the directory for saving the heatmap image.
    """

    # Read data from the file, handling potential errors gracefully
    try:
        with open(data_file, "r") as f:
            lines = f.readlines()
    except FileNotFoundError:
        print(f"Error: File not found: {data_file}")
        return

    # Extract data into a NumPy array, handling potential formatting issues
    try:
        data = np.array([[float(x) for x in line.split()] for line in lines])
    except ValueError:
        print(f"Error: Invalid data format in file: {data_file}")
        return

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
    norm = plt.Normalize(0,1)
    heatmap = ax.pcolormesh(Y, X, value_2d, cmap="viridis", norm=norm)  # Use a good colormap

    # Add labels and title
    ax.set_xlabel("X")
    ax.set_ylabel("Y")
    ax.set_title(os.path.basename(data_file).replace(".data", ""))

    # Add colorbar
    fig.colorbar(heatmap, label="Value")

    # Create the output filename
    output_filename = os.path.join(
        save_dir, os.path.splitext(os.path.basename(data_file))[0] + ".jpg"
    )

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
data_dir = os.path.join(curr_dir, "data")
imgs_dir = os.path.join(curr_dir, "imgs")
gifs_dir = os.path.join(curr_dir, "gifs")

if not os.path.exists(imgs_dir):
    os.makedirs(imgs_dir)

# Get all data files sorted by creation date (using os.path.getmtime)
data_files = sorted(glob(os.path.join(data_dir, "*.data")), key=os.path.getmtime)
l = len(data_files)

for i, data_file in enumerate(data_files):
    print(f"\r{i+1}/{l}", end="")
    create_heatmap(data_file, imgs_dir)

print("\nHeatmaps created and saved successfully!")

jpg_files = get_jpg_files_ordered_by_creation_date(imgs_dir)
existing_gif_files = len(list(Path(gifs_dir).glob("animation_*.gif")))
new_gif_number = existing_gif_files + 1
gif_filename = f"animation_{new_gif_number}.gif"
gif_path = os.path.join(gifs_dir, gif_filename)
create_gif_from_images(jpg_files, gif_path)
print(f"\nCreated gif: {gif_filename}")
