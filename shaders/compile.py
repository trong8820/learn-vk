import os

if not os.path.exists("../data"):
    os.makedirs("../data")

for file in os.listdir('.'):
    if file.endswith('.vert') or file.endswith('.frag'):
        print(f'glslangValidator.exe -V {file} -o ../data/{file}.spv')
        os.system(f'glslangValidator.exe -V {file} -o ../data/{file}.spv')