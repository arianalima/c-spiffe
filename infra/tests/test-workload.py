#!/usr/bin/env python

from ctypes import *

def check_so(soname):
    try:
        lib = CDLL(soname)
        print("INFO: Found so as", lib)
        error=0
        dummy_string = create_string_buffer(str.encode("original_string"))
        # client = lib.workloadapi_NewClient(error)
        print('a')
        # lib.workloadapi_Client_defaultOptions("client", None)
        print('b')
        error = lib.workloadapi_Client_Connect(dummy_string)
        print('c', error)
        print(lib.workloadapi_Client_defaultOptions(dummy_string))
        print('d')
        return True
    except OSError as ex:
        print("WARNING:", ex)
        return False


if __name__ == "__main__":
    
    j = check_so("/mnt/build/workload/libsource.so")
    if not j:
        print("Error: Could not test")
    else:
        print("Success: J %s  \n" % j)
