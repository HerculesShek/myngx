
build:
	$(MAKE) -f objs/Makefile

install:
	$(MAKE) -f objs/Makefile install

clean:
	rm -rf Makefile objs

upgrade:
	/Users/will/CLionProjects/myngx/install/sbin/nginx -t

	# upgrade compatibility from 0.1.x to 0.2.x
	cp /Users/will/CLionProjects/myngx/install/logs/nginx.pid /Users/will/CLionProjects/myngx/install/logs/nginx.pid.oldbin

	kill -USR2 `cat /Users/will/CLionProjects/myngx/install/logs/nginx.pid`
	sleep 1
	test -f /Users/will/CLionProjects/myngx/install/logs/nginx.pid.oldbin

	# upgrade compatibility from 0.1.x to 0.2.x
	cp /Users/will/CLionProjects/myngx/install/logs/nginx.pid /Users/will/CLionProjects/myngx/install/logs/nginx.pid.newbin

	kill -WINCH `cat /Users/will/CLionProjects/myngx/install/logs/nginx.pid.oldbin`
