#!/bin/bash

# Get the current date and time
timestamp=$(date +"%Y%m%d_%H%M%S")

# Specify the directory to save the screenshots
screenshot_directory="$HOME/Pictures/Screenshots/"

# Create the directory if it doesn't exist
mkdir -p "$screenshot_directory"

# Define the screenshot file name
screenshot_file="$screenshot_directory/screenshot_$timestamp.png"

# Take the screenshot using scrot
scrot "$screenshot_file"

# Notify the user about the successful screenshot
notify-send "Screenshot taken" "Saved as: $screenshot_file"
