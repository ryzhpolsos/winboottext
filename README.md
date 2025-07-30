# WinBootText
WinBootText is a tiny utility for Windows to display custom text on the boot screen. It uses Native API and BootExecute to show messages during boot process.

<details>
  <summary>Screenshot</summary>

  ![Screenshot](https://raw.githubusercontent.com/ryzhpolsos/winboottext/refs/heads/main/winboottext.png)

</details>

## Installation
1. Go to [releases page](https://github.com/ryzhpolsos/winboottext/releases) and download `winboottext.zip` file from the latest release
2. Extract the archive to any folder
3. Run `install.bat` as administrator
4. Reboot the system. If the installation was successful, you will see "Hello from winboottext!" on the boot screen.

## Configuration
1. Run `config.bat` as administrator to open GUI configuration utility
2. Make changes and click "Apply" to update the configuration.

## Uninstallation
1. Run `uninstall.bat` as administrator.

## Building
1. Make sure you have Visual Studio and Git installed
2. Open "x64 Native Tools Command Prompt for VS <version>" from the start menu and navigate to the folder where you want to place winboottext
3. Clone the repository: `git clone https://github.com/ryzhpolsos/winboottext`
4. Install all dependencies and run first build: `build /d`
5. Use `build` to re-build winboottext without dependency installation.

## License
This project is licensed under [the MIT license](https://github.com/ryzhpolsos/winboottext/blob/main/LICENSE).
