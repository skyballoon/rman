#!/bin/bash

date
c_begin_time_sec=`date +%s`

rmanformat=$(date +%Y%m%d_%H%M%S)
orabackup=/u01/app/oracle/backups
foldername=$(date +%Y%m%d)
dateformat=$(date +%d%m%Y_%H%M%S)
currentfolder="$orabackup"/pools/"$foldername"
mkdir -p  "$currentfolder"

#create pfile from spfile
su -c "
. /home/oracle/.bash_profile
sqlplus / as sysdba << EOF
create pfile='/u01/app/oracle/backups/pools/pfile.ora' from spfile;
exit;
EOF
" oracle

su -c "
. /home/oracle/.bash_profile
#metadata
expdp \'/ as sysdba\' directory=DUMPDIR dumpfile=MTDT-'$dateformat'.DMP logfile=MTDT-'$dateformat'.LOG content=METADATA_ONLY
expdp \'/ as sysdba\' directory=DUMPDIR dumpfile=DATA-'$dateformat'.DMP logfile=DATA-'$dateformat'.LOG full=Y
" oracle

'mv' -f "$orabackup"/pools/pfile.ora "$currentfolder"

'cp' -f /u01/app/oracle/product/11.2.0/db1/dbs/spfileitcsyariah2.ora "$currentfolder"/spfileitcsyariah2.ora
'cp' -f /u01/app/oracle/product/11.2.0/db1/network/admin/listener.ora "$currentfolder"/listener.ora
'cp' -f /u01/app/oracle/product/11.2.0/db1/network/admin/tnsnames.ora "$currentfolder"/tnsnames.ora
'cp' -f /u01/app/oracle/backups/fast_recovery_area/itcsyariah2/controlspfile/control01.ctl "$currentfolder"/control01.ctl

'cp' -afr /u01/app/oracle/product/11.2.0/db1/dbs "$currentfolder"/dbs

tar cvzfP "$orabackup"/dumps/syr"$foldername".tgz "$orabackup"/dumps/*$dateformat*.* "$currentfolder"/ "$orabackup"/scripts/

openssl aes-256-cbc -in "$orabackup"/dumps/syr"$foldername".tgz -out "$orabackup"/dumps/syr"$foldername".tgz.enc -k IIttCC

'rm' -f "$orabackup"/dumps/*$dateformat*.*

'rm' -f "$orabackup"/dumps/syr"$foldername".tgz

'mv' -f "$orabackup"/dumps/syr"$foldername".tgz.enc "$orabackup"/dumps/syr"$foldername".tgz

'rm' -rf $orabackup/pools/$foldername

su -c "
. /home/oracle/.bash_profile
export NLS_DATE_FORMAT='YYYY/MM/DD HH24:MI:SS'
rman target=/ log=/mnt/bcks/rman/archivelog/59/logs/crc_${rmanformat}.log << EOF
SET ECHO ON;
RUN {
  SHOW ALL;
  CONFIGURE CONTROLFILE AUTOBACKUP ON;
  CONFIGURE CONTROLFILE AUTOBACKUP FORMAT FOR DEVICE TYPE DISK TO '/mnt/bcks/rman/controlfile/59/59ctl_%F';

  CROSSCHECK COPY;
  CROSSCHECK BACKUP;
  CROSSCHECK ARCHIVELOG ALL;

  DELETE NOPROMPT EXPIRED ARCHIVELOG ALL;
  DELETE NOPROMPT EXPIRED BACKUP;
  DELETE NOPROMPT EXPIRED COPY;
  DELETE NOPROMPT OBSOLETE;

  DELETE NOPROMPT BACKUP OF ARCHIVELOG ALL COMPLETED BEFORE 'SYSDATE-7';

  BACKUP SPFILE FORMAT '/mnt/bcks/rman/controlfile/59/spf_%d_%s_%T_dbid%I.ora' TAG 'SPF-DAILY${rmanformat}'; 
  BACKUP CURRENT CONTROLFILE FORMAT '/mnt/bcks/rman/controlfile/59/ctl_%t_dbid%I.bak' TAG 'CTL-DAILY${rmanformat}';
}
EXIT;
EOF
" oracle

date
c_end_time_sec=`date +%s`
v_total_execution_time_sec=`expr ${c_end_time_sec} - ${c_begin_time_sec}`
echo "Script execution time is $v_total_execution_time_sec seconds"
