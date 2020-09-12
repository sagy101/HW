#!/usr/bin/python

import os
import pyTodoTasks
import time
pid = os.getpid()
#
# Calling system api's.
#
print("////////////////////add dog start///////////////////")
pyTodoTasks.add_TODO(pid, "dog")
print("////////////////////add dog ended///////////////////")

print("////////////////////read dog in 1 start//////////////")
pyTodoTasks.read_TODO(pid, 1, len("dog"))
print("////////////////////read dog in 1 end//////////////")

print("////////////////////add feed start///////////////////")
pyTodoTasks.add_TODO(pid, "feed")
print("////////////////////add feed ended///////////////////")

print("////////////////////add night started///////////////////")
pyTodoTasks.add_TODO(pid, "night")
print("////////////////////add night ended///////////////////")

print("///////////////read night in 3 start//////////////////")
pyTodoTasks.read_TODO(pid, 3, len("night"))
print("////////////////////read night in 3 end//////////////")

print("////////////read feed in 2 start/////////////////////")
pyTodoTasks.read_TODO(pid, 2, len("feed"))
print("////////////read feed in 2 end/////////////////////")

print("////////////////////dog deleted start/////////////////////")
pyTodoTasks.delete_TODO(pid, 1)
print("////////////////////dog deleted end/////////////////////")

print("////////////read feed in 2 start/////////////////////")
pyTodoTasks.read_TODO(pid, 2, len("night"))
print("////////////read feed in 2 end//////////////////////")

print("//////////////////feed marked with 1 start///////////////////")
pyTodoTasks.mark_TODO(pid, 1, 1)
print("//////////////////feed marked with 1 end///////////////////")

print("/////////////////////read feed in 2 start//////////////////")
pyTodoTasks.read_TODO(pid, 1, len("feed"))
print("///read feed in 2 end///")

print("////////////////////dog marked with 10 start/////////////////////")
pyTodoTasks.mark_TODO(pid, 1, 10)
print("////////////////////dog marked with 10 end/////////////////////")

print("//////////////////read feed in 2 start////////////////////////")
pyTodoTasks.read_TODO(pid, 1, len("feed"))
print("////////////////////////read feed in 2 ended//////////////////")

print("////////////////////////forked//////////////////")
pidC = os.fork()

if pidC == 0:
	time.sleep(6)
	print("////////////////////add fish child start///////////////////")
	pyTodoTasks.add_TODO(os.getpid(), "fish")
	print("////////////////////add fish child ended///////////////////")
	print("////////////////child read 2 of night is:////////////////")
	pyTodoTasks.read_TODO(os.getpid(), 2, len("night"))
	print("////////////////child read 2 of night end////////////////")
	
else:
	print("////////////////////add cat father start///////////////////")
	pyTodoTasks.add_TODO(pid, "cat")
	print("////////////////////add cat father ended///////////////////")
	print("////////////////////add lion to child start///////////////////")
	pyTodoTasks.add_TODO(pidC, "lion")
	print("////////////////////add lion to child ended///////////////////")
	print("////////////////////feed deleted from child start/////////////////////")
	pyTodoTasks.delete_TODO(pidC, 1)
	print("////////////////////feed deleted from child end/////////////////////")
	print("////////////////////lion marked with 100 start/////////////////////")
	pyTodoTasks.mark_TODO(pidC, 2, 100)
	print("////////////////////lion marked with 100 end/////////////////////")
	print("////////////////1 father read 2 of night is:////////////////")
	pyTodoTasks.read_TODO(pid, 2, len("night"))
	print("////////////////1 father read 2 of night end////////////////")
	print("////////////////////night deleted from father start/////////////////////")
	pyTodoTasks.delete_TODO(pid, 1)
	print("////////////////////night deleted from father end/////////////////////")
	print("////////////////////lion deleted from father start/////////////////////")
	pyTodoTasks.delete_TODO(pid, 1)
	print("////////////////////lion deleted from father end/////////////////////")
	print("////////////////////fish deleted from father start/////////////////////")
	pyTodoTasks.delete_TODO(pid, 1)
	print("////////////////////fish deleted from father end/////////////////////")
	print("////////////////////add camel father start///////////////////")
	pyTodoTasks.add_TODO(pid, "camel")
	print("////////////////////add camel father ended///////////////////")
	print("////////////////2 father read 1 of camel is:////////////////")
	pyTodoTasks.read_TODO(pid, 1, len("camel"))
	print("////////////////2 father read 1 of camel end////////////////")
	print("////////////////////camel marked with 1000 start/////////////////////")
	pyTodoTasks.mark_TODO(pid, 1, 1000)
	print("////////////////////camel marked with 1000 end/////////////////////")
	print("////////////////2 father read 1 of camel is:////////////////")
	pyTodoTasks.read_TODO(pid, 1, len("camel"))
	print("////////////////2 father read 1 of camel end////////////////")
	
	
	




