#ifndef __COHOOKEX_HPP__
#define __COHOOKEX_HPP__

#ifdef __cplusplus
extern "C"
{
#endif

	int co_pthread_create(pthread_t *tidp, const pthread_attr_t *attr, void *(*start_rtn)(void *), void *arg);
	int co_pthread_join(pthread_t thread, void **retval);
	int co_pthread_detach(pthread_t thread);

#ifdef __cplusplus
}
#endif
#endif