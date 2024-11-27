import json
import hashlib
import os
import sys
import zipfile
import urllib.request
import re
from pathlib import Path
import subprocess



archive = zipfile.ZipFile('techstudent10.discord_rich_presence', 'r')
eee = archive.open('changelog.md')
eeee = []
for eeeee in eee:
	eeee.append(eeeee.decode('utf-8'))

eeeeee = ''.join(eeee)

one = ('## ' + eeeeee.split('##')[1]).replace((eeeeee.split('##')[1]).split('\r')[0], 'What\'s New This Update').replace('\n', "")
two = json.loads(archive.read('mod.json'))["version"].replace("v","")

if os.getenv('GITHUB_OUTPUT'):
	with open(os.getenv('GITHUB_OUTPUT'), 'a') as file:
		file.write(f'cl={one}\n')
if os.getenv('GITHUB_OUTPUT'):
	with open(os.getenv('GITHUB_OUTPUT'), 'a') as file:
		file.write(f'version={two}\n')
