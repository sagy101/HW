#!/usr/bin/python

import sys
import os
import time
import pyTodoTasks


if __name__ == "__main__":
	pid = os.getpid()
	deadline_delta = 5
	punish_delay = 10

	#check simple one punishment
	base_deadline = int(time.time())
	pyTodoTasks.add_TODO(pid, "boo", base_deadline + deadline_delta)
	times = []
	for i in range(10):
		times.append(int(time.time()) - base_deadline)
		for k in xrange(12000000):
			pass
		print times[i]

	for sample in times:
		assert ((sample < (deadline_delta + 2)) or (sample > deadline_delta + punish_delay - 2)), "The process was awake at time %d when he was supposed to be asleep" % sample
	print "simple one punishment successful"

	#check that punishment are aggregated
	base_deadline = int(time.time())
	pyTodoTasks.add_TODO(pid, "boo", base_deadline + deadline_delta)
	pyTodoTasks.add_TODO(pid, "coo", base_deadline + deadline_delta)
	pyTodoTasks.add_TODO(pid, "doo", base_deadline + deadline_delta)
	times = []
	for i in range(10):
		#print "first for"
		times.append(int(time.time()) - base_deadline)
		for k in xrange(12000000):
			pass
		print times[i]
	

	for sample in times:
		#print "second for"
		assert ((sample < (deadline_delta+2)) or (sample > deadline_delta+punish_delay*3-2)) , "one of the processes was awake at time %d when he was supposed to be asleep" % sample


	print "three punishments successful aggregated"

	#check that serial punishment
	base_deadline = int(time.time())
	pyTodoTasks.add_TODO(pid, "boo", base_deadline + deadline_delta)
	pyTodoTasks.add_TODO(pid, "boo", base_deadline + deadline_delta * 2) #inside of punishment of previous TODO punishment
	pyTodoTasks.add_TODO(pid, "boo", base_deadline + deadline_delta * 6) # outside of two prev TODO punishments
	times = []
	for i in range(15):
		# print "first for"
		times.append(int(time.time()) - base_deadline)
		for k in xrange(12000000):
			pass
		print times[i]
	for sample in times:
		# print "second for"
		assert ((sample < (deadline_delta + 2)) or ((sample > deadline_delta + punish_delay * 2 - 2) and (sample < deadline_delta +  punish_delay * 3 + 2)) or (sample > (deadline_delta * 2 + punish_delay * 3 - 2))), "one of the processes was awake at time %d when he was supposed to be asleep" % sample

	print "serial punishments successful"

	# check add deadline invalid
	try:
		pyTodoTasks.add_TODO(pid, "boo", base_deadline - deadline_delta)
	except:
		print "add deadline invalid check successful"

	# punishment starts at exactly punishment end
	base_deadline = int(time.time())
	pyTodoTasks.add_TODO(pid, "boo", base_deadline + deadline_delta)
	pyTodoTasks.add_TODO(pid, "coo", base_deadline + deadline_delta * 3)
	times = []
	for i in range(15):
		times.append(int(time.time()) - base_deadline)
		for k in xrange(12000000):
			pass
		print times[i]
	for sample in times:
		assert ((sample < (deadline_delta + 2)) or (sample > (deadline_delta + punish_delay * 2 - 2))) , "one of the processes was awake at time %d when he was supposed to be asleep" % sample

	print "exactly serial punishments successful"

	# check mark - no punishment
	base_deadline = int(time.time())
	pyTodoTasks.add_TODO(pid, "boo", base_deadline + deadline_delta)
	pyTodoTasks.read_TODO(pid, 1, 3)
	pyTodoTasks.mark_TODO(pid, 1, 1)
	times = []
	for i in range(10):
		# print "first for"
		times.append(int(time.time()) - base_deadline)
		for k in xrange(12000000):
			pass
		print times[i]
	for sample in times:
		# print "second for"
		assert (sample < (deadline_delta + punish_delay + 2)), "one of the processes was asleep at time %d when he was supposed to be awake" % sample

	print "marked successful no punishment occurred"

	# fork child
	times = []
	base_deadline = int(time.time())
	pyTodoTasks.add_TODO(pid, "boo", base_deadline + deadline_delta * 2)
	pidC = os.fork()

	if pidC == 0:
		pid = os.getpid()
		pyTodoTasks.add_TODO(pid, "doo", base_deadline + deadline_delta * 5)
		for i in range(20):
			times.append(int(time.time()) - base_deadline)
			for k in xrange(12000000):
				pass
			print
			times[i]
		for sample in times:
			assert ((sample < (deadline_delta * 2 + 2)) or ((sample > deadline_delta * 2 + punish_delay - 2) and (
					sample < deadline_delta * 5 + 2)) or (sample > (
						deadline_delta * 5 + punish_delay - 2))), "one of the processes was awake at time %d when he was supposed to be asleep" % sample
		print
		"child success"
		os._exit(0)
	else:
		pyTodoTasks.delete_TODO(pid, 1)
		os.wait()

	# fork father
	times = []
	base_deadline = int(time.time())
	pyTodoTasks.add_TODO(pid, "boo", base_deadline + deadline_delta * 2)
	pidC = os.fork()

	if pidC == 0:
		try:
			pyTodoTasks.add_TODO(pid, "doo", base_deadline + deadline_delta * 5)
		except:
			os._exit(0)
		os._exit(0)
	else:
		pyTodoTasks.add_TODO(pid, "doo", base_deadline + deadline_delta * 5)
		for i in range(20):
			times.append(int(time.time()) - base_deadline)
			for k in xrange(12000000):
				pass
			print
			times[i]
		for sample in times:
			assert ((sample < (deadline_delta * 2 + 2)) or ((sample > deadline_delta * 2 + punish_delay - 2) and (
					sample < deadline_delta * 5 + 2)) or (sample > (
					deadline_delta * 5 + punish_delay * 1 - 2))), "one of the processes was awake at time %d when he was supposed to be asleep" % sample
		print
		"father success"
		os.wait()
