
import os, random

from bup import options
from bup.compat import argv_bytes
from bup.helpers import log
from bup.io import path_msg


def randblock(n):
    return bytes(random.randrange(0,256) for i in range(n))


optspec = """
bup damage [-n count] [-s maxsize] [-S seed] <filenames...>
--
   WARNING: THIS COMMAND IS EXTREMELY DANGEROUS
n,num=   number of blocks to damage
s,size=  maximum size of each damaged block
percent= maximum size of each damaged block (as a percent of entire file)
equal    spread damage evenly throughout the file
S,seed=  random number seed (for repeatable tests)
"""

def main(argv):
    o = options.Options(optspec)
    opt, flags, extra = o.parse_bytes(argv[1:])

    if not extra:
        o.fatal('filenames expected')

    if opt.seed != None:
        random.seed(opt.seed)

    for name in extra:
        name = argv_bytes(name)
        log('Damaging "%s"...\n' % path_msg(name))
        with open(name, 'r+b') as f:
            st = os.fstat(f.fileno())
            size = st.st_size
            if opt.percent or opt.size:
                ms1 = int(float(opt.percent or 0)/100.0*size) or size
                ms2 = opt.size or size
                maxsize = min(ms1, ms2)
            else:
                maxsize = 1
            chunks = opt.num or 10
            chunksize = (size // chunks) or 1
            for r in range(chunks):
                sz = random.randrange(1, maxsize+1)
                if sz > size:
                    sz = size
                if opt.equal:
                    ofs = (r * chunksize) % size
                else:
                    ofs = random.randrange(0, size - sz + 1)
                log('  %6d bytes at %d\n' % (sz, ofs))
                f.seek(ofs)
                f.write(randblock(sz))
