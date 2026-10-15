"""Independent local QR decode check; optional opencv-python/numpy, no device I/O."""
import json
from pathlib import Path
import cv2
import numpy as np

root=Path(__file__).resolve().parents[1]
data=json.loads((root/'build/qr-reference-test.json').read_text())
matrix=np.array(data['matrix'],dtype=np.uint8)
image=np.pad(255-matrix*255,4,constant_values=255)
image=np.repeat(np.repeat(image,10,axis=0),10,axis=1)
decoded,points,_=cv2.QRCodeDetector().detectAndDecode(image)
assert decoded==data['payload'],(decoded,data['payload'])
cv2.imwrite(str(root/'build/qr-reference-test.png'),image)
print('PASS: offline QR independently decoded; exact QC reference payload')
