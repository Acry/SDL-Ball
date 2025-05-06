#!/usr/bin/env python3
import os
import re
import json
from collections import defaultdict


def find_classes(directory='.'):
    classes = defaultdict(list)
    class_pattern = re.compile(r'class\s+(\w+)(?:\s*:\s*(?:public|private|protected)?\s*(\w+))?')

    # Absoluten Pfad des Verzeichnisses ermitteln
    abs_directory = os.path.abspath(directory)

    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith(('.cpp', '.hpp', '.h')):
                rel_path = os.path.join(root, file)
                abs_path = os.path.abspath(rel_path)  # Absoluten Pfad erstellen
                try:
                    with open(abs_path, 'r', encoding='utf-8') as f:
                        content = f.read()
                        matches = class_pattern.finditer(content)
                        for match in matches:
                            class_name = match.group(1)
                            base_class = match.group(2)
                            classes[abs_path].append({  # Absoluten Pfad als Schlüssel verwenden
                                'name': class_name,
                                'base': base_class
                            })
                except UnicodeDecodeError:
                    print(f"Fehler beim Lesen von {abs_path}")
    return classes

if __name__ == '__main__':
    print("Suche nach C++ Klassen...")
    classes = find_classes('../src')

    # JSON-Datei speichern
    with open('classes_viz/data/classes.json', 'w') as f:
        json.dump(dict(classes), f)

    print("JSON-Daten wurden in classes.json gespeichert")
