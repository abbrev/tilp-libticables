#ifndef __CONFIG_H__
#define __CONFIG_H__

#ifdef __MACOSX__

#define VERSION "3.1.0 OS X 0.1.0"
#define HAVE_SYS_PERM_H
#define HAVE_SYS_IPC_H  /* needed for virtual linking */
#define HAVE_SYS_SHM_H
#define HAVE_VPRINTF

#endif __MACOSX__

#endif /* !__CONFIG_H__ */