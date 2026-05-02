/* No std */

#include <asm-generic/errno-base.h>
extern int  write(int fildes, const void* buf, unsigned long nbytes);
extern int  close(int fd);
extern long getdents64(int fd, void* dirp, unsigned long count);
extern int  openat(int fd, const char* path, int oflag, ...);

unsigned long strlen(const char *str) {
    const char* ptr;
    for (ptr = str; *ptr; ptr++);
    return ptr - str;
}

int strcmp(const char* a, const char* b) {
    while (*a && *a == *b) {
        a++; b++;
    }
    return (unsigned char)(*a) - (unsigned char)(*b);
}

#define stdout 1
#define stderr 2

#define BLUE "\033[34m"
#define RESET "\033[0m"

#define fprint(fd, s)   write(fd, s, strlen(s))
#define fprintln(fd, s) fprint(fd, s "\n")
#define print(s)        fprint(stdout, s)
#define println(s)      fprintln(stdout, s)
#define eprint(s)       fprint(stderr, s)
#define eprintln(s)     fprintln(stderr, s)

#define print_blue(s)   \
    print(BLUE); \
    print(s); \
    print(RESET)

/* Flags for openat() */
#define AT_FDCWD    -100     // Relative to current dir
#define O_DIRECTORY 00200000 // Must be a dir, or fails with errno
#define O_RDONLY    00000000 // Read only

/* Errno for openat() */
#define ENOTDIR     20
#define MACRO       13

/* Directory entry */
typedef struct {
    unsigned long  d_ino;    // 8 bytes
    unsigned long  d_off;    // 8 bytes
    unsigned short d_reclen; // 2 bytes
    unsigned char  d_type;   // 1 byte
    char           d_name[]; // d_reclen bytes, null ended string
} linux_dirent64;

// Entry types
#define DT_REG  8

void usage() {
    println("ls [flag] <path>\n");
    println("-a\tList all entries");
}

int main(int argc, char *argv[]) {
    if (argc > 3) {
        usage();
        return 1;
    }

    int show_all = 0;
    char* path = ".";
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "-a") == 0) {
                show_all = 1;
            } else {
                eprintln("Invalid flags");
                usage();
                return 1;
            }
        } else {
            path = argv[i];
        }
    }

    int fd = openat(AT_FDCWD, path, O_DIRECTORY | O_RDONLY, 0);
    if (fd < 0) {
        switch (-fd) {
            case (ENOTDIR): {
                eprintln("Not a directory");
                break;
            }
            case (EACCES): {
                eprintln("Permission denied");
                break;
            }
            default: {
                eprintln("Failed to open directory"); 
                break;
            }
        }
        return -fd;
    }

    unsigned char buf[128];  // buffer to store the dir entries
    int nread = 0;           // number of bytes read, 0 on end
    while ((nread = getdents64(fd, buf, sizeof(buf))) > 0) {
        for (int bpos = 0; bpos < nread; ) {
            linux_dirent64* ent = (linux_dirent64*)(buf + bpos);
            bpos += ent->d_reclen;

            if (ent->d_name[0] == '.' && show_all != 1)
                continue;  // this is a hidden entry, and -a flag is off

            if (ent->d_type == DT_REG) {
                print(ent->d_name);
            } else {
                print_blue(ent->d_name);
            }
            print("  ");

        }
    }

    print("\n");
 
    close(fd);
    return 0;
}
