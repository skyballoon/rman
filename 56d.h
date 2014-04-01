#!/bin/bash
dateformat=$(date +%d%m%Y_%H%M%S)

date
c_begin_time_sec=`date +%s`

su -c "
. /home/oracle/.db_profile
export NLS_DATE_FORMAT='YYYY/MM/DD HH24:MI:SS'
rman target=/ log=/mnt/bcks/rman/archivelog/56/logs/crc_${dateformat}.log << EOF
SET ECHO ON;
RUN {
  SHOW ALL;
  CONFIGURE CONTROLFILE AUTOBACKUP ON;
  CONFIGURE CONTROLFILE AUTOBACKUP FORMAT FOR DEVICE TYPE DISK TO '/mnt/bcks/rman/controlfile/56/56ctl_%F';

  CROSSCHECK COPY;
  CROSSCHECK BACKUP;
  CROSSCHECK ARCHIVELOG ALL;

  DELETE NOPROMPT EXPIRED ARCHIVELOG ALL;
  DELETE NOPROMPT EXPIRED BACKUP;
  DELETE NOPROMPT EXPIRED COPY;
  DELETE NOPROMPT OBSOLETE;

  DELETE NOPROMPT ARCHIVELOG UNTIL TIME 'SYSDATE-8'; 

  BACKUP SPFILE FORMAT '/mnt/bcks/rman/controlfile/56/spf_%d_%s_%T_dbid%I.ora' TAG 'SPF-DAILY${dateformat}'; 
  BACKUP CURRENT CONTROLFILE FORMAT '/mnt/bcks/rman/controlfile/56/ctl_%t_dbid%I.bak' TAG 'CTL-DAILY${dateformat}';

}
EXIT;
EOF
" oracle

date
c_end_time_sec=`date +%s`
v_total_execution_time_sec=`expr ${c_end_time_sec} - ${c_begin_time_sec}`
echo "Script execution time is $v_total_execution_time_sec seconds"
