# Documentation and Guides
Here's where you can find additional guides to assist with programming for BURT.

## Examples
Examples for componets like the radio can be found [here](./EXAMPLES)

## Really Using BurtLib
The first thing you'll probably want to do is download the latest robot code. The setup varries depending on your prefered enviornment.

### Using the Arduino IDE
This is the offically supported method of writing projects for Arduino. It's relatively easy to setup.
1. Download the [Arduino IDE](https://www.arduino.cc/).
1. Download this repository from the home page.
2. Extract the files to your computer.
3. Copy the `BurtLib_2024` folder to where your Arduino libraries are kept. (If you're unsure, navigate to **File > Preferences > Sketchbook** from the IDE and find your sketchbook location*. Your `libraries` folder is kept there.)
4. Open the IDE and find the `.ino` project you want to use from the files you extracted.

*Verification needed.

### Using VS Code
VS Code is no longer offically support by Arduino, but a community extention still exists to make it work. This is a more involved process and not recommened if you're a beginner or just want to start coding right away.
1. Download [VS Code](https://code.visualstudio.com/) (if you haven't already).
2. Install the [Arduino Community Edition](https://marketplace.visualstudio.com/items?itemName=vscode-arduino.vscode-arduino-community) extension for VS Code.
4. (Optional) If you don't have the official Arduino IDE installed, install the [Arduino CLI](https://docs.arduino.cc/arduino-cli/) onto your computer. Also make sure to run `arduino-cli core update-index` and `arduino-cli core install arduino:avr` from your terminal.
5. Download this repository from the home page.
6. Extract the files to a known location.
7. Open `burt-programming` and copy `BurtLib_2024` into wherever you keep your Arduino libraries.
8. Open `burt-programming` in VS Code. Navigate to the project you want to use.
9. (Optional) If you need to change the code in `BurtLib_2024`, also open your Arduino libraries folder in VS Code. Select **Add Folder to Workspace**.
10. To run Arduino commands, press `f1` and type "Arduino". Run the command to select the board you're using (probably an Uno or Mega 2560). Finally run the command to select your sketch.
11. If all has gone well, run `Arduino: Verify` and the output box show run without error.
12. Last step is to connect your Arduino board via usb and run `Arduino: Select Serial Port` before running `Arduino: Upload`.

#### (Optional) Syntax Highlighting and Additional Features
If you want syntax highlighting and additioanl support writing C++, install the [C/C++ Extension Pack](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools-extension-pack). If you get red squiggles under code, make sure you add your Arduino library folder to the extension's **Include Path** box. Mine looks like this:
```
${workspaceFolder}/**
/home/olive/Arduino/**
/home/olive/.arduino15/packages/arduino/**
```
Keep in mind my system may be different from yours.

#### Saving Code to GitHub with Source Control
First off, make sure you have contrubitor push code to this repository. When you are ready, make sure you copy your `BurtLib_2024` library files from your Arduino library to the project you have opened in VS Code (usually overriding the contents there). Then select **Source Control** from the sidebar in VS Code. Stage files to commit, write an accurate and consiece message describing your changes, and hit **Commit**. The click **Sync** to push changes to GitHub.