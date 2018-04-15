#coding:utf-8
import model
from glob import glob
import numpy as np
from PIL import Image
import time
import os
import sys
import cv2
import matplotlib.pyplot as plt  
paths = glob('./106/*.*')

if __name__ =='__main__':
    reload(sys)  
    sys.setdefaultencoding('utf-8')  
    i=1
    for path in paths:
        im = Image.open(path)
        img = np.array( im.convert('RGB'))
        t = time.time()
        result,img = model.model(img,model='keras')
        print "It takes time:{}s".format(time.time()-t)
        print "---------------------------------------"
	s="%s%d%s"%("./output/",i,".txt")	
	f1 = open(s,'w')
	i+=1
	for key in result:
		f1.write(result[key][1].encode('UTF-8'))
		
	f1.close() 