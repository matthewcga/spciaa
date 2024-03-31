import os
import sys
import datetime
from glob import glob
import matplotlib.pyplot as plt
import numpy as np

def create_heatmap(data_file, save_dir):
  """
  Creates a heatmap from data in a file and saves it as a PNG.

  Args:
      data_file (str): Path to the data file.
      save_dir (str): Path to the directory for saving the heatmap image.
  """

  # Read data from the file, handling potential errors gracefully
  try:
    with open(data_file, 'r') as f:
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
  x = data[:, 1]
  y = data[:, 0]
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
  heatmap = ax.pcolormesh(X, Y, value_2d, cmap='viridis')  # Use a good colormap

  # Add labels and title
  ax.set_xlabel('X')
  ax.set_ylabel('Y')
  ax.set_title(os.path.basename(data_file).replace('.data', ''))

  # Add colorbar
  fig.colorbar(heatmap, label='Value')

  # Create the output filename
  output_filename = os.path.join(save_dir, os.path.splitext(os.path.basename(data_file))[0] + '.png')

  # Save the heatmap as a PNG with transparency for better visualization
  plt.savefig(output_filename, transparent=True)
  plt.close(fig)  # Close the plot to avoid memory leaks

# Define the data directory constant

curr_dir = os.path.curdir
data_dir = os.path.join(curr_dir, 'data')
imgs_dir = os.path.join(curr_dir, 'imgs')

if not os.path.exists(imgs_dir):
  os.makedirs(imgs_dir)

# Get all data files sorted by creation date (using os.path.getmtime)
data_files = sorted(glob(os.path.join(data_dir, '*.data')), key=os.path.getmtime)
l = len(data_files)

for i, data_file in enumerate(data_files):
  print(f'\r{i+1}/{l}', end='')
  create_heatmap(data_file, imgs_dir)

print("\nHeatmaps created and saved successfully!")