import numpy as np
import tensorflow as tf
from tensorflow.keras import Model
from tensorflow.keras.layers import Conv2D, Input, Dense, MaxPool2D
import sys


def main(argv):

    input_image = Input(shape=(256, 256, 3))
    conv1 = Conv2D(8, (3, 3), padding='same', activation='relu')(input_image)
    pool1 = MaxPool2D(pool_size=(2, 2), stride2=(2, 2), padding='same')(conv1)
    conv2 = Conv2D(8, (3, 3), padding='same', activation='relu')(pool1)
    pool2 = MaxPool2D(pool_size=(2, 2), strides=(2, 2), padding='same')(conv2)
    conv3 = Conv2D(8, (3, 3), padding='same', activation='relu')(pool2)
    pool3 = MaxPool2D(pool_size=(2, 2), strides=(2, 2), padding='same')(conv3)
    conv4 = Conv2D(4, (3, 3), padding='same', activation='relu')(pool3)
    dense1 = Dense(1024, activation='relu')(conv4)
    output = Dense(5, activation='softmax')(dense1)
    target = Input(shape=(5,))

    return 0


if __name__ == '__main__':
    ret = main(sys.argv)
    exit(ret)
