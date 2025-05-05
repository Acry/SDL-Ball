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
        text_complete = ''

        for part in parts:
            part = part.strip()
            if part.startswith('"'):
                text_complete += part.strip('"')
            elif part.endswith('"'):
                text_complete += part.strip('"')
            else:
                text_complete += '%s'
                variables.append(part)

        # Entferne überflüssige %s am Ende
        if text_complete.endswith('%s'):
            text_complete = text_complete[:-2]

        if variables:
            return f'SDL_Log("{text_complete}", {"".join(variables)});'
        else:
            return f'SDL_Log("{text_complete}");'

    updated_content = re.sub(pattern, replacer, content, flags=re.MULTILINE)

    with open(file_path, 'w') as file:
        file.write(updated_content)

    print(f"`cout` wurde in {file_path} durch `SDL_Log` ersetzt.")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Verwendung: python replace_cout_with_sdl_log.py <dateipfad>")
        sys.exit(1)

    dateipfad = sys.argv[1]
    replace_cout_with_sdl_log(dateipfad)