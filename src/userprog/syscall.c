#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "devices/shutdown.h"
#include "userprog/process.h"
#include "threads/vaddr.h"
#include "threads/synch.h"
#include "threads/palloc.h"

#define BUFSIZE 4096

static void syscall_handler (struct intr_frame *);
int sys_halt (struct intr_frame *);
int sys_exit (struct intr_frame *);
int sys_write (struct intr_frame *);
int argptr (struct intr_frame *f, int n, void **pp);
int argint (struct intr_frame *f, int n, int *ip);
int arguint (struct intr_frame *f, int n, unsigned *ip);
int get_user (const uint8_t *uaddr);
size_t copy_from_user (void *dst_t, const void *src_, size_t size);

static int (*syscalls[]) (struct intr_frame *) =
  {
    [SYS_HALT]		sys_halt,
    [SYS_EXIT]		sys_exit,
    [SYS_EXEC]		NULL, //sys_exec,
    [SYS_WAIT]		NULL, //sys_wait,
    [SYS_CREATE]	NULL, //sys_create,
    [SYS_REMOVE]	NULL, //sys_remove,
    [SYS_OPEN]		NULL, //sys_open,
    [SYS_FILESIZE]	NULL, //sys_filesize,
    [SYS_READ]		NULL, //sys_read,
    [SYS_WRITE]		sys_write,
    [SYS_SEEK]		NULL, //sys_seek,
    [SYS_TELL]		NULL, //sys_tell,
    [SYS_CLOSE]		NULL, //sys_close,
    [SYS_MMAP]		NULL, //sys_mmap,
    [SYS_MUNMAP]	NULL, //sys_munmap,
    [SYS_CHDIR]		NULL, //sys_chdir,
    [SYS_MKDIR]		NULL, //sys_mkdir,
    [SYS_READDIR]	NULL, //sys_readdir,
    [SYS_ISDIR]		NULL, //sys_isdir,
    [SYS_INUMBER]	NULL, //sys_inumber,
  };

/* Reads a byte at user virtual address UADDR.
 *    UADDR must be below PHYS_BASE.
 *       Returns the byte value if successful, -1 if a segfault
 *          occurred. */
__attribute__ ((noinline)) // use to check context in page_fault()
int get_user (const uint8_t *uaddr)
{
  int result;
  asm ("movl $1f, %0; movzbl %1, %0; 1:"
       : "=&a" (result) : "m" (*uaddr));
  return result;
}

size_t copy_from_user (void *dst_, const void *src_, size_t size)
{
  uint8_t *dst = dst_;
  const uint8_t *src = src_;
  int c;
  int ret = 0;

  if (size <= 0 || src + size >= (uint8_t *)PHYS_BASE
      || dst == NULL || src == NULL)
    return -1;

  while (size-- > 0)
  {
    c = get_user (src++);
    if (c == -1)
      {
        ret = -1;
        break;
      }
    *dst++ = (uint8_t)c;
  }
  return ret;
}

int argint (struct intr_frame *f, int n, int *ip)
{
  uint32_t addr;
  if (n < 0)
    return -1;
  addr = (uint32_t)f->esp + 4 * n; // skip syscall number
  if (addr >= (uint32_t) PHYS_BASE)
    return -1;

  return copy_from_user (ip, (int *)addr, sizeof(int));
}

int arguint (struct intr_frame *f, int n, unsigned *ip)
{
  int i;
  if (n <= 0)
    return -1;
  if(argint (f, n, &i) < 0)
    return -1;
  *ip = (unsigned) i;
  return 0;
}

int argptr (struct intr_frame *f, int n, void **pp)
{
  int i;
  if (n <= 0)
    return -1;
  if(argint (f, n, &i) < 0)
    return -1;
  *pp = (void *) i;
  return 0;
}

void
syscall_init (void)
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f)
{
  int sys_no;
  if (argint (f, 0, &sys_no))
    thread_exit_with_reason (EXIT_UNNORMAL);

  if(sys_no > SYS_HALT && sys_no < SYS_INUMBER && syscalls[sys_no])
    f->eax = syscalls[sys_no] (f);
  else
    thread_exit_with_reason (EXIT_UNNORMAL);
}

int sys_halt (struct intr_frame *f UNUSED)
{
  shutdown_power_off ();
  return 0;
}

int sys_exit (struct intr_frame *f)
{
  int status;

  if (argint (f, 1, &status))
    status = -1;
  thread_exit_with_reason (status);
  return 0;
}

int sys_write (struct intr_frame *f)
{
  int fd;
  void *buffer;
  unsigned size;
  char *kern_buffer;
  int ret = -1;

  if (argint (f, 1, &fd) || argptr (f, 2, &buffer) || arguint (f, 3, &size))
    return ret;

  if (fd == STDOUT_FILENO)
  {
    kern_buffer = palloc_get_page (0);
    copy_from_user (kern_buffer, buffer, size);
    putbuf (kern_buffer, size);
    ret = 0;
    palloc_free_page (kern_buffer);
  }
  return ret;
}
