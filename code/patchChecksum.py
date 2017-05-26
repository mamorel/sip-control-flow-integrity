from pwn import *
import struct
import sys
from binascii import unhexlify

if(len(sys.argv) != 2):
    print "Usage: python "+sys.argv[0]+" <binaryName>"

orig_name = sys.argv[1]

with open("checksum.txt", "r") as f:
    new_hash = unhexlify(f.read().strip())

e = ELF(sys.argv[1])

addr = int("0x603100", 16)
e.write(addr, new_hash)
checksum = e.disasm(addr, 32)
print "Checksum", checksum

new_name = orig_name+"_patched"
e.save(new_name)
