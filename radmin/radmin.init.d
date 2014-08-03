#!/sbin/runscript
# GITI CNC Daemon

PATH='/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/opt/bin'

opts="start stop restart"

cmd="env -i PATH=${PATH} /usr/local/sbin/radmin -c /etc/radmin.conf >> /dev/null 2>&1 &"

pidfile="/var/run/radmin.pid"

start_cmd="start-stop-daemon --start --exec /usr/local/sbin/radmin \
    --pidfile /var/run/radmin.pid -l "

stop_cmd="start-stop-daemon --stop --quiet \
    --pidfile /var/run/radmin.pid"


depend() {
	need net
	use dns logger
}

start() {
	ebegin "Starting Radmin"
	${cmd}
# start-stop-daemon --quiet --start --exec ${cmd} -- --start -l ${logfile} -e ${errfile}
	eend $? 
}

stop() {
	ebegin "Stopping Radmin"
	killall radmin
	eend $?
}

svc_restart() {
        ebegin "Restarting Radmin"
        stop
        start
        eend $?
}

