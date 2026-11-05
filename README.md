# STM32 template

## Install CubeIDE

1. Download STM CubeIDE (it's necessary to generate and compile your code, and after send it to your board). You can download it [here](https://www.st.com/en/development-tools/stm32cubeide#section-get-software-table).

2. Aside, create a MyST account, otherwise your IDE won't be able to download important file (such as firmware) required by your project

3. Install the IDE:

```sh
# Uncompress the file
unzip stm32cubeide*.zip

# Make it executable
chmod +x st-stm32cubeide*.sh

# Make the installation¹
sudo sh -c ./stm32cubeide*.sh
```
¹ Note: you'll have to:
    (1) accept the licenses
    (2) also install Segger J-Link udev rules

<!--
## Install STM32CubeMX (optional)
If you want to edit STM pins (or edit the .ioc), you'll have to installation the STM32CubeMX:

1. Download it [here](https://www.st.com/en/development-tools/stm32cubemx.html).

2. Install:

```sh
unzip SetupSTM32CubeMX*.zip

chmod +x SetupSTM32CubeMX*
```
-->
