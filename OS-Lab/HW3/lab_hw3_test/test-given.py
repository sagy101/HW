#!/usr/bin/python

import sys
import os
import time
import pyTodoTasks


if __name__ == "__main__":
	pid = os.getpid()
	deadline_delta = 5
	punish_delay = 10
	# Add TODO with a deadline in 5 seconds
	base_deadline = int(time.time())
	#print "adding"
	pyTodoTasks.add_TODO(pid, "boo", base_deadline+deadline_delta)
	#print "added"
	times = []
	# Log when the process was awake
	for i in range(10):
		#print "first for"
		times.append(int(time.time()) - base_deadline)
		for k in xrange(12000000):
			pass
		print times[i]
	
	#print "first for done"
	for sample in times:
		#print "second for"
		assert ((sample < (deadline_delta+2)) or (sample > deadline_delta+punish_delay-2)) , "The process was awake at time %d when he was supposed to be asleep" % sample
		
	print "Test done"
