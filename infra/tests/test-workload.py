#!/usr/bin/env python

from ctypes import *

def check_so(soname):
    try:
        lib = CDLL(soname)
        print("INFO: Found so as", lib)
        error=0
        #client = lib.workloadapi_NewClient(error)
        print('a')
        #lib.workloadapi_Client_defaultOptions(client, None)
        print('b')
        #error = lib.workloadapi_Client_Connect(client)
        print('c')
        #print(lib.workloadapi_Client_FetchX509SVID(client, error))
        print('d')
        return True
    except OSError as ex:
        print("WARNING:", ex)
        return False


if __name__ == "__main__":
    
    j = check_so("./libsource1.so")
    if not j:
        print("Error: Could not test")
    else:
        print("Success: J %s  \n" % j)
