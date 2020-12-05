import numpy as np
import os
for d in range(255):
    try: os.mkdir("output/f"+str(d+1))  
    except: continue
