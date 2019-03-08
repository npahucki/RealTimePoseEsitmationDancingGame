import numpy as np
import tensorflow as tf
import time
import sys
import os.path
from kerasify import export_model

output_dir = sys.argv[1] if sys.argv[1] else "models/"

LABEL_NAMES = {
    0: "normal",
    1: "right-arm-raised",
    2: "left-arm-raised",
}


def load_samples(label_num):
    name = LABEL_NAMES[label_num]
    loaded_data = np.loadtxt("samples/{}.txt".format(name))
    loaded_data_labels = np.full(loaded_data.shape[0], label_num)
    loaded_tests = np.loadtxt("samples/{}_test.txt".format(name))
    loaded_tests_labels = np.full(loaded_tests.shape[0], label_num)
    return loaded_data_labels, loaded_data, loaded_tests_labels, loaded_tests


data = np.empty((0, 50), float)
data_labels = np.empty(0, int)
tests = np.empty((0, 50), float)
tests_labels = np.empty(0, int)

for label_num in range(len(LABEL_NAMES)):
    (loaded_labels, loaded_data, loaded_tests_labels, loaded_tests) = load_samples(label_num)
    data_labels = np.append(data_labels, loaded_labels)
    data = np.append(data, loaded_data, axis=0)
    tests_labels = np.append(tests_labels, loaded_tests_labels)
    tests = np.append(tests, loaded_tests, axis=0)

# Try with only the torso keypoints which are 0-7 * (x,y)
data = data[:, 0:16]
tests = tests[:, 0:16]

data = tf.keras.utils.normalize(data, axis=1)
tests = tf.keras.utils.normalize(tests, axis=1)

model = tf.keras.models.Sequential()
model.add(tf.keras.layers.Dense(64, activation=tf.nn.relu))
model.add(tf.keras.layers.Dense(64, activation=tf.nn.relu))
model.add(tf.keras.layers.Dense(3, activation=tf.nn.softplus))
model.compile(optimizer='adam', loss='sparse_categorical_crossentropy', metrics=['accuracy'])

start_time = time.time()
model.fit(data, data_labels, epochs=100, batch_size=2)
val_loss, val_acc = model.evaluate(tests, tests_labels)
elapsed_time = time.time() - start_time
print("Final Loss: {} Final Accuracy:{} Total Time:{}s".format(val_loss, val_acc, elapsed_time))

export_model(model, os.path.join(output_dir, 'raised-hands-upper-body-only.model'))


