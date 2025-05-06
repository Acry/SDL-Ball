#!/usr/bin/env python3
import os
import re

def find_classes(directory='.'):
    # Erkennt "class Name" und "class Name : Base" Muster
    class_pattern = re.compile(r'class\s+(\w+)(?:\s*:\s*(?:public|private|protected)?\s*(\w+))?')

    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith(('.cpp', '.hpp', '.h')):
                file_path = os.path.join(root, file)
                try:
                    with open(file_path, 'r', encoding='utf-8') as f:
                        content = f.read()
                        matches = class_pattern.finditer(content)
                        for match in matches:
                            class_name = match.group(1)
                            base_class = match.group(2)
                            print(f"Datei: {file_path}")
                            if base_class:
                                print(f"  Klasse: {class_name} erbt von {base_class}")
                            else:
                                print(f"  Klasse: {class_name}")
                except UnicodeDecodeError:
                    print(f"Fehler beim Lesen von {file_path}")

if __name__ == '__main__':
    print("Suche nach C++ Klassen...")
    find_classes('../src')
