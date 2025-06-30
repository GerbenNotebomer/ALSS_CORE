import os
import shutil

SRC_FILE = "translations.json"
DST_DIR = "../data/lang/"
DST_FILE = os.path.join(DST_DIR, "translations.json")

def ensure_folder(path):
    if not os.path.exists(path):
        os.makedirs(path)
        print(f"📁 Map aangemaakt: {path}")

def deploy_translation():
    ensure_folder(DST_DIR)
    
    if not os.path.exists(SRC_FILE):
        print("❌ Bronbestand niet gevonden:", SRC_FILE)
        return
    
    shutil.copyfile(SRC_FILE, DST_FILE)
    print(f"✅ {SRC_FILE} gekopieerd naar {DST_FILE}")

if __name__ == "__main__":
    deploy_translation()
