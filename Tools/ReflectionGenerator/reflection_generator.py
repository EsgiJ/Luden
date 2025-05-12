import os
import re

HEADER = """
#include <rttr/registration>
using namespace rttr;
"""

def parse_components(file_path):
    with open(file_path, 'r', encoding='utf-8') as f:
        content = f.read()

    class_pattern = re.compile(r'(?:class|struct)\s+\w+\s+COMPONENT\(\)\s+(\w+)')
    property_pattern = re.compile(r'PROPERTY\(\)\s+([\w:<>\s\*&]+)\s+(\w+)\s*(?:=\s*[^;]+)?;')
    constructor_pattern = re.compile(rf'(\w+)\s*\((.*?)\)', re.DOTALL)

    registrations = []
    classes = list(class_pattern.finditer(content))

    if not classes:
        print(f"  No COMPONENT() classes found in {file_path}")
        return registrations

    for i, class_match in enumerate(classes):
        class_name = class_match.group(1)
        print(f"Found COMPONENT class: {class_name} in {file_path}")

        start_pos = class_match.end()
        end_pos = classes[i + 1].start() if i + 1 < len(classes) else len(content)
        body_content = content[start_pos:end_pos]

        # Extract properties
        properties = []
        for prop in property_pattern.finditer(body_content):
            prop_type, var_name = prop.groups()
            print(f"  Found PROPERTY: {var_name}")
            properties.append((prop_type.strip(), var_name.strip()))

        # Extract constructors with parameter types and names
        constructors = []
        for ctor in constructor_pattern.finditer(body_content):
            ctor_name, param_str = ctor.groups()
            if ctor_name != class_name:
                continue
            params = [p.strip() for p in param_str.split(',') if p.strip()]
            if params:
                param_signature = ', '.join(params)
                constructors.append(f'.constructor<{param_signature}>()')
            else:
                constructors.append('.constructor<>()')

        registration = f'registration::class_<{class_name}>("{class_name}")\n'
        for ctor in constructors:
            registration += f'    {ctor}\n'
        for prop_type, prop_name in properties:
            registration += f'    .property("{prop_name}", &{class_name}::{prop_name})\n'
        registration += ';\n'
        registrations.append(registration)

    return registrations


def parse_directory(input_directory, output_file):
    all_registrations = []

    for root, _, files in os.walk(input_directory):
        for file in files:
            if file.endswith(".h") or file.endswith(".hpp"):
                file_path = os.path.join(root, file)
                registrations = parse_components(file_path)
                all_registrations.extend(registrations)

    if all_registrations:
        os.makedirs(os.path.dirname(output_file), exist_ok=True)
        with open(output_file, 'w', encoding='utf-8') as out_file:
            out_file.write(HEADER)
            for reg in all_registrations:
                out_file.write(reg + '\n')
        print(f"\nGenerated {output_file}")
    else:
        print("\nNo matching components or properties found.")


# Run parser
parse_directory('Engine/include', 'Engine/src/Generated/Components.gen.cpp')
