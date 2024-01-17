import os

current_path = os.environ.get('PATH', '')
new_directory = '/path/to/your/new/directory'
new_path = f'{current_path}:{new_directory}'

os.environ['PATH'] = new_path
print("Updated PATH:", os.environ['PATH'])
