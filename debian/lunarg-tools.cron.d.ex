#
# Regular cron jobs for the lunarg-tools package
#
0 4	* * *	root	[ -x /usr/bin/lunarg-tools_maintenance ] && /usr/bin/lunarg-tools_maintenance
