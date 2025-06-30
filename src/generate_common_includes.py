import os
import re
from collections import Counter

# Omdat we vanuit src/ runnen:
SRC_DIR = os.path.abspath(os.getcwd())
COMMON_DIR = os.path.join(SRC_DIR, "common")
OUTPUT_FILE = os.path.join(COMMON_DIR, "common_includes.h")

INCLUDE_PATTERN = re.compile(r'#include\s+"([^"]+)"')

def find_includes():
    includes = []
    for root, _, files in os.walk(SRC_DIR):
        for fname in files:
            if fname.endswith((".h", ".cpp")):
                path = os.path.join(root, fname)
                with open(path, 'r', encoding='utf-8', errors='ignore') as f:
                    for line in f:
                        m = INCLUDE_PATTERN.search(line)
                        if m:
                            inc = m.group(1)
                            # Alleen relatieve includes
                            if not inc.startswith("<"):
                                includes.append(inc)
    return includes

def write_common_header(top_includes):
    os.makedirs(COMMON_DIR, exist_ok=True)
    with open(OUTPUT_FILE, 'w', encoding='utf-8') as f:
        f.write("// automatisch gegenereerd door generate_common_includes.py\n")
        f.write("#pragma once\n\n")
        for inc in sorted(top_includes):
            f.write(f'#include "{inc}"\n')
    print(f"\n✅ common_includes.h aangemaakt in: {COMMON_DIR}")

def main():
    print(f"🔍 Scannen van alle .cpp/.h in {SRC_DIR} ...")
    includes = find_includes()
    counts = Counter(includes)
    most_common = counts.most_common(20)  # aantal regels aanpassen indien gewenst

    print("📊 Top gebruikte includes:")
    for inc, cnt in most_common:
        print(f"  {inc:40} — {cnt} keer")

    top_list = [inc for inc, _ in most_common]
    write_common_header(top_list)

if __name__ == "__main__":
    main()
