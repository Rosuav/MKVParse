import glob
import sys; sys.path.append("../python-ebml")
from ebml.container import File

# Initial attempt to actually parse EBML... probably not worth the hassle.
def read_bytes(n):
	global data
	if len(data) < n: raise ValueError("Insufficient data")
	ret = data[:n]
	data = data[n:]
	return ret

def read_vint():
	b = format(read_bytes(1)[0], "08b")
	more = b.index("1")
	binary = b[more + 1:] + "".join(format(b, "08b") for b in read_bytes(more))
	return int(binary, 2)

def parse_mkv(fn):
	print(fn)
	file = File(fn)
	print(file.summary())
	return file

if __name__ == '__main__':
	for fn in sys.argv[1:] or glob.glob("*.mkv"):
		file = parse_mkv(fn) # Stash for interactive mode if desired
