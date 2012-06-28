# $FreeBSD: projects/armv6/lib/libthr/thread/Makefile.inc 218414 2011-02-07 21:26:46Z jkim $

# thr sources
.PATH: ${.CURDIR}/thread

SRCS+= \
	thr_affinity.c \
	thr_attr.c \
	thr_barrier.c \
	thr_barrierattr.c \
	thr_cancel.c \
	thr_clean.c \
	thr_concurrency.c \
	thr_cond.c \
	thr_condattr.c \
	thr_create.c \
	thr_detach.c \
	thr_equal.c \
	thr_event.c \
	thr_exit.c \
	thr_fork.c \
	thr_getprio.c \
	thr_getcpuclockid.c \
	thr_getschedparam.c \
	thr_getthreadid_np.c \
	thr_info.c \
	thr_init.c \
	thr_join.c \
	thr_list.c \
	thr_kern.c \
	thr_kill.c \
	thr_main_np.c \
	thr_multi_np.c \
	thr_mutex.c \
	thr_mutexattr.c \
	thr_once.c \
	thr_printf.c \
	thr_pspinlock.c \
	thr_resume_np.c \
	thr_rtld.c \
	thr_rwlock.c \
	thr_rwlockattr.c \
	thr_self.c \
	thr_sem.c \
	thr_setprio.c \
	thr_setschedparam.c \
	thr_sig.c \
	thr_single_np.c \
	thr_sleepq.c \
	thr_spec.c \
	thr_spinlock.c \
	thr_stack.c \
	thr_syscalls.c \
	thr_suspend_np.c \
	thr_switch_np.c \
	thr_symbols.c \
	thr_umtx.c \
	thr_yield.c