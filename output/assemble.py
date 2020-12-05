import os
for i in range(256):
	os.system("montage -geometry +0+0 f"+str(i)+"/generatedFractal_P?????.png -tile 256x1 f"+str(i)+".png")
