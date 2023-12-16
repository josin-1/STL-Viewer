# STL Viewer
This is a very simple STL-File Viewer, using the [SDL](https://github.com/libsdl-org/SDL) library. <br />
It's my first "bigger" project, and my first project using git.

<br /><br />
> [!NOTE]
> This Viewer only supports binary STL Files for now.

To choose a file, the argument of model_load() in [main.c](https://github.com/josin-1/STL-Viewer/blob/master/STL%20Viewer/main.c): line 34, has to be changed:
``` C
model = model_load("file.stl");
```

<br />

## Known Bugs:
- [ ] The window close button (x) doesnt work!
