import os
import shutil

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.abspath(os.path.join(BASE_DIR, "..", "..", ".."))
DST_DIR = os.path.join(PROJECT_ROOT, "data", "lang")

SRC_WEB = os.path.join(BASE_DIR, "translations.json")
SRC_APP = os.path.join(BASE_DIR, "translations_app.json")

def ensure_folder(path):
    if not os.path.exists(path):
        os.makedirs(path)
        print(f"📁 Map aangemaakt: {path}")

def deploy():
    ensure_folder(DST_DIR)

    for file in [SRC_WEB, SRC_APP]:
        if not os.path.exists(file):
            print("❌ Ontbrekend bestand:", file)
            continue

        dst_file = os.path.join(DST_DIR, os.path.basename(file))
        shutil.copyfile(file, dst_file)
        print(f"✅ {file} gekopieerd naar {dst_file}")

if __name__ == "__main__":
    deploy()
