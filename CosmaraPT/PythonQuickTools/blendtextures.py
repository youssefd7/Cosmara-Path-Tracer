import subprocess
import sys
scriptname = sys.argv[0]
blendee1 = sys.argv[1]
blendee2 = sys.argv[2]
outputname = sys.argv[3]
result = subprocess.run(['convert', blendee1, blendee2,'-compose' ,'Over', 'remove','-composite', outputname], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
