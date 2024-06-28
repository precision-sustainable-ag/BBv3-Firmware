import subprocess
from subprocess import Popen, PIPE, STDOUT
from from_root import from_root

firmware_compiled = str(from_root("build/parser/parser"))
print(firmware_compiled)

process = Popen(args=(),bufsize=1, executable=firmware_compiled,stdin=PIPE,stdout=PIPE,stderr=STDOUT)
print(process.communicate("?C\n")[0])
process.kill()