import shutil
import os
import sys

if len(sys.argv) == 1:
    print 'Delete all the directories provided in argument if they exist'
for p in sys.argv[1:]:
    if os.path.isdir( p ):
        shutil.rmtree( p )
