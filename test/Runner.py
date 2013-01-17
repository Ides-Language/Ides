#!/usr/bin/env python

import string
import sys
import re
import ConfigParser
import StringIO
import os
import subprocess
import tempfile

cfgregex = re.compile("/\\*\\s*(\\[test\\].*)\\*/", re.DOTALL)
assert(cfgregex != None)

ilibfile = "%s/test.ilib" % tempfile.gettempdir()

def cfg_default(cfgfile, section, option, default):
	if (cfgfile.has_option(section, option)):
		return cfgfile.get(section, option)
	return default

def test(ic, x):
	with file(os.path.expanduser(x)) as f:
		src = f.read()
		m = cfgregex.search(src)
		if m == None:
			print "No test definition found in %s" % x
			return False
		cfgfile = ConfigParser.SafeConfigParser()
		cfgfile.readfp(StringIO.StringIO(m.group(1)))

		success = True
		
		exitcode = 0
		compiler_exitcode = 0
		if cfgfile.has_option("compiler", "exit"):
			exitcode = cfgfile.getint("compiler", "exit")

		compiler_stdout_re = string.strip(cfg_default(cfgfile, "compiler", "stdout", ""))
		compiler_stderr_re = string.strip(cfg_default(cfgfile, "compiler", "stderr", ""))

		if cfgfile.has_option("test", "exit"):
			exitcode = cfgfile.getint("test", "exit")

		stdout_re = string.strip(cfg_default(cfgfile, "test", "stdout", ""))
		stderr_re = string.strip(cfg_default(cfgfile, "test", "stderr", ""))

		proc = subprocess.Popen([ic, "-o", ilibfile, x], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
		data = proc.communicate()

		if not re.match(compiler_stdout_re, data[0]):
			print "compiler stdout did not match regex '%s':\n%s" % (compiler_stdout_re, data[0])
			success = False
		if not re.match(compiler_stderr_re, data[1]):
			print "compiler stderr did not match regex '%s':\n%s" % (stderr_re, data[1])
			success = False

		if proc.returncode != compiler_exitcode:
			print "Bad return code from compiler. Expected %s, got %s." % (compiler_exitcode, proc.returncode)
			success = False

		if not success:
			return False
		elif compiler_exitcode != 0:
			return True # We expected the compile to fail. Everything is OK.
		

		proc = subprocess.Popen(["lli", ilibfile], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
		data = proc.communicate()

		if not re.match(stdout_re, data[0]):
			print "stdout did not match regex '%s':\n%s" % (stdout_re, data[0])
			success = False
		if not re.match(stderr_re, data[1]):
			print "stderr did not match regex '%s':\n%s" % (stderr_re, data[1])
			success = False

		if proc.returncode != exitcode:
			print "Bad return code from process. Expected %s, got %s." % (exitcode, proc.returncode)
			success = False

		return success



srcfile = sys.argv[1]
compiler = sys.argv[2]
if test(srcfile, compiler) == False:
	print "Test of %s failed." % srcfile
	sys.exit(1)
else:
	print "Test of %s passed." % srcfile

