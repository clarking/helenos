#!/usr/bin/env python
#
# Copyright (c) 2014 Jakub Jermar 
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# - Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
# - Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
# - The name of the author may not be used to endorse or promote products
#   derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

import sys
import yaml 
import re

def usage():
	print("%s - Automated structure and offsets generator" % sys.argv[0])
	print("%s file.ag probe|generate struct.ag" % sys.argv[0])
	sys.exit()

def generate_includes(struct):
	code = ""
	for include in struct['includes']:
		if 'guard' in include.keys():
			code = code + "#ifdef %s\n" % include['guard']
		if 'negative-guard' in include.keys():
			code = code + "#ifndef %s\n" % include['negative-guard']
		code = code + "#include %s\n" % include['include']
		if 'guard' in include.keys():
			code = code + "#endif\n"
		if 'negative-guard' in include.keys():
			code = code + "#endif\n"
	return code.strip()

def generate_struct(struct):
	code = "typedef struct %s {\n" % struct['name']
	for i in range(len(struct['members'])):
		member = struct['members'][i]
		code = code + "\t%s %s;\n" % (member['type'], member['name'])
	code = code + "} %s_t;" % struct['name']
	return code

def generate_probes(struct):
	code = ""
	for i in range(len(struct['members'])):
		member = struct['members'][i]
		code = code + ("\temit_constant(%s_OFFSET_%s, offsetof(%s_t, %s));\n" % 
		    (struct['name'].upper(), member['name'].upper(), struct['name'],
		    member['name']))
	return code

def probe(struct):
	name = struct['name']
	typename = struct['name'] + "_t"

	code = """
%s		

#define str(s) #s
#define emit_constant(n, v) \
    asm volatile ("EMITTED_CONSTANT " str(n) \" = %%0\" :: \"i\" (v))
#define offsetof(t, m) ((size_t) &(((t *) 0)->m))

%s

int main()
{
%s
	emit_constant(%s_SIZE, sizeof(%s));
	return 0;
}
	""" % (generate_includes(struct), generate_struct(struct),
	    generate_probes(struct), name.upper(), typename)

	return code

def generate_defines(pairs):
	code = ""
	for pair in pairs:
		code = code + "#define %s %s\n" % (pair[0], pair[1])
	return code.strip()

def generate(struct, lines):
	code = """
/***************************************************************************** 
 * AUTO-GENERATED FILE, DO NOT EDIT!!!
 * Generated by: tools/autogen.py
 * Generated from: %s
 *****************************************************************************/

#ifndef AUTOGEN_%s_H
#define AUTOGEN_%s_H

#ifndef __ASM__
%s
#endif

%s

#ifndef __ASM__
%s
#endif

#endif
	""" % (sys.argv[2], struct['name'].upper(), struct['name'].upper(),
	    generate_includes(struct), generate_defines(lines),
	    generate_struct(struct))

	return code

def filter_pairs(lines):
	pattern = re.compile("^\tEMITTED_CONSTANT ([A-Z_][A-Z0-9_]*) = \$([0-9]+)$");
	pairs = []
	for line in lines:
		res = pattern.match(line)
		if res == None:
			continue
		pairs = pairs + [res.group(1, 2)]
	return pairs
	

def run():
	if len(sys.argv) != 3:
		usage()

	with open(sys.argv[2], "rb") as fp:
		struct = yaml.load(fp)

	if sys.argv[1] == "probe":
		code = probe(struct)
		print(code)
	elif sys.argv[1] == "generate":
		lines = sys.stdin.readlines()
		pairs = filter_pairs(lines)
		code = generate(struct, pairs)
		print(code)
	else:
		usage()

run()

