from pwn import *
import sys

if(len(sys.argv) != 2):
    print "Usage: python "+sys.argv[0]+" <binaryName>"
    print "\tExample: ", sys.argv[0], "memoryTrace"
    exit(1)

orig_name = sys.argv[1]

e = ELF(sys.argv[1])

funcAddr = e.symbols["verifyStack"]
verifyAddr = e.symbols["verify"]

callString = "call   "+hex(verifyAddr)

print "verifyStack", hex(funcAddr)
print 'verify:', hex(verifyAddr)

found = False

for i in range (400):
    dis = e.disasm(funcAddr + i, 5)
    if callString in dis:
        found = True
        print "Found: '" + dis + "'"
        addr = funcAddr + i
        e.write(addr, "\x90"*5)

if not found:
    print "verify call not found, could not patch binary."
else:
    new_name = orig_name+"_nop"

    print "Writing NOPed binary to file'"+new_name+"'"
    e.save(new_name)
