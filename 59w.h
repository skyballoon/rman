#!/bin/bash

date
c_begin_time_sec=`date +%s`

rmanformat=$(date +%Y%m%d_%H%M%S)

su -c "
. /home/oracle/.bash_profile
export NLS_DATE_FORMAT='YYYY/MM/DD HH24:MI:SS'
rman target=/ log=/mnt/uranus/rman/archivelog/59/logs/arc_${rmanformat}.log << EOF
SET ECHO ON;
RUN {
  SHOW ALL;
  CONFIGURE CONTROLFILE AUTOBACKUP ON;
  CONFIGURE CONTROLFILE AUTOBACKUP FORMAT FOR DEVICE TYPE DISK TO '/mnt/uranus/rman/controlfile/59/59ctl_%F';

  BACKUP INCREMENTAL LEVEL 1 CUMULATIVE DATABASE PLUS ARCHIVELOG TAG 'INC1-WEEKLY${rmanformat}';
  sql 'alter system archive log current';
  DELETE NOPROMPT OBSOLETE RECOVERY WINDOW OF 7 DAYS;

  CROSSCHECK ARCHIVELOG ALL;
  DELETE NOPROMPT ARCHIVELOG ALL COMPLETED BEFORE 'SYSDATE-7' BACKED UP 1 TIMES TO DISK;
}
EXIT;
EOF
" oracle

#find /mnt/uranus/rman/archivelog/59/logs/*rc_* -mtime +7 -exec rm {} \;

date
c_end_time_sec=`date +%s`
v_total_execution_time_sec=`expr ${c_end_time_sec} - ${c_begin_time_sec}`
echo "Script execution time is $v_total_execution_time_sec seconds"
