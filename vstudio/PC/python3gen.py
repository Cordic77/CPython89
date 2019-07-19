# Generate python3stub.def and python3_d.def out of python3.def
# The regular import library cannot be used, since it doesn't
# provide the right symbols for data forwarding

# ---  python3stub.def  ---
out = open("python3stub.def", "w")
out.write('EXPORTS\n')

inp = open("..\..\cpython\PC\python3.def")
line = inp.readline()
while line.strip().startswith(';'):
    line = inp.readline()
line = inp.readline() # LIBRARY
assert line.strip()=='EXPORTS'

for line in inp:
    # SYM1=python3x.SYM2[ DATA]
    head, tail = line.split('.')
    if 'DATA' in tail:
        symbol, tail = tail.split(' ')
    else:
        symbol = tail.strip()
    out.write(symbol+'\n')

inp.close()
out.close()

# ---  python3_d.def  ---
out = open("python3_d.def", "w")
inp = open("..\..\cpython\PC\python3.def")
while True:
    line = inp.readline()
    if not line.strip().startswith(';'):
        break
    out.write(line)

line = line.replace("python3", "python3_d")
out.write(line)

line = inp.readline() # LIBRARY
assert line.strip()=='EXPORTS'
out.write(line)

for line in inp:
    # SYM1=python3x.SYM2[ DATA]
    head, tail = line.split('.')
    symbol = tail.strip()
    out.write(head+'_d.'+symbol+'\n')

inp.close()
out.close()
