#! /bin/bash

# Create a read-only disk image of the contents of a folder;
# based on https://github.com/andreyvit/create-dmg, MIT license

set -e;

function pure_version() {
  echo '1.0.0.2'
}

function version() {
  echo "create-dmg $(pure_version)"
}

function usage() {
  version
  echo "Creates a fancy DMG file."
  echo "Usage:  $(basename $0) options... image.dmg source_folder"
  echo "All contents of source_folder will be copied into the disk image."
  echo "Options:"
  echo "  --volname name"
  echo "      set volume name (displayed in the Finder sidebar and window title)"
  echo "  --volicon icon.icns"
  echo "      set volume icon"
  echo "  --app-drop-link x y"
  echo "      make a drop link to Applications, at location x,y"
  echo "  --eula eula_file"
  echo "      attach a license file to the dmg"
  echo "  --no-internet-enable"
  echo "      disable automatic mount&copy"
  echo "  --version         show tool version number"
  echo "  -h, --help        display this help"
  exit 0
}

while test "${1:0:1}" = "-"; do
  case $1 in
    --volname)
      VOLUME_NAME="$2"
      shift; shift;;
    --volicon)
      VOLUME_ICON_FILE="$2"
      shift; shift;;
    -h | --help)
      usage;;
    --version)
      version; exit 0;;
    --pure-version)
      pure_version; exit 0;;
    --app-drop-link)
      APPLICATION_LINK=$2
      shift; shift; shift;;
    --eula)
      EULA_RSRC=$2
      shift; shift;;
    --no-internet-enable)
      NOINTERNET=1
      shift;;
    -*)
      echo "Unknown option $1. Run with --help for help."
      exit 1;;
  esac
done

test -z "$2" && {
  echo "Not enough arguments. Invoke with --help for help."
  exit 1
}

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
DMG_PATH="$1"
DMG_DIRNAME="$(dirname "$DMG_PATH")"
DMG_DIR="$(cd "$DMG_DIRNAME" > /dev/null; pwd)"
DMG_NAME="$(basename "$DMG_PATH")"
DMG_TEMP_NAME="$DMG_DIR/rw.${DMG_NAME}"
SRC_FOLDER="$(cd "$2" > /dev/null; pwd)"
test -z "$VOLUME_NAME" && VOLUME_NAME="$(basename "$DMG_PATH" .dmg)"

# Create the image
echo "Creating disk image..."
test -f "${DMG_TEMP_NAME}" && rm -f "${DMG_TEMP_NAME}"
ACTUAL_SIZE=`du -sm "$SRC_FOLDER" | sed -e 's/	.*//g'`
DISK_IMAGE_SIZE=$(expr $ACTUAL_SIZE + 20)
hdiutil create -srcfolder "$SRC_FOLDER" -volname "${VOLUME_NAME}" -fs HFS+ -fsargs "-c c=64,a=16,e=16" -format UDRW -size ${DISK_IMAGE_SIZE}m "${DMG_TEMP_NAME}"

# mount it
echo "Mounting disk image..."
MOUNT_DIR="/Volumes/${VOLUME_NAME}"

# try unmount dmg if it was mounted previously (e.g. developer mounted dmg, installed app and forgot to unmount it)
echo "Unmounting disk image..."
DEV_NAME=$(hdiutil info | egrep '^/dev/' | sed 1q | awk '{print $1}')
test -d "${MOUNT_DIR}" && hdiutil detach "${DEV_NAME}"

echo "Mount directory: $MOUNT_DIR"
DEV_NAME=$(hdiutil attach -readwrite -noverify -noautoopen "${DMG_TEMP_NAME}" | egrep '^/dev/' | sed 1q | awk '{print $1}')
echo "Device name:     $DEV_NAME"

if ! test -z "$APPLICATION_LINK"; then
  echo "making link to Applications dir"
  echo $MOUNT_DIR
  ln -s /Applications "$MOUNT_DIR/Applications"
fi

if ! test -z "$VOLUME_ICON_FILE"; then
  echo "Copying volume icon file '$VOLUME_ICON_FILE'..."
  cp "$VOLUME_ICON_FILE" "$MOUNT_DIR/.VolumeIcon.icns"
  SetFile -c icnC "$MOUNT_DIR/.VolumeIcon.icns"
fi

# make sure it's not world writeable
echo "Fixing permissions..."
chmod -Rf go-w "${MOUNT_DIR}" &> /dev/null || true
echo "Done fixing permissions."

# make the top window open itself on mount:
echo "Blessing started"
bless --folder "${MOUNT_DIR}" --openfolder "${MOUNT_DIR}"
echo "Blessing finished"

if ! test -z "$VOLUME_ICON_FILE"; then
   # tell the volume that it has a special file attribute
   SetFile -a C "$MOUNT_DIR"
fi

# unmount
echo "Unmounting disk image..."
hdiutil detach "${DEV_NAME}"

# compress image
echo "Compressing disk image..."
hdiutil convert "${DMG_TEMP_NAME}" -format UDZO -imagekey zlib-level=9 -o "${DMG_DIR}/${DMG_NAME}"
rm -f "${DMG_TEMP_NAME}"

if [ ! -z "${NOINTERNET}" -a "${NOINTERNET}" == 1 ]; then
        echo "not setting 'internet-enable' on the dmg"
else
        hdiutil internet-enable -yes "${DMG_DIR}/${DMG_NAME}"
fi

echo "Disk image done"
exit 0
