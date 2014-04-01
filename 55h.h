#!/bin/bash

date
c_begin_time_sec=`date +%s`

dateformat=$(date +%d%m%Y_%H%M%S)

su -c "
. /home/oracle/.db_profile
export NLS_DATE_FORMAT='YYYY/MM/DD HH24:MI:SS'
rman target=/ log=/mnt/bcks/rman/archivelog/55/logs/arc_${dateformat}.log << EOF
SET ECHO ON;
RUN {
  SHOW ALL;
  CONFIGURE CONTROLFILE AUTOBACKUP ON;
  CONFIGURE CONTROLFILE AUTOBACKUP FORMAT FOR DEVICE TYPE DISK TO '/mnt/bcks/rman/controlfile/55/55ctl_%F';

  BACKUP ARCHIVELOG ALL NOT BACKED UP 1 TIMES TAG 'ARC-HOURLY${dateformat}';
  DELETE NOPROMPT ARCHIVELOG ALL COMPLETED BEFORE 'SYSDATE-8/24' BACKED UP 1 TIMES TO DISK;

  DELETE NOPROMPT BACKUP OF ARCHIVELOG ALL COMPLETED BEFORE 'SYSDATE-3';
}
EXIT;
EOF
" oracle

date
c_end_time_sec=`date +%s`
v_total_execution_time_sec=`expr ${c_end_time_sec} - ${c_begin_time_sec}`
echo "Script execution time is $v_total_execution_time_sec seconds"
