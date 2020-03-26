
from pathlib import Path
from shutil import copyfile

COMMON_FOLDER = "common"
GENERAL_FOLDERS = ["esp", "arduino"]

print("Copying files from '{}' folder to [{}] folders..."
      .format(COMMON_FOLDER, ', '.join(GENERAL_FOLDERS)))

for obj in Path("./{}".format(COMMON_FOLDER)).glob("*"):
    if not obj.is_file():
        continue
    
    for folder in GENERAL_FOLDERS:
        copyfile(str(obj), './{}/{}'.format(folder, obj.name))

print("Done!")
