import shutil
import os

def copy_firmware(source, target, env):
    src = target[0].get_path()

    # Haal de project root op
    workspace_dir = env['PROJECT_DIR']

    # Haal de naam van de workspace (laatste mapdeel van het pad)
    workspace_name = os.path.basename(workspace_dir.rstrip("/"))

    # Stel de 'firmware' directory in binnen de workspace
    firmware_dir = os.path.join(workspace_dir, "firmware")

    # Maak de directory aan als die nog niet bestaat
    if not os.path.exists(firmware_dir):
        os.makedirs(firmware_dir)
        print(f"[OTA] Directory '{firmware_dir}' aangemaakt.")

    # Bestandsnaam: firmware_<workspacenaam>.bin
    dst_filename = f"firmware_{workspace_name}.bin"
    dst = os.path.join(firmware_dir, dst_filename)

    print(f"[OTA] Kopieer firmware: {src} → {dst}")
    shutil.copy(src, dst)

Import("env")
env.AddPostAction("$BUILD_DIR/firmware.bin", copy_firmware)
