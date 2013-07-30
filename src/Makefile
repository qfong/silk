
CC = gcc
LINK = $(CC)

INSTALL=install

C_PREFIX=objs
C_LIBDIR=$(C_PREFIX)/lib
C_LIB_CORE=$(C_LIBDIR)/core
C_LIB_EVENT=$(C_LIBDIR)/event
C_LIB_OS=$(C_LIBDIR)/os
C_LIB_IDS=$(C_LIBDIR)/ids
C_SBINDIR=$(C_PREFIX)/sbin



CORE_INCS = -I src/core\
	-I src/event\
	-I src/ids\
	-I src/os\
	-I src/add

		
CORE_DEPS =./src/os/sk_config.h\
	src/os/sk_log.h\
	src/os/sk_array.h\
	src/os/sk_define.h\
	src/os/sk_palloc.h\
	src/os/sk_times.h\
	src/os/sk_buf.h\
	src/os/sk_socket.h\
	src/event/sk_event_posted.h\
	src/event/sk_event.h\
	src/event/sk_event_epoll.h\
	src/event/sk_event_accept.h\
	src/event/sk_event_timer.h\
	src/event/sk_event_blacklist_timer.h\
	src/core/sk_connection.h\
	src/core/sk_rbtree.h\
	src/core/sk_cycle.h\
	src/core/sk_shmtx.h\
	src/core/sk_process.h\
	src/core/sk_core.h\
	src/core/sk_process_cycle.h\
	src/ids/sk_ids.h\
	src/ids/sk_detect.h\
	src/ids/sk_body_detect.h\
	src/ids/sk_mem_pool.h\
	src/ids/sk_mempool_list.h\
	src/ids/sk_rule.h\
	src/ids/sk_sdlist.h\
	src/os/sk_tools.h\
	src/ids/sk_ids_log.h\
	src/ids/sk_ids_request.h\
	src/ids/mwm.h\
	src/add/cJSON.h\
	src/add/json_http.h
			

	

		
objs/silk:objs/lib/silk.o\
	objs/lib/os/sk_palloc.o\
	objs/lib/os/sk_log.o\
	objs/lib/os/sk_times.o\
	objs/lib/os/sk_array.o\
	objs/lib/os/sk_socket.o\
	objs/lib/os/sk_buf.o\
	objs/lib/event/sk_event_timer.o\
	objs/lib/event/sk_event_blacklist_timer.o\
	objs/lib/event/sk_event_epoll.o\
	objs/lib/event/sk_event_accept.o\
	objs/lib/event/sk_event_posted.o\
	objs/lib/event/sk_event.o\
	objs/lib/core/sk_cycle.o\
	objs/lib/core/sk_connection.o\
	objs/lib/core/sk_rbtree.o\
	objs/lib/core/sk_shmtx.o\
	objs/lib/core/sk_process.o\
	objs/lib/core/sk_process_cycle.o\
	objs/lib/ids/sk_ids.o\
	objs/lib/ids/sk_detect.o\
	objs/lib/ids/sk_body_detect.o\
	objs/lib/ids/sk_mem_pool.o\
	objs/lib/ids/sk_mempool_list.o\
	objs/lib/ids/sk_rule.o\
	objs/lib/ids/sk_sdlist.o\
	objs/lib/os/sk_tools.o\
	objs/lib/ids/sk_ids_log.o\
	objs/lib/ids/sk_ids_request.o\
	objs/lib/ids/mwm.o\
	objs/lib/add/cJSON.o\
	objs/lib/add/json_http.o
			
		$(LINK) -o ./objs/silk\
		objs/lib/silk.o\
		objs/lib/os/sk_palloc.o\
		objs/lib/os/sk_log.o\
		objs/lib/os/sk_times.o\
		objs/lib/os/sk_array.o\
		objs/lib/os/sk_socket.o\
		objs/lib/os/sk_buf.o\
		objs/lib/event/sk_event_timer.o\
		objs/lib/event/sk_event_blacklist_timer.o\
		objs/lib/event/sk_event_epoll.o\
		objs/lib/event/sk_event_accept.o\
		objs/lib/event/sk_event_posted.o\
		objs/lib/event/sk_event.o\
		objs/lib/core/sk_cycle.o\
		objs/lib/core/sk_connection.o\
		objs/lib/core/sk_rbtree.o\
		objs/lib/core/sk_shmtx.o\
		objs/lib/core/sk_process.o\
		objs/lib/core/sk_process_cycle.o\
		objs/lib/ids/sk_ids.o\
		objs/lib/ids/sk_detect.o\
		objs/lib/ids/sk_body_detect.o\
		objs/lib/ids/sk_mem_pool.o\
		objs/lib/ids/sk_mempool_list.o\
		objs/lib/ids/sk_rule.o\
		objs/lib/ids/sk_sdlist.o\
		objs/lib/os/sk_tools.o\
		objs/lib/ids/sk_ids_log.o\
		objs/lib/ids/mwm.o\
		objs/lib/ids/sk_ids_request.o\
		objs/lib/add/cJSON.o\
		objs/lib/add/json_http.o\
		 -g -O2 -Wall -lm


objs/lib/silk.o: $(CORE_DEPS)  \
	src/silk.c
	$(CC) -c $(CFLAGS) $(CORE_INCS)  \
		-o objs/lib/silk.o  \
		src/silk.c
		
objs/lib/core/sk_cycle.o: $(CORE_DEPS)  \
	src/core/sk_cycle.c
	$(CC) -c $(CFLAGS) $(CORE_INCS)  \
		-o objs/lib/core/sk_cycle.o  \
		src/core/sk_cycle.c
		
objs/lib/core/sk_connection.o: $(CORE_DEPS)  \
	src/core/sk_connection.c
	$(CC) -c $(CFLAGS) $(CORE_INCS)  \
		-o objs/lib/core/sk_connection.o  \
		src/core/sk_connection.c
		
objs/lib/core/sk_rbtree.o: $(CORE_DEPS)  \
	src/core/sk_rbtree.c
	$(CC) -c $(CFLAGS) $(CORE_INCS)  \
		-o objs/lib/core/sk_rbtree.o  \
		src/core/sk_rbtree.c
		
objs/lib/core/sk_shmtx.o: $(CORE_DEPS)  \
	src/core/sk_shmtx.c
	$(CC) -c $(CFLAGS) $(CORE_INCS)  \
		-o objs/lib/core/sk_shmtx.o  \
		src/core/sk_shmtx.c
		
objs/lib/core/sk_process.o: $(CORE_DEPS)  \
	src/core/sk_process.c
	$(CC) -c $(CFLAGS) $(CORE_INCS)  \
		-o objs/lib/core/sk_process.o  \
		src/core/sk_process.c
		
objs/lib/core/sk_process_cycle.o: $(CORE_DEPS)  \
	src/core/sk_process_cycle.c
	$(CC) -c $(CFLAGS) $(CORE_INCS)  \
		-o objs/lib/core/sk_process_cycle.o  \
		src/core/sk_process_cycle.c		
		
objs/lib/event/sk_event.o: $(CORE_DEPS)  \
	src/event/sk_event.c
	$(CC) -c $(CFLAGS) $(CORE_INCS)  \
		-o objs/lib/event/sk_event.o  \
		src/event/sk_event.c
		
objs/lib/event/sk_event_posted.o: $(CORE_DEPS)  \
	src/event/sk_event_posted.c
	$(CC) -c $(CFLAGS) $(CORE_INCS)  \
		-o objs/lib/event/sk_event_posted.o  \
		src/event/sk_event_posted.c
	
objs/lib/event/sk_event_accept.o: $(CORE_DEPS)  \
	src/event/sk_event_accept.c
	$(CC) -c $(CFLAGS) $(CORE_INCS)  \
		-o objs/lib/event/sk_event_accept.o  \
		src/event/sk_event_accept.c
		
objs/lib/event/sk_event_epoll.o: $(CORE_DEPS)  \
	src/event/sk_event_epoll.c
	$(CC) -c $(CFLAGS) $(CORE_INCS)  \
		-o objs/lib/event/sk_event_epoll.o  \
		src/event/sk_event_epoll.c
		
objs/lib/event/sk_event_timer.o: $(CORE_DEPS)  \
	src/event/sk_event_timer.c
	$(CC) -c $(CFLAGS) $(CORE_INCS)  \
		-o objs/lib/event/sk_event_timer.o  \
		src/event/sk_event_timer.c

objs/lib/event/sk_event_blacklist_timer.o: $(CORE_DEPS)  \
	src/event/sk_event_blacklist_timer.c
	$(CC) -c $(CFLAGS) $(CORE_INCS)  \
		-o objs/lib/event/sk_event_blacklist_timer.o  \
		src/event/sk_event_blacklist_timer.c		
			
objs/lib/os/sk_palloc.o: $(CORE_DEPS)\
	src/os/sk_palloc.c
	$(CC) -c $(CFLAGS) $(CORE_INCS) \
		-o objs/lib/os/sk_palloc.o \
		src/os/sk_palloc.c
		
objs/lib/os/sk_log.o: $(CORE_DEPS)\
	src/os/sk_log.c
	$(CC) -c $(CFLAGS) $(CORE_INCS) \
		-o objs/lib/os/sk_log.o \
		src/os/sk_log.c
		
objs/lib/os/sk_array.o: $(CORE_DEPS)  \
	src/os/sk_array.c
	$(CC) -c $(CFLAGS) $(CORE_INCS)  \
		-o objs/lib/os/sk_array.o  \
		src/os/sk_array.c		
		
objs/lib/os/sk_socket.o: $(CORE_DEPS)  \
	src/os/sk_socket.c
	$(CC) -c $(CFLAGS) $(CORE_INCS)  \
		-o objs/lib/os/sk_socket.o  \
		src/os/sk_socket.c		
		
objs/lib/os/sk_buf.o: $(CORE_DEPS)  \
	src/os/sk_buf.c
	$(CC) -c $(CFLAGS) $(CORE_INCS)  \
		-o objs/lib/os/sk_buf.o  \
		src/os/sk_buf.c		

objs/lib/os/sk_times.o: $(CORE_DEPS)  \
	src/os/sk_times.c
	$(CC) -c $(CFLAGS) $(CORE_INCS)  \
		-o objs/lib/os/sk_times.o  \
		src/os/sk_times.c		
		
objs/lib/ids/sk_ids.o: $(CORE_DEPS)  \
	src/ids/sk_ids.c
	$(CC) -c $(CFLAGS) $(CORE_INCS)  \
		-o objs/lib/ids/sk_ids.o  \
		src/ids/sk_ids.c		

objs/lib/ids/sk_detect.o: $(CORE_DEPS)  \
	src/ids/sk_detect.c
	$(CC) -c $(CFLAGS) $(CORE_INCS)  \
		-o objs/lib/ids/sk_detect.o  \
		src/ids/sk_detect.c	
			
objs/lib/ids/sk_body_detect.o: $(CORE_DEPS)  \
	src/ids/sk_body_detect.c
	$(CC) -c $(CFLAGS) $(CORE_INCS)  \
		-o objs/lib/ids/sk_body_detect.o  \
		src/ids/sk_body_detect.c	
	
objs/lib/ids/sk_mem_pool.o: $(CORE_DEPS)  \
	src/ids/sk_mem_pool.c
	$(CC) -c $(CFLAGS) $(CORE_INCS)  \
		-o objs/lib/ids/sk_mem_pool.o  \
		src/ids/sk_mem_pool.c	

objs/lib/ids/sk_mempool_list.o: $(CORE_DEPS)  \
	src/ids/sk_mempool_list.c
	$(CC) -c $(CFLAGS) $(CORE_INCS)  \
		-o objs/lib/ids/sk_mempool_list.o  \
		src/ids/sk_mempool_list.c	

objs/lib/ids/sk_rule.o: $(CORE_DEPS)  \
	src/ids/sk_rule.c
	$(CC) -c $(CFLAGS) $(CORE_INCS)  \
		-o objs/lib/ids/sk_rule.o  \
		src/ids/sk_rule.c	

objs/lib/ids/sk_sdlist.o: $(CORE_DEPS)  \
	src/ids/sk_sdlist.c
	$(CC) -c $(CFLAGS) $(CORE_INCS)  \
		-o objs/lib/ids/sk_sdlist.o  \
		src/ids/sk_sdlist.c			
		
objs/lib/os/sk_tools.o: $(CORE_DEPS)  \
	src/os/sk_tools.c
	$(CC) -c $(CFLAGS) $(CORE_INCS)  \
		-o objs/lib/os/sk_tools.o  \
		src/os/sk_tools.c	
		
objs/lib/ids/sk_ids_log.o: $(CORE_DEPS)  \
	src/ids/sk_ids_log.c
	$(CC) -c $(CFLAGS) $(CORE_INCS)  \
		-o objs/lib/ids/sk_ids_log.o  \
		src/ids/sk_ids_log.c		

objs/lib/ids/mwm.o: $(CORE_DEPS)  \
	src/ids/mwm.c
	$(CC) -c $(CFLAGS) $(CORE_INCS)  \
		-o objs/lib/ids/mwm.o  \
		src/ids/mwm.c		

objs/lib/ids/sk_ids_request.o: $(CORE_DEPS)  \
	src/ids/sk_ids_request.c
	$(CC) -c $(CFLAGS) $(CORE_INCS)  \
		-o objs/lib/ids/sk_ids_request.o  \
		src/ids/sk_ids_request.c			
	
objs/lib/add/cJSON.o: $(CORE_DEPS)  \
	src/add/cJSON.c
	$(CC) -c $(CFLAGS) $(CORE_INCS)  \
		-o objs/lib/add/cJSON.o  \
		src/add/cJSON.c	
objs/lib/add/json_http.o: $(CORE_DEPS)  \
	src/add/json_http.c
	$(CC) -c $(CFLAGS) $(CORE_INCS)  \
		-o objs/lib/add/json_http.o  \
		src/add/json_http.c	

MKDIRS := $(sort $(MKDIRS))


			
clean:
	rm -f $(C_LIB_CORE)/*.o
	rm -f $(C_LIB_EVENT)/*.o
	rm -f $(C_LIB_OS)/*.o
	rm -f $(C_LIB_NC)/*.o
	rm -f $(C_SBINDIR)/*

install:
	mkdir -p $(PREFIX)
	mkdir -p $(LIBDIR)
	mkdir -p $(SBINDIR)
	mkdir -p $(CONFDIR)

	cp -r $(C_LIBDIR)/*	 $(LIBDIR)
	cp -r $(C_SBINDIR)/* $(SBINDIR)
	cp -r src/conf/neosilk_conf.xml $(CONFDIR)

uninstall:
	rm -rf $(LIBDIR)
	rm -rf $(SBINDIR)
