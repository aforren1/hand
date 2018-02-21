import os
import shutil
Import('env')

framework_dir = env.PioPlatform().get_package_dir("framework-arduinoteensy")
path_to_desc = os.path.join(os.sep, 'cores', 'teensy3')
full_path = framework_dir + path_to_desc + os.sep

def swap_origin_usb_desc(source, target, env):
    print("Trying to swap usb_desc.h")
    if not os.path.isfile(full_path + '_usb_desc.h'): # if the temp version doesn't already exist
        print('Renaming usb_desc.h for now...')
        os.rename(full_path + 'usb_desc.h', full_path + '_usb_desc.h')
        print('Move our confederate into the right spot')
        shutil.copyfile('include/usb_desc.h', full_path + 'usb_desc.h')

def unswap_origin_usb_desc(source, target, env):
    print("Trying to unswap usb_desc.h")
    if os.path.isfile(full_path + '_usb_desc.h'): # only if the temp version exists
        print('Naming back...')
        os.remove(full_path + 'usb_desc.h')
        os.rename(full_path + '_usb_desc.h', full_path + 'usb_desc.h')
    

env.AddPreAction("$BUILD_DIR/src/analog.cpp.o", swap_origin_usb_desc)
env.AddPostAction("upload", unswap_origin_usb_desc)
