# Chip-8 Emulator
This repository contains the C implementation of the Chip-8 emulator. The project was done purely for fun and to improve my programming abilities with C. In the future,
I may improve this emulator further by extending the implementation to cover the additional features of the Super Chip-8 emulator. To implement this emulator I followed
Cowgod's technical reference document which can be found [here](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM).

<p align="center">
  <img src="https://thumbs.gfycat.com/GleamingColorfulHagfish-size_restricted.gif" />
</p>

# Running the Program

To run the file simply download and extract the contents of the bin file which contains the **_main.exe_** executable. To run the executable you will need to provide a game (ROM)
for the program to execute. To do this you can extract a ROM of your choosing to the bin folder of the project, open the console and type the following command:

```bash
./main.exe ./YOUR_ROM
```

Executing this command will initate the programme and begin to draw your ROM to the screen. If you wish to modify the code you will need to remake the contents of the bin directory.
As the MakeFile is included with this programme you do not need to modify this file. You will only need to modify the contents of the MakeFile if you plan on adding additional C
files to the programmes directory.
