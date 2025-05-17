import os
import re

HEADER = """
#include <rttr/registration>
using namespace rttr;
"""

NAMESPACE_WRAP = True

script_dir = os.path.dirname(os.path.realpath(__file__))
project_root = os.path.abspath(os.path.join(script_dir, '../../'))
os.chdir(project_root)


def extract_namespace_stack(content):
    namespace_pattern = re.compile(r'namespace\s+([\w:]+)\s*\{')
    stack = []
    result = []
    lines = content.splitlines()
    for line in lines:
        ns_match = namespace_pattern.search(line)
        if ns_match:
            stack.append(ns_match.group(1))
        if "}" in line and stack:
            stack.pop()
        result.append("::".join(stack) if stack else "")
    return result


def parse_properties_with_access_control(content: str):
    access_level = 'private'
    lines = content.split('\n')
    property_pattern = re.compile(r'PROPERTY\(\)\s+([\w:<>\s\*&]+)\s+(\w+)\s*(?:=\s*([^;]+))?;')

    properties = []
    for line in lines:
        stripped_line = line.strip()
        if stripped_line == 'public:':
            access_level = 'public'
        elif stripped_line == 'protected:':
            access_level = 'protected'
        elif stripped_line == 'private:':
            access_level = 'private'
        else:
            match = property_pattern.search(stripped_line)
            if match:
                prop_type, prop_name, default_value = match.groups()
                properties.append({
                    'type': prop_type.strip(),
                    'name': prop_name.strip(),
                    'default': default_value.strip() if default_value else None,
                    'access': access_level
                })
    return properties


def extract_classes(file_path, annotation_macro):
    with open(file_path, 'r', encoding='utf-8') as f:
        content = f.read()

    namespace_stack = extract_namespace_stack(content)
    class_pattern = re.compile(r'(?:class|struct)\s+\w+\s+' + annotation_macro + r'\s+(\w+)')
    constructor_pattern = re.compile(r'(\w+)\s*\((.*?)\)', re.DOTALL)

    registrations = []
    classes = list(class_pattern.finditer(content))
    if not classes:
        return [], []

    for i, class_match in enumerate(classes):
        class_name = class_match.group(1)
        line_number = content[:class_match.start()].count('\n')
        namespace = namespace_stack[line_number]
        qualified_class_name = f'{namespace}::{class_name}' if namespace else class_name

        start_pos = class_match.end()
        end_pos = classes[i + 1].start() if i + 1 < len(classes) else len(content)
        body_content = content[start_pos:end_pos]

        properties = parse_properties_with_access_control(body_content)

        constructors = []
        for ctor in constructor_pattern.finditer(body_content):
            ctor_name, param_str = ctor.groups()
            if ctor_name != class_name:
                continue
            params = [p.strip() for p in param_str.split(',') if p.strip()]
            param_types = [p.split()[-2] if ' ' in p else p for p in params]
            signature = ', '.join(param_types) if param_types else ''
            constructors.append(f'.constructor<{signature}>()' if signature else '.constructor<>()')

        registration = f'    registration::class_<{qualified_class_name}>("{class_name}")\n'
        for ctor in constructors:
            registration += f'        {ctor}\n'

        for prop in properties:
            access_flag = 'registration::private_access' if prop['access'] in ['private', 'protected'] else None
            if prop['default']:
                default_meta = f'metadata("default", "{prop["default"]}")'
                if access_flag:
                    registration += f'        .property("{prop["name"]}", &{qualified_class_name}::{prop["name"]}, {default_meta}, {access_flag})\n'
                else:
                    registration += f'        .property("{prop["name"]}", &{qualified_class_name}::{prop["name"]}, {default_meta})\n'
            else:
                if access_flag:
                    registration += f'        .property("{prop["name"]}", &{qualified_class_name}::{prop["name"]}, {access_flag})\n'
                else:
                    registration += f'        .property("{prop["name"]}", &{qualified_class_name}::{prop["name"]})\n'

        registration += '    ;\n'
        registrations.append(registration)

    return registrations, [file_path]


def process_directory(directory, annotation_macro):
    all_registrations, include_files = [], set()
    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith((".h", ".hpp")):
                file_path = os.path.join(root, file)
                regs, includes = extract_classes(file_path, annotation_macro)
                all_registrations.extend(regs)
                include_files.update(includes)
    return all_registrations, include_files


def generate_output(output_path, registrations, include_files):
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    with open(output_path, 'w', encoding='utf-8') as f:
        f.write(HEADER)
        for include in include_files:
            rel_path = os.path.relpath(include, os.path.dirname(output_path)).replace("\\", "/")
            f.write(f'#include "{rel_path}"\n')
        f.write('\n')
        if NAMESPACE_WRAP:
            f.write('namespace Luden\n{\n')
        f.write('RTTR_REGISTRATION\n{\n')
        for reg in registrations:
            f.write(reg + '\n')
        f.write('}\n')
        if NAMESPACE_WRAP:
            f.write('} // namespace Luden\n')
    print(f"\nGenerated {output_path}")


if __name__ == "__main__":
    output_file = 'Engine/src/Generated/Reflection.gen.cpp'
    all_regs, all_includes = [], set()
    for macro in ['COMPONENT\\(\\)', 'CLASS\\(\\)']:
        regs, includes = process_directory('Engine/include', macro)
        all_regs.extend(regs)
        all_includes.update(includes)

    if all_regs:
        generate_output(output_file, all_regs, all_includes)
    else:
        print("\nNo matching COMPONENT() or CLASS() classes found.")
