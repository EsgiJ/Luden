import os

# Configuration: You can update these values as needed.
template_path = "../extern/rttr/src/rttr/detail/base/version.h.in"
output_path = "../extern/rttr/src/rttr/detail/base/version.h"

# Define the values you want to inject into the template.
version_info = {
    "@RTTR_VERSION_MAJOR@": "0",
    "@RTTR_VERSION_MINOR@": "9",
    "@RTTR_VERSION_PATCH@": "6"
}

# Check if the template file exists
if not os.path.isfile(template_path):
    print(f"Error: Template file not found: {template_path}")
    exit(1)

# Read template content
with open(template_path, "r", encoding="utf-8") as file:
    content = file.read()

# Replace placeholders with actual values
for placeholder, value in version_info.items():
    content = content.replace(placeholder, value)

# Write the processed content to the output file
with open(output_path, "w", encoding="utf-8") as file:
    file.write(content)

print(f"Generated {output_path} successfully.")
