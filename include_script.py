import os
import re

def update_include_statements(directory, header_folder):
    # Regex pattern to find includes with brackets or quotes
    pattern = re.compile(r'#include [<"]' + re.escape(header_folder) + r'/([^>"]+)[>"]')

    # Walk through the directory
    for root, _, files in os.walk(directory):
        for filename in files:
            if filename.endswith(('.c', '.h')):  # Targeting C and header files
                filepath = os.path.join(root, filename)
                with open(filepath, 'r') as file:
                    contents = file.read()
                
                # Replace the pattern with the new format
                new_contents = pattern.sub(r'#include "\1"', contents)

                # Write back if changes were made
                if new_contents != contents:
                    with open(filepath, 'w') as file:
                        file.write(new_contents)
                    print(f"Updated includes in {filepath}")

# Define the directory containing the C files and the specific header folder name
source_directory = './'
header_directory_name = 'libxml2-2.12.6/include/libxml'

update_include_statements(source_directory, header_directory_name)
