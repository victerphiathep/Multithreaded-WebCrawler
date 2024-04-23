import os
import re

## Messed with the build files so I needed to write a script to fix them

def update_include_statements(directory):
    # Pattern to find and replace includes
    pattern = re.compile(r'#include <libxml2/(libxml/[^>]+)>')
    replacement = r'#include <\1>'

    # Walk through the directory
    for root, dirs, files in os.walk(directory):
        for filename in files:
            if filename.endswith('.h'):
                filepath = os.path.join(root, filename)
                with open(filepath, 'r') as file:
                    contents = file.readlines()
                
                # Flag to check if changes were made
                changed = False
                with open(filepath, 'w') as file:
                    for line in contents:
                        new_line = pattern.sub(replacement, line)
                        if new_line != line:
                            changed = True
                        file.write(new_line)
                
                if changed:
                    print(f"Updated includes in {filepath}")

# Set the directory path to 'libxml' folder
libxml_directory = '/usr/include/libxml2/libxml'
update_include_statements(libxml_directory)
