# Copyright 2014-present PlatformIO <contact@platformio.org>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Extended and rewritten by Maximilian Gerhardt <maximilian.gerhardt@rub.de>
# for Bouffalo BL602 core.

"""
Arduino

Arduino Wiring-based Framework allows writing cross-platform software to
control devices attached to a wide range of Arduino boards to create all
kinds of creative coding, interactive objects, spaces or physical experiences.

https://github.com/pine64/ArduinoCore-bouffalo
"""

import json
from os.path import isfile, isdir, join

from platformio.util import get_systype

from SCons.Script import COMMAND_LINE_TARGETS, DefaultEnvironment

env = DefaultEnvironment()
platform = env.PioPlatform()
board_config = env.BoardConfig()
board_name = env.subst("$BOARD")

FRAMEWORK_DIR = platform.get_package_dir("framework-arduinobouffalo")
assert isdir(FRAMEWORK_DIR)

BL_IOT_SDK_DIR = platform.get_package_dir("framework-bl-iot-sdk-arduino")
assert isdir(BL_IOT_SDK_DIR)

mcu = board_config.get("build.mcu", "")
variant = board_config.get(
    "build.variant", board_config.get("build.arduino.variant", ""))
assert variant != ""

variants_dir = (
    join("$PROJECT_DIR", board_config.get("build.variants_dir"))
    if board_config.get("build.variants_dir", "")
    else join(FRAMEWORK_DIR, "variants")
)
variant_dir = join(variants_dir, variant)
inc_variant_dir = variant_dir
if "windows" not in get_systype().lower() and not (
    set(["_idedata", "idedata"]) & set(COMMAND_LINE_TARGETS) and " " not in variant_dir
):
    inc_variant_dir = variant_dir.replace("(", r"\(").replace(")", r"\)")

upload_protocol = env.subst("$UPLOAD_PROTOCOL")

def get_arduino_board_id(board_config, mcu):
    if env.subst("$BOARD") == "pinecone":
        return "PINECONE_EVB"

board_id = get_arduino_board_id(board_config, mcu)

# from platform.txt
sdk = {"version": "release_bl_iot_sdk_1.6.11-1-g66bb28da-dirty",
       "phy_ver": "a0_final-44-geb7fadd", "rf_ver": "f76e39a"}

env.Append(
    ASFLAGS=["-x", "assembler-with-cpp"],
    CFLAGS=["-std=gnu99"],
    CXXFLAGS=[
        "-std=gnu++11",
        "-fno-use-cxa-atexit", 
        "-nostdlib", 
        "-Wpointer-arith", 
        "-fexceptions", 
        "-fstack-protector", 
        "-fno-rtti", 
        "-fno-exceptions", 
        "-fms-extensions", 
        "-Werror=return-type"
    ],
    CCFLAGS=[
        "-Os",  # optimize for size
        "-Wall",  # show warnings
        "-march=%s" % board_config.get("build.march"),
        "-mabi=%s" % board_config.get("build.mabi"),

        "-gdwarf", 
        "-ffunction-sections",
        "-fdata-sections",
        "-fstrict-volatile-bitfields",
        "-fshort-enums",
        "-ffreestanding",
        "-fno-strict-aliasing",
        "--param",
        "max-inline-insns-single=500",
    ],
    CPPDEFINES=[
        ("ARDUINO", 10808),
        ("F_CPU", "$BOARD_F_CPU"),
        "ARDUINO_%s" % board_id,
        "ARDUINO_BOUFFALO",
        "BL602",
        "CONF_USER_ENABLE_PSRAM",
        "configUSE_TICKLESS_IDLE=0",
        ("FEATURE_WIFI_DISABLE", 1),
        "CFG_FREERTOS",
        "ARCH_RISCV",
        "BL602",
        "CONFIG_SET_TX_PWR",
        "CFG_BLE_ENABLE",
        "BFLB_BLE",
        "CFG_BLE",
        "CFG_SLEEP",
        "OPTIMIZE_DATA_EVT_FLOW_FROM_CONTROLLER",
        ("CFG_CON", 2),
        ("CFG_BLE_TX_BUFF_DATA", 2),
        "CONFIG_BT_ALLROLES",
        "CONFIG_BT_CENTRAL",
        "CONFIG_BT_OBSERVER",
        "CONFIG_BT_PERIPHERAL",
        "CONFIG_BT_BROADCASTER",
        "CONFIG_BT_L2CAP_DYNAMIC_CHANNEL",
        "CONFIG_BT_GATT_CLIENT",
        "CONFIG_BT_CONN",
        "CONFIG_BT_GATT_DIS_PNP",
        "CONFIG_BT_GATT_DIS_SERIAL_NUMBER",
        "CONFIG_BT_GATT_DIS_FW_REV",
        "CONFIG_BT_GATT_DIS_HW_REV",
        "CONFIG_BT_GATT_DIS_SW_REV",
        "CONFIG_BT_ECC",
        "CONFIG_BT_GATT_DYNAMIC_DB",
        "CONFIG_BT_GATT_SERVICE_CHANGED",
        "CONFIG_BT_KEYS_OVERWRITE_OLDEST",
        "CONFIG_BT_KEYS_SAVE_AGING_COUNTER_ON_P",
        "CONFIG_BT_GAP_PERIPHERAL_PREF_PARAMS",
        "CONFIG_BT_BONDABLE",
        "CONFIG_BT_HCI_VS_EVT_USER",
        "CONFIG_BT_ASSERT",
        "CONFIG_BT_SMP",
        "CONFIG_BT_SIGNING",
        "CONFIG_BT_SETTINGS_CCC_LAZY_LOADING",
        "CONFIG_BT_SETTINGS_USE_PRINTK",
        "CFG_BLE_STACK_DBG_PRINT",
        ("BL_SDK_VER", f"{sdk['version']}"),
        ("BL_SDK_PHY_VER", f"{sdk['phy_ver']}"),
        ("BL_SDK_RF_VER", f"{sdk['rf_ver']}"),
        ("BL_CHIP_NAME", "BL602"),
        "ARCH_RISCV",
        ("CONFIG_PSM_EASYFLASH_SIZE", 16384),
        ("configUSE_TICKLESS_IDLE", 0),
        ("FEATURE_WIFI_DISABLE", 1),
        "CFG_BLE_ENABLE",
        "CONF_USER_ENABLE_PSRAM",
        "CONF_USER_ENABLE_VFS_ROMFS",
        ("CFG_COMPONENT_BLOG_ENABLE", 0)
    ],
    CPPPATH=[
        join(FRAMEWORK_DIR, "cores", "bl602"),
        join(BL_IOT_SDK_DIR, "include", "bl602", "bl602"),
        join(BL_IOT_SDK_DIR, "include", "bl602", "bl602", "include"), 
        join(BL_IOT_SDK_DIR, "include", "bl602", "bl602_std"),
        join(BL_IOT_SDK_DIR, "include", "bl602", "bl602_std", "include"),
        join(BL_IOT_SDK_DIR, "include", "bl602", "bl602_std", "bl602_std", "StdDriver", "Inc"),
        join(BL_IOT_SDK_DIR, "include", "bl602", "bl602_std", "bl602_std", "Device", "Bouffalo", "BL602", "Peripherals"),
        join(BL_IOT_SDK_DIR, "include", "bl602", "bl602_std", "bl602_std", "RISCV", "Device", "Bouffalo", "BL602", "Startup"),  
        join(BL_IOT_SDK_DIR, "include", "bl602", "bl602_std", "bl602_std", "RISCV", "Core", "Include"), 
        join(BL_IOT_SDK_DIR, "include", "bl602", "bl602_std", "bl602_std", "Include"), 
        join(BL_IOT_SDK_DIR, "include", "bl602", "bl602_std", "bl602_std", "Common", "platform_print"), 
        join(BL_IOT_SDK_DIR, "include", "bl602", "bl602_std", "bl602_std", "Common", "soft_crc"), 
        join(BL_IOT_SDK_DIR, "include", "bl602", "bl602_std", "bl602_std", "Common", "partition"), 
        join(BL_IOT_SDK_DIR, "include", "bl602", "bl602_std", "bl602_std", "Common", "xz"), 
        join(BL_IOT_SDK_DIR, "include", "bl602", "bl602_std", "bl602_std", "Common", "cipher_suite/inc"), 
        join(BL_IOT_SDK_DIR, "include", "bl602", "bl602_std", "bl602_std", "Common", "ring_buffer"), 
        join(BL_IOT_SDK_DIR, "include", "stage", "blfdt"), 
        join(BL_IOT_SDK_DIR, "include", "stage", "blfdt", "include"), 
        join(BL_IOT_SDK_DIR, "include", "stage", "blfdt", "inc"), 
        join(BL_IOT_SDK_DIR, "include", "sys", "blmtd"), 
        join(BL_IOT_SDK_DIR, "include", "sys", "blmtd", "include"), 
        join(BL_IOT_SDK_DIR, "include", "sys", "blmtd", "include"), 
        join(BL_IOT_SDK_DIR, "include", "stage", "blog"), 
        join(BL_IOT_SDK_DIR, "include", "stage", "blog", "include"), 
        join(BL_IOT_SDK_DIR, "include", "stage", "blog"), 
        join(BL_IOT_SDK_DIR, "include", "stage", "blog_testc"), 
        join(BL_IOT_SDK_DIR, "include", "stage", "blog_testc", "include"), 
        join(BL_IOT_SDK_DIR, "include", "stage", "blog_testc"), 
        join(BL_IOT_SDK_DIR, "include", "sys", "bloop", "bloop"), 
        join(BL_IOT_SDK_DIR, "include", "sys", "bloop", "bloop", "include"), 
        join(BL_IOT_SDK_DIR, "include", "sys", "bloop", "bloop", "include"), 
        join(BL_IOT_SDK_DIR, "include", "sys", "bltime"), 
        join(BL_IOT_SDK_DIR, "include", "sys", "bltime", "include"), 
        join(BL_IOT_SDK_DIR, "include", "sys", "bltime", "include"), 
        join(BL_IOT_SDK_DIR, "include", "stage", "cli"), 
        join(BL_IOT_SDK_DIR, "include", "stage", "cli", "include"), 
        join(BL_IOT_SDK_DIR, "include", "stage", "cli", "cli", "include"), 
        join(BL_IOT_SDK_DIR, "include", "bl602", "freertos_riscv_ram"), 
        join(BL_IOT_SDK_DIR, "include", "bl602", "freertos_riscv_ram", "include"), 
        join(BL_IOT_SDK_DIR, "include", "bl602", "freertos_riscv_ram", "config"), 
        join(BL_IOT_SDK_DIR, "include", "bl602", "freertos_riscv_ram", "portable", "GCC", "RISC-V"), 
        join(BL_IOT_SDK_DIR, "include", "bl602", "freertos_riscv_ram", "portable", "GCC", "RISC-V", "chip_specific_extensio), ", "RV32F_float_abi_single"), 
        join(BL_IOT_SDK_DIR, "include", "bl602", "freertos_riscv_ram", "panic"), 
        join(BL_IOT_SDK_DIR, "include", "hal_drv"), 
        join(BL_IOT_SDK_DIR, "include", "hal_drv", "include"), 
        join(BL_IOT_SDK_DIR, "include", "hal_drv", "bl602_hal"), 
        join(BL_IOT_SDK_DIR, "include", "sys", "bloop", "looprt"), 
        join(BL_IOT_SDK_DIR, "include", "sys", "bloop", "looprt", "include"), 
        join(BL_IOT_SDK_DIR, "include", "sys", "bloop", "loopset"), 
        join(BL_IOT_SDK_DIR, "include", "sys", "bloop", "loopset", "include"), 
        join(BL_IOT_SDK_DIR, "include", "fs", "romfs"), 
        join(BL_IOT_SDK_DIR, "include", "fs", "romfs", "include"), 
        join(BL_IOT_SDK_DIR, "include", "utils"), 
        join(BL_IOT_SDK_DIR, "include", "utils", "include"), 
        join(BL_IOT_SDK_DIR, "include", "fs", "vfs"), 
        join(BL_IOT_SDK_DIR, "include", "fs", "vfs", "include"), 
        join(BL_IOT_SDK_DIR, "include", "fs", "vfs", "posix", "include"), 
        join(BL_IOT_SDK_DIR, "include", "stage", "yloop"), 
        join(BL_IOT_SDK_DIR, "include", "stage", "yloop", "include"), 
    ], 
    LINKFLAGS=[
        "-Os",
        "-march=%s" % board_config.get("build.march"),
        "-mabi=%s" % board_config.get("build.mabi"),
        "--specs=nano.specs",
        "-nostartfiles",
        "-Wl,--gc-sections,--relax",
        "-Wl,--check-sections",
        "-Wl,--entry=Reset_Handler",
        "-Wl,--unresolved-symbols=report-all",
        "-Wl,--warn-common",
        "-Wl,--defsym=LD_MAX_SIZE=%d" % board_config.get("upload.maximum_size"),
        "-Wl,--defsym=LD_MAX_DATA_SIZE=%d" % board_config.get("upload.maximum_ram_size"),
    ],
    LIBS=[
        "c",
        "m",
        "gcc",
        "stdc++",
        "bl602", 
        "bl602_std", 
        "blfdt", 
        "blmtd", 
        "blog", 
        "blog_testc", 
        "bloop", 
        "bltime", 
        "cli", 
        "freertos_riscv_ram", 
        "hal_drv", 
        "looprt", 
        "loopset", 
        "romfs", 
        "utils", 
        "vfs",
    ],
    LIBPATH=[
        join(BL_IOT_SDK_DIR, "lib"),
        join(BL_IOT_SDK_DIR, "ld"),
    ]
)

env.ProcessFlags(board_config.get("build.framework_extra_flags.arduino", ""))

#
# Linker requires preprocessing with correct RAM|ROM sizes
#

if not board_config.get("build.ldscript", ""):
    #env.Replace(LDSCRIPT_PATH=join(FRAMEWORK_DIR, "system", "ldscript.ld"))
    if not isfile(join(env.subst(variant_dir), "ldscript.ld")):
        print("Warning! Cannot find linker script for the current target!\n")
    env.Append(
        LINKFLAGS=[
            (
                "-Wl,--default-script",
                join(
                    inc_variant_dir,
                    board_config.get("build.arduino.ldscript", "ldscript.ld"),
                ),
            )
        ]
    )

#
# Process configuration flags
#

cpp_defines = env.Flatten(env.get("CPPDEFINES", []))

# copy CCFLAGS to ASFLAGS (-x assembler-with-cpp mode)
env.Append(ASFLAGS=env.get("CCFLAGS", [])[:])

env.Append(
    LIBSOURCE_DIRS=[
        join(FRAMEWORK_DIR, "libraries"),
    ]
)

#
# Target: Build Core Library
#

libs = []

if "build.variant" in board_config:
    env.Append(
        CPPPATH=[inc_variant_dir],
        LIBPATH=[inc_variant_dir]
    )
    env.BuildSources(join("$BUILD_DIR", "FrameworkArduinoVariant"), variant_dir)

libs.append(env.BuildLibrary(
    join("$BUILD_DIR", "FrameworkArduino"), join(FRAMEWORK_DIR, "cores", "bl602")
))

env.Prepend(LIBS=libs)
