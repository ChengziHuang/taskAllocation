import os
import subprocess
from unittest import result


result_list = []

for root, dirs, files in os.walk('./'):
    for file in files:
        if os.path.splitext(file)[1] == '.test':
            output = subprocess.Popen(["./MC", file], stdout=subprocess.PIPE).communicate()[0]
            output = str(output)

            if ('success' in output):
                result_list.append(1)
            if ('failure' in output):
                result_list.append(0)

print(result_list)
ratio = sum(result_list) / len(result_list)
print(ratio)
