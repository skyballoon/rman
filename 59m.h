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
  sql 'alter system archive log current';
  BACKUP INCREMENTAL LEVEL 0 DATABASE PLUS ARCHIVELOG TAG 'INC0-MONTHLY${rmanformat}';
  DELETE NOPROMPT OBSOLETE RECOVERY WINDOW OF 7 DAYS;

  CROSSCHECK ARCHIVELOG ALL;
  DELETE NOPROMPT ARCHIVELOG ALL COMPLETED BEFORE 'SYSDATE-7' BACKED UP 1 TIMES TO DISK;
}
EXIT;
EOF
" oracle

date
c_end_time_sec=`date +%s`
v_total_execution_time_sec=`expr ${c_end_time_sec} - ${c_begin_time_sec}`
echo "Script execution time is $v_total_execution_time_sec seconds"
