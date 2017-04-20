Creating a "DMG installer" for OS X
======
A DMG Installer is convenient way to provide end-users a simple way to install
an application bundle.  They are basically a folder with a shortcut to the Applications
directory but they can be customized with icons, backgrounds, and layout properties.
A DMG file (.dmg) is a Mac OS X Disk Image file and it is used to package files or folders
providing compression, encryption, and read-only to the package.

##Creating the DMG file
#Disk Utility
Type `Disk Utility` into Spotlite __CMD+SPACEBAR__ from Finder
or from Terminal:
```
open /Applications/Utilities/Disk\ Utility.app/
```

__File__ -> __New__ -> __Disk Image from Folder__
or __CMD+SHIFT+N__

Select the folder `myapp.app/` when prompted then click `Image`.

In the `Save As` field enter a name for the file like `myapp.dmg`.

From the `Image Format` drop-down select `read/write` then click `Save`.

#Edit Folder Preferences
Mount the dmg and open it by double-clicking the file in Finder
or from Terminal:
```
open myapp.dmg
open /Volumes/myapp/
```

Create a link/shortcut to /Applications folder by right-clicking
on the `Applications` folder and selecting `Make Alias` then drag it into
the dmg folder or from Terminal:
```
cd /Volumes/myapp/
ln -s /Applications Applications
```

In Finder press __CMD+1__ to switch to icon view and arrange icons as needed.

Press __CMD+J__ to show the View Options window and
adjust view settings as needed.

From `Background:` section choose `Picture` then
Drag and drop the image you want to use as the
background where it says `Drag image here`.

Unmount/Eject the dmg when finished with View Options
or from Terminal:
```
umount /Volumes/myapp/
```

#Compress and convert to Read-only
From Disk Utility right-click on myapp.dmg disk image and
select `Convert "myapp.dmg"`.

In the `Save As` field enter a new name for the file like `myappfinal.dmg`.

From the `Image Formate` drop-down select `read-only` then click `Save`
or from Terminal:
```
hdiutil convert -format UDZO -o myappfinal.dmg myapp.dmg
```

#FIN
Congratulations you are finished!  Mount the new dmg to verify it is working properly.
