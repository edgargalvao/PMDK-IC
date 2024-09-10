echo "HAve you installed libsasl2-dev?"
gcc -o memslap  memslap.c ms_setting.c ms_stats.c ms_thread.c ms_conn.c ms_task.c -I./ -lpthread -levent -lm
