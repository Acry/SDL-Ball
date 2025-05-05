import re
import sys

def replace_cout_with_sdl_log(file_path):
    with open(file_path, 'r') as file:
        content = file.read()

    pattern = r'cout\s*<<\s*(.*?);'

    def replacer(match):
        inner_content = match.group(1)
        inner_content = re.sub(r'\s*endl\s*', '', inner_content)
        parts = re.split(r'\s*<<\s*', inner_content)

        message = ''
        variables = []

        for part in parts:
            part = part.strip()
            if part.startswith('"'):
                message += part.strip('"')
            elif part.endswith('"'):
                message += part.strip('"')
            elif part:
                variables.append(part)
                message = message.rstrip() + '%s'

        if variables:
            return f'SDL_Log("{message}", {", ".join(variables)});'
        else:
            return f'SDL_Log("{message}");'

    updated_content = re.sub(pattern, replacer, content, flags=re.MULTILINE)

    with open(file_path, 'w') as file:
        file.write(updated_content)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Verwendung: python replace_cout_with_sdl_log.py <dateipfad>")
        sys.exit(1)

    dateipfad = sys.argv[1]
    replace_cout_with_sdl_log(dateipfad)
