#!/bin/python3

# Purpose of this script
# ----------------------
# The script will fetch a git describe and compare it to the last one seen.
# If the git describe changed it will touch some files to trigger recompilation of them by Qt.
# I recommend adding it as first step to the build process.

import os
from subprocess import Popen, PIPE

scriptDir = os.path.dirname(os.path.realpath(__file__))
lastGitDescribeFile = scriptDir + '/lastGitDescribe'
filesToTouch = ['Moneyphant.pro', 'src/version.cpp']

def execute(cmd):
	process = Popen(cmd, stdout=PIPE)
	(output, err) = process.communicate()
	exit_code = process.wait()
	return output.decode("utf-8")[0:-1]

def hasVersionChanged(gitDescribe):
	if os.path.isfile(lastGitDescribeFile):
		with open(lastGitDescribeFile, 'r') as f:
			if f.readline() == gitDescribe:
				return False
	return True

def onNewVersion(gitDescribe):
	with open(lastGitDescribeFile, 'w') as f:
		f.write(gitDescribe)
	for f in filesToTouch:
		execute(['touch', scriptDir + '/../' + f])

gitDescribe = execute(['git', '--git-dir', scriptDir + '/../.git', '--work-tree', scriptDir + '/..', 'describe', '--long', '--always', '--tags'])

if hasVersionChanged(gitDescribe):
	onNewVersion(gitDescribe)

