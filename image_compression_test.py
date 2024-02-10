import cv2
import numpy as np
import matplotlib.pyplot as plt

size = np.array([8,8])
zres = 8
invert = -1

def gaussian_reduce(img, ksize, stride):
    convolved = cv2.GaussianBlur(img, ksize, 0)

    half = (np.array(stride)/2).astype(np.int32)
    convolved = convolved[half[0]:-half[0], half[1]:-half[1]]
    row, column = np.indices(((np.array(img.shape)[:-1])/stride).astype(np.int32))
    return convolved[row*stride[0], column*stride[1]]

def scale(img, min, max):
    img = img-np.min(img)
    return img/np.max(img)*(max-min) + min

raw = cv2.cvtColor(cv2.imread("g.webp"), cv2.COLOR_BGR2RGB)

gray = cv2.cvtColor(raw, cv2.COLOR_RGB2GRAY)

gray = cv2.resize(gray, [np.min(np.array(gray.shape))]*2)


stride = np.floor((np.array(gray.shape)[:-1]-1)/size).astype(np.int32)
ksize = np.ceil(np.array(gray.shape)[:-1]/size*2)
ksize = (np.floor(ksize/2)*2+1).astype(np.int32)

gray = gaussian_reduce(gray, ksize, stride)

gray = (scale(gray*invert, 0, zres-1)).astype(np.int32)

plt.imshow(gray, cmap="gray")
plt.show()

