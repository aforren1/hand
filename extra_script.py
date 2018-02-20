import os
import shutil
Import('env')

FRAMEWORK_DIR = env.PioPlatform().get_package_dir("framework-arduinoteensy")
path_to_desc = os.path.join(os.sep, 'cores', 'teensy3')
full_path = FRAMEWORK_DIR + path_to_desc + os.sep

def swap_origin_usb_desc(source, target, env):
    print('Renaming usb_desc.h for now...')
    os.rename(full_path + 'usb_desc.h', full_path + 'dontuse.xx')
    print('Move our confederate into the right spot')
    shutil.copyfile('include/usb_desc.h', full_path + 'usb_desc.h')

def unswap_origin_usb_desc(source, target, env):
    print('Naming back...')
    os.remove(full_path + 'usb_desc.h')
    os.rename(full_path + 'dontuse.xx', full_path + 'usb_desc.h')
    

env.AddPreAction("$BUILD_DIR/firmware.elf", swap_origin_usb_desc)
env.AddPostAction("$BUILD_DIR/firmware.elf", unswap_origin_usb_desc)
