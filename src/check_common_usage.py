import os
import re

# Zet hier het pad naar je src-map (of run dit script vanuit src en gebruik ".")
SRC_DIR = os.path.abspath(os.getcwd())
COMMON_HEADER = os.path.join(SRC_DIR, "common", "common_includes.h")

INCLUDE_PATTERN = re.compile(r'#include\s+"([^"]+)"')

def load_common_includes():
    includes = []
    with open(COMMON_HEADER, 'r', encoding='utf-8') as f:
        for line in f:
            m = INCLUDE_PATTERN.search(line)
            if m:
                includes.append(m.group(1))
    return includes

def find_usage(includes):
    usage = {inc: [] for inc in includes}
    for root, _, files in os.walk(SRC_DIR):
        for fname in files:
            if fname.endswith((".h", ".cpp")):
                path = os.path.join(root, fname)
                with open(path, 'r', encoding='utf-8', errors='ignore') as f:
                    text = f.read()
                    for inc in includes:
                        # exact match van include
                        if f'#include "{inc}"' in text:
                            rel = os.path.relpath(path, SRC_DIR)
                            usage[inc].append(rel)
    return usage

def main():
    if not os.path.exists(COMMON_HEADER):
        print(f"❌ Kan common header niet vinden: {COMMON_HEADER}")
        return

    print(f"🔍 Lezen van common_includes.h uit {COMMON_HEADER}")
    includes = load_common_includes()

    print(f"🔍 Scannen op gebruik in .cpp/.h …")
    usage = find_usage(includes)

    print("\n📊 Usage per include:")
    for inc, files in usage.items():
        print(f"\n▶ {inc} ({len(files)} keer):")
        for fn in sorted(files):
            print(f"   - {fn}")

if __name__ == "__main__":
    main()
